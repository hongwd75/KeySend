
// keysenderDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "keysender.h"
#include "keysenderDlg.h"
#include "afxdialogex.h"
#include "virtualCode.h"
#include <vector>
#include <psapi.h>
#include <strsafe.h>
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
struct handle_data {
	unsigned long process_id;
	HWND best_handle;
};


extern CString GetPath(int nFolder); // 0 : userdata / 1: current

UINT PacketParsingThread(LPVOID pParam);

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CkeysenderDlg 대화 상자

HCURSOR	CkeysenderDlg::g_hCursorSearchWindow = NULL;
BOOL CkeysenderDlg::g_isFullScrrenMode = FALSE;
BOOL CkeysenderDlg::g_styleHelper = FALSE;

CkeysenderDlg *CkeysenderDlg::gDlg = nullptr;
CkeysenderDlg::CkeysenderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CkeysenderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_bStartSearchWindow = FALSE;
	g_hwndFoundWindow = NULL;
	g_hRectanglePen = NULL;
	m_castDlg = nullptr;
	m_pThread = nullptr;

	gDlg = this;

}

void CkeysenderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BIND, m_bindList);
	DDX_Control(pDX, IDC_COMBO_WINDOWLIST, m_processList);
}

BEGIN_MESSAGE_MAP(CkeysenderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_INPUT()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_LOAD_BTN, &CkeysenderDlg::OnBnClickedLoadBtn)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
//	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &CkeysenderDlg::OnItemdblclickListBind)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BIND, &CkeysenderDlg::OnItemchangedListBind)
	ON_BN_CLICKED(IDC_EDIT_BINDOBJECT, &CkeysenderDlg::OnBnClickedEditBindobject)
	ON_BN_CLICKED(IDC_NEW_BINDOBJECT, &CkeysenderDlg::OnBnClickedNewBindobject)
	ON_BN_CLICKED(IDC_REMOVE_BINDOBJECT, &CkeysenderDlg::OnBnClickedRemoveBindobject)
	ON_BN_CLICKED(IDC_SAVE_BINDLIST, &CkeysenderDlg::OnBnClickedSaveBindlist)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_BIND, &CkeysenderDlg::OnNMDblclkListBind)
	ON_CBN_SELCHANGE(IDC_COMBO_WINDOWLIST, &CkeysenderDlg::OnSelchangeComboWindowlist)
	ON_BN_CLICKED(IDC_CASTHELPBTN, &CkeysenderDlg::OnBnClickedCasthelpbtn)
	ON_BN_CLICKED(IDC_CHECK_USE_FULLSCREEN, &CkeysenderDlg::OnBnClickedCheckUseFullscreen)
	ON_BN_CLICKED(IDC_CHATLOGBTN, &CkeysenderDlg::OnBnClickedChatlogbtn)
END_MESSAGE_MAP()


void CkeysenderDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	EndEventCapture();
	UninitialiseResources();
	CDialogEx::OnClose();
}

BOOL CkeysenderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	m_HoldCheckBind = false;
	InitialiseResources();
	StartEventCapture();
	SetTimer(1002, 5, NULL);

	m_myImageList.Create(16, 16, ILC_COLOR8 | ILC_MASK, 8, 1);
	CBitmap bm,bm2;
	bm.LoadBitmap(IDB_BITMAP1);
	m_myImageList.Add(&bm, RGB(0, 0, 0));
	bm2.LoadBitmap(IDB_BITMAP2);
	m_myImageList.Add(&bm2, RGB(0, 0, 0));
	
	m_bindList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT); // 타이틀 삽입 
	m_bindList.InsertColumn(0, _T("단축키"), LVCFMT_CENTER, 100, -1);
	m_bindList.InsertColumn(1, _T("설명"), LVCFMT_CENTER, 206, -1);

	m_bindList.SetImageList(&m_myImageList, LVSIL_SMALL);
	UpdateWindowProcessList();
	m_processList.SetCurSel(0);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CkeysenderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CkeysenderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
HCURSOR CkeysenderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CkeysenderDlg::PreTranslateMessage(MSG* pMsg)
{
	//// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if ( pMsg->message == WM_KEYUP)
	{
		CString mmm;
		mmm.Format(_T("* WM_KEYUP -   w:%08x, l:%08x\r\n"), pMsg->wParam, pMsg->lParam);
		TRACE(mmm);
	} else
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return true;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CkeysenderDlg::updateTargetWidowState(void)
{
	if (g_hwndFoundWindow == nullptr)
	{
		activeMacroObjectManager::targetWindowPosition.x = 0;
		activeMacroObjectManager::targetWindowPosition.y = 0;
		KillTimer(1010);
	}
	RECT rect;
	::GetWindowRect(g_hwndFoundWindow, &rect);
	activeMacroObjectManager::targetWindowPosition.x = rect.left + activeMacroObjectManager::targetWindowBorderSize.x;
	activeMacroObjectManager::targetWindowPosition.y = rect.top + activeMacroObjectManager::targetWindowBorderSize.y;
}


void CkeysenderDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (1002 == nIDEvent)
	{
		DWORD nn = GetTickCount();
		m_activeMacro.onTick(nn);
	} else
	if (1010 == nIDEvent)
	{
		updateTargetWidowState();
	}
	CDialogEx::OnTimer(nIDEvent);
}



//////////////////////////////////////////////////////////////////////////////////////////
//
void CkeysenderDlg::paintListControl(KeyBindSTATE bindKind, DWORD keyvalue, bool on)
{
	//isActive
	int itemIndex = bindKind * 1000 + keyvalue;
	int max = m_bindList.GetItemCount();
	for (int i = 0; i < max; i++)
	{
		if (m_bindList.GetItemData(i) == itemIndex)
		{
			m_bindList.SetItem(i, 0,LVIF_IMAGE, NULL, (int)!on, 0, 0, 0);
		}
	}
}
void CkeysenderDlg::UpdateBindList(void)
{
	int cnt = 0;
	int itemcnt = 0;
	m_bindList.DeleteAllItems();
	auto itemlist = m_MacroList.getList();
	for (auto &item : *itemlist)
	{
		cnt = m_bindList.InsertItem(0,item->getBindString().c_str(),1);
		m_bindList.SetItem(cnt, 1, LVIF_TEXT, item->getTitle().c_str(), 0, 0, 0, NULL);

		int keyreg = item->getBindState() * 1000;
		keyreg += item->getBindValue();
		m_bindList.SetItemData(cnt, keyreg);
		cnt++;
		itemcnt++;
	}

}


///////////////////////////////////////////////////////////////////////
//
bool PrintProcessNameAndID(DWORD processID, std::wstring &out, HANDLE &hProcess)
{
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

	// Get a handle to the process.
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);

	// Get the process name.

	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
			&cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName,
				sizeof(szProcessName) / sizeof(TCHAR));
		}

		out = szProcessName;
	}
	CloseHandle(hProcess);

	return (bool)(out.compare(_T("<unknown>")) != 0);
}



///////////////////////////////////////////////////////////////////////
//
void CkeysenderDlg::UpdateWindowProcessList(void)
{
	m_processList.ResetContent();
	int cnt = m_processList.AddString(_T("= 특정 윈도우로 설정하지 않음 ="));
	m_processList.SetItemData(cnt, 0);
	cnt = m_processList.AddString(_T("* 윈도우 리스트를 갱신 *"));
	m_processList.SetItemData(cnt, 1);

	DWORD dwSize = 0;
	DWORD dwProcIdentifiers[10000] = { 0 };
	EnumProcesses(dwProcIdentifiers, sizeof(dwProcIdentifiers), &dwSize);
	if (!dwSize)
	{
		return;
	}

	DWORD cProcesses = dwSize / sizeof(DWORD);

	// Print the name and process identifier for each process.
	std::wstring  winName;
	HANDLE wHandle;
	for (int i = 0; i < cProcesses; i++)
	{
		if (dwProcIdentifiers[i] != 0)
		{
			if (PrintProcessNameAndID(dwProcIdentifiers[i], winName, wHandle) == true)
			{
				if (winName.length() > 1)
				{
					cnt = m_processList.AddString(winName.c_str());
					m_processList.SetItemData(cnt, dwProcIdentifiers[i]);
				}
			}
		}
	}
}

void CkeysenderDlg::OnBnClickedLoadBtn()
{
	TCHAR szFilter[] = _T("Text (*.text, *.json, *.txt) | *.text;*.txt;*.json | All Files(*.*)|*.*||");

	m_HoldCheckBind = true;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

	if (IDOK == dlg.DoModal()) {
		m_activeMacro.stopAllBindMacro();
		if (m_MacroList.Load((TCHAR*)dlg.GetPathName().GetString()) == true)
		{
			UpdateBindList();
		}
	}
	m_HoldCheckBind = false;
}


/////////////////////////////////////////////////////////////////////////////////
//
void CkeysenderDlg::OnItemchangedListBind(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uOldState & LVNI_SELECTED) && !(pNMLV->uNewState & LVNI_SELECTED))
	{

		//int at = (int)pNMLV->lParam;
		//auto itemlist = m_MacroList.getList();
		//if (itemlist->size() > at)
		//{
		//	auto value = itemlist->at(at);
		//}
	}

	*pResult = 0;
}




void CkeysenderDlg::OnNMDblclkListBind(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem >=0)
	{
		OnBnClickedEditBindobject();
	}
	else
	{
		OnBnClickedNewBindobject();
	}
	*pResult = 0;
}


BOOL is_main_window(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}
HWND FindTopWindow(DWORD pid)
{
	std::pair<HWND, DWORD> params = { 0, pid };

	// Enumerate the windows using a lambda to process each window
	BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
	{
		auto pParams = (std::pair<HWND, DWORD>*)(lParam);

		DWORD processId;
		if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second && is_main_window(hwnd) == TRUE)
		{
			// Stop enumerating
			SetLastError(-1);
			pParams->first = hwnd;
			return FALSE;
		}

		// Continue enumerating
		return TRUE;
	}, (LPARAM)&params);

	if (!bResult && GetLastError() == -1 && params.first)
	{
		return params.first;
	}

	return 0;
}

extern CString GetPath(int nFolder);
void CkeysenderDlg::OnSelchangeComboWindowlist()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int sel = m_processList.GetCurSel();

	if (sel < 2)
	{
		if (sel == 1)
		{
			UpdateWindowProcessList();
			m_processList.SetCurSel(0);
		}
		activeMacroObjectManager::targetWindowPosition.x = 0;
		activeMacroObjectManager::targetWindowPosition.y = 0;
		KillTimer(1010);
	}
	else
	{
		KillTimer(1010);
		activeMacroObjectManager::targetWindowPosition.x = 0;
		activeMacroObjectManager::targetWindowPosition.y = 0;

		HWND hwnd = FindTopWindow(m_processList.GetItemData(sel));
		if (hwnd != 0)
		{
			DisplayInfoOnFoundWindow(hwnd);
			if (g_hwndFoundWindow)
			{
				RefreshWindow(g_hwndFoundWindow);
			}
			g_hwndFoundWindow = hwnd;

			this->m_activeMacro.setTargetWindowWidthHwnd(g_hwndFoundWindow);
			SetTimer(1010, 12000,NULL);
			updateTargetWidowState();

			MessageBox(GetPath(1));
		}
		else
		{
			MessageBox(_T("설정할 수 없는 윈도우 입니다."), _T("타겟 윈도우 설정 오류"));
		}
	}
}


void CkeysenderDlg::OnBnClickedCasthelpbtn()
{
	if (m_castDlg == nullptr)
	{
		m_castDlg = new castHelperDLG;
		m_castDlg->Create(IDD_FASTCASTGROUPDLG, this);
		m_castDlg->ShowWindow(SW_SHOW);
		m_castDlg->doShow();
	}
	return;
}


void CkeysenderDlg::OnBnClickedCheckUseFullscreen()
{
	CkeysenderDlg::g_isFullScrrenMode = (bool)(this->IsDlgButtonChecked(IDC_CHECK_USE_FULLSCREEN) != 0);
}

// 스타일/케스팅 도우미
void CkeysenderDlg::OnBnClickedChatlogbtn()
{
	if (g_styleHelper == FALSE)
	{
		m_chatLogFile = _T("D:\\testoo");//GetPath(0) + _T("chat.log");
		m_pThread = AfxBeginThread(PacketParsingThread, this);
		g_styleHelper = TRUE;
	}
	else 
	{
		DWORD nExitCode = NULL;
		GetExitCodeThread(m_pThread->m_hThread, &nExitCode);
		TerminateThread(m_pThread->m_hThread, nExitCode);
		g_styleHelper = FALSE;
	}
}

BYTE globalLogBuffer[1024 * 1024];

UINT PacketParsingThread(LPVOID pParam)
{
	HANDLE hDir = CreateFileW(CkeysenderDlg::gDlg->m_chatLogFile.GetString(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
	CONST DWORD cbBuffer = 1024 * 1024;
	BYTE* pBuffer = (PBYTE)&globalLogBuffer;
	BOOL bWatchSubtree = FALSE;
	//DWORD dwNotifyFilter = FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION;
	DWORD dwNotifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE;
	DWORD bytesReturned;
	WCHAR temp[MAX_PATH] = { 0 };

	for (;;)
	{
		FILE_NOTIFY_INFORMATION* pfni;
		BOOL fOk = ReadDirectoryChangesW(hDir, pBuffer, cbBuffer,
			bWatchSubtree, dwNotifyFilter, &bytesReturned, 0, 0);
		if (!fOk)
		{
			DWORD dwLastError = GetLastError();
			printf("error : %d\n", dwLastError);
			break;
		}

		pfni = (FILE_NOTIFY_INFORMATION*)pBuffer;

		do {
			printf("NextEntryOffset(%d)\n", pfni->NextEntryOffset);
			switch (pfni->Action)
			{
			case FILE_ACTION_ADDED:
				TRACE(L"FILE_ACTION_ADDED\n");
				break;
			case FILE_ACTION_REMOVED:
				TRACE(L"FILE_ACTION_REMOVED\n");
				break;
			case FILE_ACTION_MODIFIED:
				TRACE(L"FILE_ACTION_MODIFIED \n");
				break;
			default:
				break;
			}
			printf("FileNameLength(%d)\n", pfni->FileNameLength);

			StringCbCopyNW(temp, sizeof(temp), pfni->FileName, pfni->FileNameLength);

			wprintf(L"FileName(%s)\n", temp);

			pfni = (FILE_NOTIFY_INFORMATION*)((PBYTE)pfni + pfni->NextEntryOffset);
		} while (pfni->NextEntryOffset > 0);
	}

	return 0;
}
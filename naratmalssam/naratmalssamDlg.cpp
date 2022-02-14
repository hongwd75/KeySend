
// naratmalssamDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "naratmalssam.h"
#include "naratmalssamDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

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


// CnaratmalssamDlg 대화 상자



CnaratmalssamDlg::CnaratmalssamDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CnaratmalssamDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CnaratmalssamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBOPROCESS, m_processList);
}

BEGIN_MESSAGE_MAP(CnaratmalssamDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBOPROCESS, &CnaratmalssamDlg::OnSelchangeComboprocess)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CnaratmalssamDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CnaratmalssamDlg 메시지 처리기

BOOL CnaratmalssamDlg::OnInitDialog()
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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	//setHOOK(0);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CnaratmalssamDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CnaratmalssamDlg::OnPaint()
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
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
		::TextOutA(dc.GetSafeHdc(), 0, 0, "100", 3);
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CnaratmalssamDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





void CnaratmalssamDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	unHOOK(0);
}



BOOL InjectDll(DWORD dwPID, LPCTSTR szDllName)
{
	int ForError = 0;
	HANDLE hProcess, hThread;
	HMODULE hMod;
	LPVOID pRemoteBuf;
	DWORD dwBufSize = lstrlen(szDllName) + 1;
	LPTHREAD_START_ROUTINE pThreadProc;

	// dwPID 를 이용하여 대상 프로세스의 HANDLE을 구함
	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))  //실패시 NULL 성공시 프로세스핸들 
		return FALSE;

	// 대상 프로세스메모리에 szDllName(dwBufSize) 크기만큼 메모리를 할당 
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE); //성공시 할당된 메모리 주소(대상 프로세스 가상메모리) 

	// 할당 받은 메모리에 myhack.dll 경로를 씀
	ForError = WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllName, dwBufSize, NULL);
	if (!ForError)
		return FALSE;


	// LoadLibraryA() API 주소를 구함 
	hMod = GetModuleHandle(_T("kernel32.dll"));
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");

	// 대상프로세스에 스레드를 실행 
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	//WaitForSingleObject(hThread, INFINITE);

	//CloseHandle(hThread);
	CloseHandle(hProcess);

	printf("모두 끝남");

	return TRUE;
}

extern int GetFirstThreadInProcess(int pid);
extern bool DoInjection(HANDLE hProcess, HANDLE hThread, PCSTR dllPath);
void simple_decodeMythicRC4Packet(unsigned char *buff, int offset, int len);


void CnaratmalssamDlg::OnBnClickedButton1()
{
	unsigned char data[] = { 
		0x00, 0x18, 0xdc, 0x11, 0x06, 0x48, 0xde, 0xd9, 0xf4, 0x15,
		0x6c, 0x23, 0x4c, 0xd6, 0xd5, 0xa5, 0x79, 0x7a, 0x9d, 0xf2,
		0x29, 0x0c, 0x73, 0x92, 0xc9, 0x79, 0xf3, 0xd7, 0xe5 };


	simple_decodeMythicRC4Packet((unsigned char*)(data + 3), 0xf3, 0x18);

	
	return;

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	HANDLE hProcess, hThread;
	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			if (_tcsicmp(entry.szExeFile, _T("uth.dll")) == 0) {

				HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, entry.th32ProcessID);
				auto hThread = ::OpenThread(THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, FALSE, GetFirstThreadInProcess(entry.th32ProcessID));
				auto th =  DoInjection(hProcess, hThread, "d:\\AppInitHook.dll");

				if (th == true)
				{
					MessageBox(_T("OKOK"));
				}
				else MessageBox(_T("FALSE"));
				//InjectDll(entry.th32ProcessID, _T("d:\\AppInitHook.dll"));
				/*
				char dirPath[MAX_PATH];
				char fullPath[MAX_PATH];

				GetCurrentDirectoryA(MAX_PATH, dirPath);

				sprintf_s(fullPath, MAX_PATH, "%s\\AppInitHook.dll", dirPath);

				HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, entry.th32ProcessID);
				LPVOID libAddr = (LPVOID)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryA");
				LPVOID llParam = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(fullPath) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

				int error = WriteProcessMemory(hProcess, llParam, (LPVOID)fullPath, strlen(fullPath)+1, NULL);
				hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)libAddr, llParam, NULL, NULL);
				CloseHandle(hProcess);
				*/
			}
		}
	}

	CloseHandle(snapshot);

	this->InvalidateRect(NULL);
}


////////////////////////////////////////
/////////////////////////////////////////
unsigned char t_sbox[] 
{
	0x4B, 0x8C, 0x96, 0x62, 0x0C, 0x29, 0xEF, 0x43, 0x0E, 0x7D, 0x7A, 0x46, 0xB9, 0x56, 0x00, 0x5F, 0x68,
	0x2F, 0x47, 0x31, 0xA1, 0x81, 0x4D, 0x55, 0x92, 0x88, 0x5B, 0xB5, 0xA5, 0x30, 0x8D, 0x08, 0x19, 0x63,
	0x9D, 0xE0, 0x42, 0xDA, 0xEE, 0xCA, 0x3F, 0x6B, 0x5E, 0x7E, 0x71, 0xCB, 0x5A, 0xF3, 0x06, 0x0D, 0x66,
	0x41, 0xEA, 0x8B, 0xEC, 0xB0, 0x10, 0x28, 0x93, 0x8A, 0x05, 0x1C, 0xC1, 0xF2, 0xF5, 0x7B, 0x4E, 0xB3,
	0x18, 0x70, 0xB8, 0x65, 0xAA, 0x74, 0xA3, 0xBA, 0xA0, 0x82, 0x6F, 0xDD, 0x53, 0xDB, 0xBF, 0x02, 0x36,
	0x32, 0xF6, 0x94, 0x16, 0x8F, 0x6E, 0x86, 0xFE, 0xD4, 0xC3, 0x27, 0x2C, 0x50, 0xC4, 0x1D, 0xC8, 0xFF,
	0xD8, 0x3A, 0x04, 0x3D, 0x0B, 0x80, 0x5C, 0x73, 0xAF, 0xA6, 0xBD, 0x2B, 0x8E, 0x23, 0x77, 0x4C, 0x7C,
	0x79, 0xD6, 0x13, 0x3C, 0xAC, 0x20, 0x60, 0xF1, 0x14, 0xE3, 0xAD, 0x95, 0xFB, 0x0A, 0x26, 0x7F, 0xCE,
	0xBB, 0x72, 0xE7, 0xD3, 0x3B, 0x09, 0xB7, 0x17, 0xE5, 0x99, 0xE1, 0x24, 0xE6, 0x9A, 0x48, 0x2A, 0xF4,
	0x6A, 0xF0, 0xD1, 0xD7, 0x6D, 0xDE, 0x69, 0xB1, 0xFA, 0x59, 0x2E, 0x2D, 0x45, 0xC2, 0x89, 0x4A, 0xF7,
	0x40, 0x91, 0x6C, 0xB4, 0xC9, 0xA9, 0xFC, 0x90, 0xD2, 0x44, 0x84, 0x11, 0xA7, 0xFD, 0xA4, 0x01, 0x12,
	0xA8, 0x98, 0xF8, 0x76, 0xEB, 0xCC, 0x1A, 0x54, 0x0F, 0xAE, 0xC6, 0x9F, 0x58, 0x35, 0xE8, 0x87, 0xE2,
	0x5D, 0xD9, 0x97, 0x75, 0xD5, 0x67, 0x51, 0x21, 0xCD, 0x64, 0xD0, 0xC5, 0x1E, 0x25, 0x15, 0x57, 0xBC, 
	0x33, 0xE4, 0x34, 0x61, 0x37, 0xE9, 0x39, 0xB2, 0xBE, 0x07, 0x03, 0x1B, 0xED, 0x52, 0x22, 0x83, 0x38,
	0x9B, 0xB6, 0xC7, 0xDC, 0xCF, 0xA2, 0xC0, 0xF9, 0x9C, 0xDF, 0xAB, 0x78, 0x1F, 0x3E, 0x85, 0x4F, 0x9E, 0x49 
};

unsigned char t_crypted[] 
{
		0x00, 0x98, 0xF4, 0xF2, 0x3D, 0x50, 0xC5, 0x95, 0xA0, 0x12,
		0x9A, 0x0B, 0xFB, 0x5E, 0xF1, 0x61, 0x7D, 0xA1, 0x28, 0x59,
		0x68, 0x3E, 0xAF, 0x91, 0x2B, 0x24, 0x3B, 0xE6, 0x69, 0xA5, 
		0x31, 0x1E, 0x5D, 0x37, 0xCA, 0xCB, 0xCB, 0x2B, 0xFF, 0x8E,
		0x1A, 0xE8, 0x15, 0x0D, 0x5B, 0x15, 0x5A, 0x1C, 0x3E, 0xD2,
		0xD6, 0x23, 0x0B, 0x33, 0x9B, 0x58, 0x41, 0x5C, 0xDC, 0x5E, 
		0x28, 0xFF, 0x7F, 0x0E, 0x7B, 0x66, 0xA6, 0x9F, 0xF6, 0x8C,
		0x8D, 0x29, 0xBC, 0x4C, 0xC5, 0xBE, 0x39, 0x97, 0x56, 0x63,
		0xE4, 0xA9, 0xD4, 0xB4, 0x5E, 0xD0, 0xAD, 0x9F, 0x74, 0x62,
		0x77, 0x73, 0xDB, 0x42, 0x6A, 0x81, 0x1C, 0x87, 0xD6, 0x86,
		0x29, 0x07, 0xA2, 0xF7, 0xC8, 0xC9, 0xA4, 0x4B, 0xEF, 0xCA 
};

unsigned char t_decrypted[] {	0x00,
								0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA7, 0x00, 0x36, 0x01, 0x0B,
								0x03, 0x74, 0x68, 0x65, 0x6B, 0x72, 0x6F, 0x6B, 0x6F, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74,
								0x68, 0x65, 0x6B, 0x72, 0x6F, 0x6B, 0x6F, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x89,
								0xDA
};

void decodeMythicRC4Packet(unsigned char *buff, int offset, int len, unsigned char *sbox, bool udp)
{
	unsigned char nBox[256];
	memcpy(&nBox, sbox, sizeof(unsigned char) * 256);

	int x = 0; 
	int y = 0;
	int tmp;

	int midpoint = len / 2;
	int pos = 0;

	for (pos = midpoint; pos < len; pos++)
	{
		x++;
		y += nBox[x];
		tmp = nBox[x];
		nBox[x] = nBox[y];
		nBox[y] = tmp;

		tmp = (nBox[x] + nBox[y]) & 0xFF;
		buff[pos + offset] ^= nBox[tmp];
		y += buff[pos + offset];
	}


	for (pos = 0; pos < midpoint; pos++) {
		x++;
		y += nBox[x];
		tmp = nBox[x];
		nBox[x] = nBox[y];
		nBox[y] = tmp;
		tmp = (nBox[x] + nBox[y]) & 0xFF;
		buff[pos + offset] ^= nBox[tmp];
		y += buff[pos + offset]; // this is not standard RC4 here
	}
}


void simple_decodeMythicRC4Packet(unsigned char *buff, int offset, int len)
{
	decodeMythicRC4Packet(buff, offset, len, &t_sbox[0], true);
}
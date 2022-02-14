
// hanDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "han.h"
#include "hanDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ChanDlg 대화 상자



ChanDlg::ChanDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ChanDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ChanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ChanDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &ChanDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

// Global variable.
HWINEVENTHOOK g_hook;
void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread, DWORD dwmsEventTime);

// ChanDlg 메시지 처리기
void InitializeMSAA()
{
	CoInitialize(NULL);
	g_hook = SetWinEventHook(
		EVENT_SYSTEM_MENUSTART, EVENT_SYSTEM_MENUEND,  // Range of events (4 to 5).
		NULL,                                          // Handle to DLL.
		HandleWinEvent,                                // The callback.
		0, 0,              // Process and thread IDs of interest (0 = all)
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS); // Flags.
}

// Unhooks the event and shuts down COM.
//
void ShutdownMSAA()
{
	UnhookWinEvent(g_hook);
	CoUninitialize();
}

HMODULE hHookEngineDll;
BOOL(__cdecl *HookFunction)(ULONG_PTR OriginalFunction, ULONG_PTR NewFunction);
VOID(__cdecl *UnhookFunction)(ULONG_PTR Function);
ULONG_PTR(__cdecl *GetOriginalFunction)(ULONG_PTR Hook);

BOOL WINAPI MyDrawTextA(HDC hdc, int x, int y, LPCSTR lpszString, int cbString)
{
	BOOL (WINAPI *pDrawText)(HDC _hdc, int _x, int _y, LPCSTR _lpszString, int _cbString);
	pDrawText = (int(WINAPI*)(HDC, int, int, LPCSTR,int)) GetOriginalFunction((ULONG_PTR)MyDrawTextA);
	TRACE("##TEXTOUTA:%s\n", lpszString);

	return pDrawText(hdc, x, y, lpszString,cbString);
}
BOOL WINAPI MyDrawTextW(HDC hdc, int x, int y, LPCWSTR lpszString, int cbString)
{
	BOOL (WINAPI *pDrawText)(HDC _hdc, int _x, int _y, LPCWSTR _lpszString, int _cbString);
	pDrawText = (int(WINAPI*)(HDC, int, int, LPCWSTR, int)) GetOriginalFunction((ULONG_PTR)MyDrawTextW);
	TRACE(_T("##TEXTOUTW:%s\n"), lpszString);

	return pDrawText(hdc, x, y, lpszString, cbString);
}

BOOL ChanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	InitializeMSAA();

	hHookEngineDll = LoadLibrary(_T("NtHookEngine.dll"));
	HookFunction = (BOOL(__cdecl *)(ULONG_PTR, ULONG_PTR))
		GetProcAddress(hHookEngineDll, "HookFunction");

	UnhookFunction = (VOID(__cdecl *)(ULONG_PTR))
		GetProcAddress(hHookEngineDll, "UnhookFunction");

	GetOriginalFunction = (ULONG_PTR(__cdecl *)(ULONG_PTR))
		GetProcAddress(hHookEngineDll, "GetOriginalFunction");

	if (HookFunction == NULL || UnhookFunction == NULL ||
		GetOriginalFunction == NULL)
	{
		MessageBox(_T("초기화 실패"));
	}
	HookFunction((ULONG_PTR)GetProcAddress(LoadLibrary(_T("gdi32.dll")),
		"TextOutA"),
		(ULONG_PTR)&MyDrawTextA);
	HookFunction((ULONG_PTR)GetProcAddress(LoadLibrary(_T("gdi32.dll")),
		"TextOutW"),
		(ULONG_PTR)&MyDrawTextW);
	//SysFunc1=(DWORD)GetProcAddress(GetModuleHandle("gdi32.dll"),"TextOutA"); 
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void ChanDlg::OnPaint()
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
HCURSOR ChanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Callback function that handles events.
//
void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread, DWORD dwmsEventTime)
{
	IAccessible* pAcc = NULL;
	VARIANT varChild;
	HRESULT hr = AccessibleObjectFromEvent(hwnd, idObject, idChild, &pAcc, &varChild);
	if ((hr == S_OK) && (pAcc != NULL))
	{
		BSTR bstrName;
		pAcc->get_accName(varChild, &bstrName);
		if (event == EVENT_SYSTEM_MENUSTART)
		{
			TRACE(_T("Begin: "));
		}
		else if (event == EVENT_SYSTEM_MENUEND)
		{
			TRACE(_T("End:   "));
		}
		TRACE(_T("%s\n"), bstrName);
		SysFreeString(bstrName);
		pAcc->Release();
	}
}

void ChanDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ShutdownMSAA();
	UnhookFunction((ULONG_PTR)GetProcAddress(LoadLibrary(_T("gdi32.dll")),
		"TextOutA"));
	UnhookFunction((ULONG_PTR)GetProcAddress(LoadLibrary(_T("gdi32.dll")),
		"TextOutW"));
	CDialogEx::OnClose();
}


void ChanDlg::OnBnClickedButton1()
{
	this->InvalidateRect(NULL);
}

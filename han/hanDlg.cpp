
// hanDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "han.h"
#include "hanDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ChanDlg ��ȭ ����



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

// ChanDlg �޽��� ó����
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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
		MessageBox(_T("�ʱ�ȭ ����"));
	}
	HookFunction((ULONG_PTR)GetProcAddress(LoadLibrary(_T("gdi32.dll")),
		"TextOutA"),
		(ULONG_PTR)&MyDrawTextA);
	HookFunction((ULONG_PTR)GetProcAddress(LoadLibrary(_T("gdi32.dll")),
		"TextOutW"),
		(ULONG_PTR)&MyDrawTextW);
	//SysFunc1=(DWORD)GetProcAddress(GetModuleHandle("gdi32.dll"),"TextOutA"); 
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void ChanDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.
		::TextOutA(dc.GetSafeHdc(), 0, 0, "100", 3);
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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

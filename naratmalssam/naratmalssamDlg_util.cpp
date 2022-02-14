#include "stdafx.h"
#include "naratmalssam.h"
#include "naratmalssamDlg.h"
#include "afxdialogex.h"
#include <psapi.h>


BOOL is_main_window(HWND handle);
HWND FindTopWindow(DWORD pid);


void CnaratmalssamDlg::updateProcessList(void)
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

///////////////////////////////////////////////////////////////////////
//
void CnaratmalssamDlg::OnSelchangeComboprocess()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int sel = m_processList.GetCurSel();

	if (sel < 2)
	{
		if (sel == 1)
		{
			updateProcessList();
			m_processList.SetCurSel(0);
		}
	}
	else
	{
		HWND hwnd = FindTopWindow(m_processList.GetItemData(sel));
		if (hwnd != 0)
		{

		}
		else
		{
			MessageBox(_T("설정할 수 없는 윈도우 입니다."), _T("타겟 윈도우 설정 오류"));
		}
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
BOOL is_main_window(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

///////////////////////////////////////////////////////////////////////
//
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
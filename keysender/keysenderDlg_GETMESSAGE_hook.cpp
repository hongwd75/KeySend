#include "stdafx.h"
#include "keysender.h"
#include "keysenderDlg.h"
#include "afxdialogex.h"


namespace HOOKFUNCS
{
	BOOL  g_hookingNow = false;
	HHOOK g_hHOOK = 0;
	HWND  g_hookTargetHwnd = 0;
	WNDPROC OldProc = 0;

	void startHookTo(HWND hWnd)
	{
		if (hWnd != nullptr && ::IsWindow(hWnd) == true)
		{
			
		}
	}

	LRESULT CALLBACK GetMessageMsgProc(int code, WPARAM wParam, LPARAM lParam)
	{
		MSG *msg = (MSG *)lParam;
		if (msg->message == WM_IME_NOTIFY)
		{
			// TODO : Add to code what you want to act
		}
		// Always call next hook in chain 
		return CallNextHookEx(g_hHOOK, code, wParam, lParam);
	}
}
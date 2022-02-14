#include "stdafx.h"
#include "naratmalssam.h"
#include "naratmalssamDlg.h"
#include "afxdialogex.h"
#include <psapi.h>
#include "mhook-lib/mhook.h"

typedef BOOL (WINAPI* _DrawTextA)(HDC hdc, int x, int y, LPCSTR lpszString, int cbString);
typedef BOOL(WINAPI* _DrawTextW)(HDC hdc, int x, int y, LPCWSTR lpszString, int cbString);


_DrawTextA trueDrawTextA = (_DrawTextA)GetProcAddress(GetModuleHandle(L"gdi32"), "TextOutA");
_DrawTextW trueDrawTextW = (_DrawTextW)GetProcAddress(GetModuleHandle(L"gdi32"), "TextOutW");

//BOOL WINAPI MyDrawTextA(HDC hdc, int x, int y, LPCSTR lpszString, int cbString)
//{
//	//int(WINAPI *pDrawText)(HDC _hdc, int _x, int _y, LPCSTR _lpszString, int _cbString);
//	//pDrawText = (int(WINAPI*)(HDC, int, int, LPCSTR, int)) GetOriginalFunction((ULONG_PTR)MyDrawTextA);
//	TRACE("##TEXTOUTA:%s\n", lpszString);
//
//	return trueDrawTextA(hdc, x, y, lpszString, cbString);
//}
//BOOL WINAPI MyDrawTextW(HDC hdc, int x, int y, LPCWSTR lpszString, int cbString)
//{
//	//int(WINAPI *pDrawText)(HDC _hdc, int _x, int _y, LPCWSTR _lpszString, int _cbString);
//	//pDrawText = (int(WINAPI*)(HDC, int, int, LPCWSTR, int)) GetOriginalFunction((ULONG_PTR)MyDrawTextW);
//	TRACE(_T("##TEXTOUTW:%s\n"), lpszString);
//
//	return trueDrawTextW(hdc, x, y, lpszString, cbString);
//}
BOOL WINAPI MyDrawTextA(HDC hdc, int x, int y, LPCSTR lpszString, int cbString)
{
	//int(WINAPI *pDrawText)(HDC _hdc, int _x, int _y, LPCSTR _lpszString, int _cbString);
	//pDrawText = (int(WINAPI*)(HDC, int, int, LPCSTR, int)) GetOriginalFunction((ULONG_PTR)MyDrawTextA);
	//_TRACE("##TEXTOUTA:%s\n", lpszString);
	char msg[124];
	wsprintfA(msg, "MyDrawTextA");
	return trueDrawTextA(hdc, x, y, msg, strlen(msg));
}
BOOL WINAPI MyDrawTextW(HDC hdc, int x, int y, LPCWSTR lpszString, int cbString)
{
	//int(WINAPI *pDrawText)(HDC _hdc, int _x, int _y, LPCWSTR _lpszString, int _cbString);
	//pDrawText = (int(WINAPI*)(HDC, int, int, LPCWSTR, int)) GetOriginalFunction((ULONG_PTR)MyDrawTextW);
	//TRACE(_T("##TEXTOUTW:%s\n"), lpszString);
	wchar_t msg[124];
	wsprintf(msg, L"MyDrawTextA");
	return trueDrawTextW(hdc, x, y, msg, wcslen(msg));
}

/////////////////////////////////////////////////////////////////////////
//
void CnaratmalssamDlg::setHOOK(DWORD processid)
{
	Mhook_SetHook((PVOID*)&trueDrawTextA, MyDrawTextA);
	Mhook_SetHook((PVOID*)&trueDrawTextW, MyDrawTextW);
}

void CnaratmalssamDlg::unHOOK(DWORD processid)
{
	Mhook_Unhook((PVOID*)&trueDrawTextA);
	Mhook_Unhook((PVOID*)&trueDrawTextW);
}
#include "stdafx.h"
#include "ScreenImage.h"


CScreenImage::CScreenImage()
{
}


CScreenImage::~CScreenImage()
{
}

/****************************************************************************************
Function:   CScreenImage::CaptureRect
Purpose:    captures a screen rectangle
Parameters: rect: screen rectangle to be captured
Return:     non-zero value if successful
****************************************************************************************/
BOOL CScreenImage::CaptureRect(const CRect& rect)
{

	// detach and destroy the old bitmap if any attached
	CImage::Destroy();


	this->Create(rect.Width(), rect.Height(), 32);
	// create a screen and a memory device context
	HDC hDCScreen = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	HDC hDCMem = ::CreateCompatibleDC(hDCScreen);
	// create a compatible bitmap and select it in the memory DC
	HBITMAP hBitmap =
		::CreateCompatibleBitmap(hDCScreen, rect.Width(), rect.Height());
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(hDCMem, hBitmap);

	// bit-blit from screen to memory device context
	// note: CAPTUREBLT flag is required to capture layered windows
	DWORD dwRop = SRCCOPY | CAPTUREBLT;
	BOOL bRet = ::BitBlt(this->GetDC(), 0, 0, rect.Width(), rect.Height(),
		hDCScreen,
		rect.left, rect.top, dwRop);
	// attach bitmap handle to this object
	//Attach(hBitmap);
	this->ReleaseDC();
	// restore the memory DC and perform cleanup
	::SelectObject(hDCMem, hBmpOld);
	::DeleteDC(hDCMem);
	::DeleteDC(hDCScreen);

	return bRet;
}

/****************************************************************************************
Function:   CScreenImage::CaptureScreen
Purpose:    captures the whole screen
Parameters: none
Return:     non-zero value if successful
****************************************************************************************/
BOOL CScreenImage::CaptureScreen()
{
	CRect rect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
	return CaptureRect(rect);
}

/****************************************************************************************
Function:   CScreenImage::CaptureWindow
Purpose:    captures a window
Parameters: hWnd - the window handle
Return:     non-zero value if successful
****************************************************************************************/
BOOL CScreenImage::CaptureWindow(HWND hWnd)
{
	BOOL bRet = FALSE;
	if (::IsWindow(hWnd))
	{
		CRect rect;
		::GetWindowRect(hWnd, rect);
		bRet = CaptureRect(rect);
	}
	return bRet;
}


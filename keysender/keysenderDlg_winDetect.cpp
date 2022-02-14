#include "stdafx.h"
#include "keysender.h"
#include "keysenderDlg.h"


#include <vector>

#define BULLSEYE_CENTER_X_OFFSET		15
#define BULLSEYE_CENTER_Y_OFFSET		18
//////////////////////////////////////////////////////////////////////////////////////
//
BOOL CkeysenderDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	WORD wID = LOWORD(wParam);         // item, control, or accelerator identifier 
	if (wID == IDC_STATIC_ICON_FINDER_TOOL)
	{
		SearchWindow(m_hWnd);
		return true;
	}
	return CDialogEx::OnCommand(wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////////////////
//
void CkeysenderDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (g_bStartSearchWindow)
	{
		DoMouseMove();
		return;
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


//////////////////////////////////////////////////////////////////////////////////////
//
void CkeysenderDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (g_bStartSearchWindow)
	{
		DoMouseUp();
		return;
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}



BOOL CkeysenderDlg::InitialiseResources()
{
	BOOL bRet = FALSE;

	g_hCursorSearchWindow = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_SEARCH_WINDOW));
	if (g_hCursorSearchWindow == NULL)
	{
		bRet = FALSE;
		goto InitialiseResources_0;
	}

	g_hRectanglePen = CreatePen(PS_SOLID, 3, RGB(256, 0, 0));
	if (g_hRectanglePen == NULL)
	{
		bRet = FALSE;
		goto InitialiseResources_0;
	}

	g_hBitmapFinderToolFilled = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_FINDER_FILLED));
	if (g_hBitmapFinderToolFilled == NULL)
	{
		bRet = FALSE;
		goto InitialiseResources_0;
	}

	g_hBitmapFinderToolEmpty = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_FINDER_EMPTY));
	if (g_hBitmapFinderToolEmpty == NULL)
	{
		bRet = FALSE;
		goto InitialiseResources_0;
	}

	// All went well. Return TRUE.
	bRet = TRUE;

InitialiseResources_0:

	return bRet;
}

BOOL CkeysenderDlg::UninitialiseResources()
{
	BOOL bRet = TRUE;

	if (g_hCursorSearchWindow)
	{
		// No need to destroy g_hCursorSearchWindow. It was not created using 
		// CreateCursor().
	}

	if (g_hRectanglePen)
	{
		bRet = DeleteObject(g_hRectanglePen);
		g_hRectanglePen = NULL;
	}

	if (g_hBitmapFinderToolFilled)
	{
		DeleteObject(g_hBitmapFinderToolFilled);
		g_hBitmapFinderToolFilled = NULL;
	}

	if (g_hBitmapFinderToolEmpty)
	{
		DeleteObject(g_hBitmapFinderToolEmpty);
		g_hBitmapFinderToolEmpty = NULL;
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Synopsis :
// 1. This function checks a hwnd to see if it is actually the "Search Window" Dialog's or Main Window's
// own window or one of their children. If so a FALSE will be returned so that these windows will not
// be selected. 
//
// 2. Also, this routine checks to see if the hwnd to be checked is already a currently found window.
// If so, a FALSE will also be returned to avoid repetitions.
BOOL CkeysenderDlg::CheckWindowValidity(HWND hwndToCheck)
{
	HWND hwndTemp = NULL;
	BOOL bRet = TRUE;

	// The window must not be NULL.
	if (hwndToCheck == NULL)
	{
		bRet = FALSE;
		goto CheckWindowValidity_0;
	}

	// It must also be a valid window as far as the OS is concerned.
	if (IsWindow(hwndToCheck) == FALSE)
	{
		bRet = FALSE;
		goto CheckWindowValidity_0;
	}

	// Ensure that the window is not the current one which has already been found.
	if (hwndToCheck == g_hwndFoundWindow)
	{
		bRet = FALSE;
		goto CheckWindowValidity_0;
	}

	// It must also not be the main window itself.
	if (hwndToCheck == m_hWnd)
	{
		bRet = FALSE;
		goto CheckWindowValidity_0;
	}

	// It also must not be one of the dialog box's children...
	hwndTemp = ::GetParent(hwndToCheck);
	if (hwndTemp == m_hWnd)
	{
		bRet = FALSE;
		goto CheckWindowValidity_0;
	}

CheckWindowValidity_0:

	return bRet;
}

long CkeysenderDlg::DoMouseMove(void)
{
	POINT		screenpoint;
	HWND		hwndFoundWindow = NULL;
	long		lRet = 0;

	GetCursorPos(&screenpoint);
	hwndFoundWindow = ::WindowFromPoint(screenpoint);
	if (CheckWindowValidity( hwndFoundWindow))
	{
		DisplayInfoOnFoundWindow(hwndFoundWindow);
		if (g_hwndFoundWindow)
		{
			RefreshWindow(g_hwndFoundWindow);
		}
		g_hwndFoundWindow = hwndFoundWindow;
		m_activeMacro.setTargetWindowWidthHwnd(g_hwndFoundWindow);
		HighlightFoundWindow(g_hwndFoundWindow);
	}
	return lRet;
}


long CkeysenderDlg::DoMouseUp(void)
{
	long lRet = 0;
	if (g_hCursorPrevious)
	{
		SetCursor(g_hCursorPrevious);
	}

	// If there was a found window, refresh it so that its highlighting is erased. 
	if (g_hwndFoundWindow)
	{
		RefreshWindow(g_hwndFoundWindow);
	}

	// Set the bitmap on the Finder Tool icon to be the bitmap with the bullseye bitmap.
	SetFinderToolImage(TRUE);

	// Very important : must release the mouse capture.
	ReleaseCapture();

	// Set the global search window flag to FALSE.
	g_bStartSearchWindow = FALSE;

	return lRet;
}


/////////////////////////////////////////////////////////////////////////////
//
BOOL CkeysenderDlg::SetFinderToolImage(BOOL bSet)
{
	HBITMAP hBmpToSet = NULL;
	BOOL bRet = TRUE;

	if (bSet)
	{
		// Set a FILLED image.
		hBmpToSet = g_hBitmapFinderToolFilled;
	}
	else
	{
		// Set an EMPTY image.
		hBmpToSet = g_hBitmapFinderToolEmpty;
	}

	::SendDlgItemMessage
		(
			(HWND)m_hWnd, // handle of dialog box 
			(int)IDC_STATIC_ICON_FINDER_TOOL, // identifier of control 
			(UINT)STM_SETIMAGE, // message to send 
			(WPARAM)IMAGE_BITMAP, // first message parameter 
			(LPARAM)hBmpToSet // second message parameter 
		);

	return bRet;
}


///////////////////////////////////////////////////////////////////////
BOOL CkeysenderDlg::MoveCursorPositionToBullsEye(HWND hwndDialog)
{
	BOOL bRet = FALSE;
	HWND hwndToolFinder = NULL;
	RECT rect;
	POINT screenpoint;

	// Get the window handle of the Finder Tool static control.
	hwndToolFinder = ::GetDlgItem(hwndDialog, IDC_STATIC_ICON_FINDER_TOOL);

	if (hwndToolFinder)
	{
		// Get the screen coordinates of the static control,
		// add the appropriate pixel offsets to the center of 
		// the bullseye and move the mouse cursor to this exact
		// position.
		::GetWindowRect(hwndToolFinder, &rect);
		screenpoint.x = rect.left + BULLSEYE_CENTER_X_OFFSET;
		screenpoint.y = rect.top + BULLSEYE_CENTER_Y_OFFSET;
		SetCursorPos(screenpoint.x, screenpoint.y);
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////////
//
long CkeysenderDlg::SearchWindow(HWND hwndDialog)
{
	long lRet = 0;

	// Set the global "g_bStartSearchWindow" flag to TRUE.
	g_bStartSearchWindow = TRUE;

	// Display the empty window bitmap image in the Finder Tool static control.
	SetFinderToolImage(FALSE);

	MoveCursorPositionToBullsEye(hwndDialog);

	// Set the screen cursor to the BullsEye cursor.
	if (g_hCursorSearchWindow)
	{
		g_hCursorPrevious = SetCursor(g_hCursorSearchWindow);
	}
	else
	{
		g_hCursorPrevious = NULL;
	}

	// Very important : capture all mouse activities from now onwards and
	// direct all mouse messages to the "Search Window" dialog box procedure.
	::SetCapture(hwndDialog);
	return lRet;
}





long CkeysenderDlg::DisplayInfoOnFoundWindow(HWND hwndFoundWindow)
{
	TCHAR		szClassName[512];
	long		lRet = 0;
	// Get the class name of the found window.
	::GetClassName(hwndFoundWindow, szClassName, sizeof(szClassName) - 1);
	SetDlgItemText(IDC_LOCKWINDOWNAME, szClassName);
	return lRet;
}





long CkeysenderDlg::RefreshWindow(HWND hwndWindowToBeRefreshed)
{
	long lRet = 0;

	::InvalidateRect(hwndWindowToBeRefreshed, NULL, TRUE);
	::UpdateWindow(hwndWindowToBeRefreshed);
	::RedrawWindow(hwndWindowToBeRefreshed, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

	return lRet;
}





// Performs a highlighting of a found window.
// Comments below will demonstrate how this is done.
long CkeysenderDlg::HighlightFoundWindow(HWND hwndFoundWindow)
{
	HDC		hWindowDC = NULL;  // The DC of the found window.
	HGDIOBJ	hPrevPen = NULL;   // Handle of the existing pen in the DC of the found window.
	HGDIOBJ	hPrevBrush = NULL; // Handle of the existing brush in the DC of the found window.
	RECT		rect;              // Rectangle area of the found window.
	long		lRet = 0;

	// Get the screen coordinates of the rectangle of the found window.
	::GetWindowRect(hwndFoundWindow, &rect);

	// Get the window DC of the found window.
	hWindowDC = ::GetWindowDC(hwndFoundWindow);

	if (hWindowDC)
	{
		// Select our created pen into the DC and backup the previous pen.
		hPrevPen = ::SelectObject(hWindowDC, g_hRectanglePen);

		// Select a transparent brush into the DC and backup the previous brush.
		hPrevBrush = ::SelectObject(hWindowDC, GetStockObject(HOLLOW_BRUSH));

		// Draw a rectangle in the DC covering the entire window area of the found window.
		Rectangle(hWindowDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top);

		// Reinsert the previous pen and brush into the found window's DC.
		::SelectObject(hWindowDC, hPrevPen);

		::SelectObject(hWindowDC, hPrevBrush);

		// Finally release the DC.
		::ReleaseDC(hwndFoundWindow, hWindowDC);
	}

	return lRet;
}


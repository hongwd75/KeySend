
// keysenderDlg.h : ��� ����
//

#pragma once
#include "macroObjectManager.h"
#include "afxcmn.h"

#include "castHelperDLG.h"
#include "ScreenImage.h"
#include "afxwin.h"

// CkeysenderDlg ��ȭ ����
class CkeysenderDlg : public CDialogEx
{
// �����Դϴ�.
public:
	static BOOL			g_isFullScrrenMode;
	static HCURSOR		g_hCursorSearchWindow;
	static BOOL         g_styleHelper;
	static CkeysenderDlg *gDlg;

	CkeysenderDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_KEYSENDER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

public:
	void paintListControl(KeyBindSTATE bindKind, DWORD keyvalue, bool on);
	static void updateListControlByObject(KeyBindSTATE bindKind,DWORD keyvalue, bool onActive);
	
public:
	CWinThread *m_pThread;
	CString m_chatLogFile;

// ��ũ�ο�
protected:
	void StartEventCapture();
	void EndEventCapture();
	void updateTargetWidowState(void);
	bool m_HoldCheckBind;

// ������ ����Ʈ��
protected:
	macroObjectManager		 m_MacroList;
	activeMacroObjectManager m_activeMacro;
	CImageList  m_myImageList;
	
protected:
	void UpdateWindowProcessList(void);
	void UpdateBindList(void);
	BOOL InitialiseResources();
	BOOL UninitialiseResources();
	
	long DoMouseMove(void);
	long DoMouseUp(void);
	BOOL CheckWindowValidity(HWND hwndToCheck);
	BOOL SetFinderToolImage( BOOL bSet);
	BOOL MoveCursorPositionToBullsEye(HWND hwndDialog);
	long SearchWindow(HWND hwndDialog);
	long DisplayInfoOnFoundWindow(HWND hwndFoundWindow);
	long RefreshWindow(HWND hwndWindowToBeRefreshed);
	long HighlightFoundWindow(HWND hwndFoundWindow);

	castHelperDLG *m_castDlg;
	BOOL		g_bStartSearchWindow;

	HCURSOR		g_hCursorPrevious;
	HBITMAP		g_hBitmapFinderToolFilled;
	HBITMAP		g_hBitmapFinderToolEmpty;
	HWND		g_hwndFoundWindow;
	HPEN		g_hRectanglePen;


// �����Դϴ�.
protected:
	HICON m_hIcon;
	CScreenImage m_capImage;
	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRawInput(UINT nInputcode, HRAWINPUT hRawInput);
	afx_msg void OnClose();
	afx_msg void OnBnClickedLoadBtn();
	CListCtrl m_bindList;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnItemdblclickListBind(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemchangedListBind(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedEditBindobject();
	afx_msg void OnBnClickedNewBindobject();
	afx_msg void OnBnClickedRemoveBindobject();
	afx_msg void OnBnClickedSaveBindlist();
	afx_msg void OnNMDblclkListBind(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_processList;
	afx_msg void OnSelchangeComboWindowlist();
	afx_msg void OnBnClickedCasthelpbtn();
	afx_msg void OnBnClickedCheckUseFullscreen();
	afx_msg void OnBnClickedChatlogbtn();
};

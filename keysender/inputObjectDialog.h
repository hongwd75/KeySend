#pragma once
#include "afxwin.h"
#include "ScreenImage.h"
#include "macroObjectManager.h"

// inputObjectDialog ��ȭ �����Դϴ�.

class inputObjectDialog : public CDialog
{
	DECLARE_DYNAMIC(inputObjectDialog)

public:
	static inputObjectDialog *inputObjectDLG;
	inputObjectDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~inputObjectDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SELECT_OJECT_DIALOG };

public:
	void onCaptureMousePosition(int x, int y);
	void setDefualtValue(BINDKIND nType, macroObject *value);
	void updateSelectBindKind(void);
protected:
	bool		 m_isNew;
	POINT		 m_mouseMovePosition;
	POINT		 m_mouseMovePosition2;
	int			 m_mouseBtn;
	POINT	     m_imageStaticPoint;		// ȭ�� ��ǥ
	bool		 m_extKey[3];
	bool         m_pushButton;
	DWORD		 m_regikey;
	macroObject  *m_returnValue;
	BINDKIND     m_inputType;
	CRect		 m_paintRect;
	DWORD		 m_delay;

	CScreenImage m_capImage;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_inputKindCombo;
	CComboBox m_keyboardListCombo;
	CComboBox m_mouseBtnCombo;
	afx_msg void OnBnClickedSaveButton();
	virtual BOOL OnInitDialog();
	CStatic m_captureDisplayRect;
	afx_msg void OnBnClickedMousemoveBtn();
	afx_msg void OnPaint();
	afx_msg void OnClose();
};

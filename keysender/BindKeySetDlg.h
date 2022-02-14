#pragma once

#include "macroObjectManager.h"
#include "afxwin.h"
#include "afxcmn.h"

// BindKeySetDlg ��ȭ �����Դϴ�.

class BindKeySetDlg : public CDialog
{
	DECLARE_DYNAMIC(BindKeySetDlg)
public:
	static BindKeySetDlg *instance;
public:
	BindKeySetDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~BindKeySetDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_BINDKEY_DIALOG };
	macroObjectList *m_editTarget;
	bool m_isNew;
protected:
	void updateItemList(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_bindkeylist;
	CListCtrl m_list;
	afx_msg void OnBnClickedAddBtn();
	afx_msg void OnBnClickedEditBtn();
	afx_msg void OnBnClickedRemoveBtn();
	afx_msg void OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSaveBtn();
	afx_msg void OnClose();
};

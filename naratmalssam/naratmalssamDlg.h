
// naratmalssamDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"


// CnaratmalssamDlg ��ȭ ����
class CnaratmalssamDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CnaratmalssamDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_NARATMALSSAM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


protected:
	void updateProcessList(void);
	void setHOOK(DWORD processid);
	void unHOOK(DWORD processid);

protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_processList;
	afx_msg void OnSelchangeComboprocess();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
};



extern bool PrintProcessNameAndID(DWORD processID, std::wstring &out, HANDLE &hProcess);
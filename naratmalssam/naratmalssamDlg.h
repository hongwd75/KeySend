
// naratmalssamDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// CnaratmalssamDlg 대화 상자
class CnaratmalssamDlg : public CDialogEx
{
// 생성입니다.
public:
	CnaratmalssamDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NARATMALSSAM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


protected:
	void updateProcessList(void);
	void setHOOK(DWORD processid);
	void unHOOK(DWORD processid);

protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
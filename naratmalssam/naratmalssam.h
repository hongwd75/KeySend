
// naratmalssam.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.

#include<string>
// CnaratmalssamApp:
// �� Ŭ������ ������ ���ؼ��� naratmalssam.cpp�� �����Ͻʽÿ�.
//

class CnaratmalssamApp : public CWinApp
{
public:
	CnaratmalssamApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CnaratmalssamApp theApp;
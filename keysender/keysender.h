
// keysender.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CkeysenderApp:
// �� Ŭ������ ������ ���ؼ��� keysender.cpp�� �����Ͻʽÿ�.
//

class CkeysenderApp : public CWinApp
{
public:
	CkeysenderApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CkeysenderApp theApp;
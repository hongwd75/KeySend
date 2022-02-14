#pragma once
#include "atlimage.h"

class CBinImage
{
public:
	CBinImage(CImage *convertImage);
	virtual ~CBinImage(void) { clear(); }

public:
	void clear(void);
	bool findTo(CBinImage *background, POINT &pos);	// ã�´�.

protected:
	void make(void);	// 2�� ���·� ������

protected:
	POINT m_rect;
	char *m_memory;
};

class CScreenImage : public CImage
{
public:
	CScreenImage();
	virtual ~CScreenImage();

public:
	BOOL CaptureRect(const CRect& rect);
	BOOL CaptureScreen() throw();
	BOOL CaptureWindow(HWND hWnd);
};


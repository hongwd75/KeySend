#pragma once
#include<string>
#include"mydefines.h"
#include"json.h"



///////////////////////////////////////////////////////////////
//
class macroObject
{
public:
	static HWND targetWindowHandle;

public:
	macroObject();
	macroObject(DWORD delay)
	{
		m_enable = false;
		m_type = BINDKIND::DELAY;
		setDelay(delay);
	}
	virtual ~macroObject();

	BINDKIND getType(void){ return m_type; }
	DWORD    getDelay(void) { return m_delay; }

	virtual std::wstring getValueString(void) { return std::wstring(_T("")); }
	virtual std::wstring getActionString(void) { return std::wstring(_T("대기")); }
	cJSON *getWrite2JSON(void);

public:
	DWORD sendAction(void);
	virtual void onActionEnd(void) { m_active = false; }
	void setDelay(DWORD delay) { m_delay = delay; }

protected:
	void insertDelayTime2JSON(cJSON *to);
	virtual void insertItemInfo2JSON(cJSON *to) { }

	virtual void onAction(void) { }	// 키보드 마우스 행동할 내용을 담는다.

protected:
	bool m_active;
	bool m_enable;
	BINDKIND m_type;
	DWORD    m_delay;
};


///////////////////////////////////////////////////////////////
// 키입력 모두 취소
class macroObjectEnd : public macroObject
{
public:
	macroObjectEnd(DWORD delay)
	{
		m_enable = false;
		setDelay(delay);
		m_type = BINDKIND::CLEAR_ALL_ACTIVE_ACTION;
	}
	virtual ~macroObjectEnd(void)
	{

	}

	void insertItemInfo2JSON(cJSON *to);
	void onAction(void);
};

///////////////////////////////////////////////////////////////
// 키보드
class macroObjectKeyboard : public macroObject
{
public:
	macroObjectKeyboard(DWORD vk, bool alt, bool ctrl, bool shift, DWORD delay)
	{
		m_enable = false;
		m_type = BINDKIND::KEYBOARD; 
		inputList = nullptr;
		set(vk, alt, ctrl, shift);
		setDelay(delay);
	}
	virtual ~macroObjectKeyboard(){ if (inputList != nullptr) delete inputList; }
	virtual std::wstring getValueString(void);
	virtual std::wstring getActionString(void) { return std::wstring(_T("키보드")); }
	void onActionEnd(void);
	void getState(DWORD &vk, bool &alt, bool &ctrl, bool &shift)
	{
		vk = m_VK;
		alt = ControlKeys[0];
		ctrl = ControlKeys[1];
		shift = ControlKeys[2];
	}


protected:
	void set(DWORD vk, bool alt, bool ctrl, bool shift);
	
	void onAction(void);	// 키보드 마우스 행동할 내용을 담는다.
	void insertItemInfo2JSON(cJSON *to);
protected:
	bool ControlKeys[3];
	DWORD m_VK;
	INPUT *inputList;
	int m_size;
	int m_needDelay;
};

///////////////////////////////////////////////////////////////
// 마우스 이동
class macroObjectMouseMove : public macroObject
{
public:
	macroObjectMouseMove(LONG x, LONG y, DWORD delay) 
	{ 
		m_enable = false;
		m_type = BINDKIND::MOSEMOVE; 
		set(x, y); 
		setDelay(delay); 
	}
	virtual ~macroObjectMouseMove(){}
	
	virtual std::wstring getValueString(void);
	virtual std::wstring getActionString(void) { return std::wstring(_T("M.이동")); }
	void getState(LONG &x, LONG &y)
	{
		x = m_postion[0];
		y = m_postion[1];

	}

protected:
	void set(LONG x, LONG y) { m_postion[0] = x, m_postion[1] = y; m_enable = true; }
	void onAction(void);	// 키보드 마우스 행동할 내용을 담는다.
	void insertItemInfo2JSON(cJSON *to);

protected:
	LONG m_postion[2];

};

///////////////////////////////////////////////////////////////
// 마우스 클릭
class macroObjectMouseClick : public macroObject
{
public:
	macroObjectMouseClick(BIND_MOUSEBUTTONKIND click, DWORD delay) 
	{ 
		m_enable = false;
		m_type = BINDKIND::MOUSECLICK; 
		set(click); 
		setDelay(delay); 
	}
	virtual ~macroObjectMouseClick(){}
	virtual std::wstring getValueString(void);
	virtual std::wstring getActionString(void) { return std::wstring(_T("M.클릭")); }
	void getState(int &out)
	{
		//out = (int)m_click[0];
		out = m_clickbtn;
	}
protected:
	void set(BIND_MOUSEBUTTONKIND click);
	void onAction(void);	// 키보드 마우스 행동할 내용을 담는다.
	void insertItemInfo2JSON(cJSON *to);

protected:
	int m_clickbtn;
	int m_click[2]; // 0 = down / 1 = up;
};


///////////////////////////////////////////////////////////////
// 마우스 클릭
class macroObjectMouseDrag : public macroObject
{
public:
	macroObjectMouseDrag(LONG x, LONG y,BIND_MOUSEBUTTONKIND click,bool isPush, DWORD delay)
	{
		m_enable = false;
		m_type = BINDKIND::MOSEDRAG;
		set(x, y, click, isPush);
		setDelay(delay);
	}
	virtual ~macroObjectMouseDrag(){}
	virtual std::wstring getValueString(void);
	virtual std::wstring getActionString(void) { return std::wstring(_T("M.드래그")); }
	void getState(LONG &x, LONG &y,int &button,bool &push)
	{
		x = m_postion[0];
		y = m_postion[1];
		button = (int)m_button;
		push = m_push;
	}

protected:
	void set(LONG x, LONG y, BIND_MOUSEBUTTONKIND click, bool isPush)
	{
		m_postion[0] = x;
		m_postion[1] = y;
		m_button = click;
		m_push = isPush;
		m_enable = true; 
	}
	void onAction(void);	// 마우스 드래그 행동할 내용을 담는다.
	void insertItemInfo2JSON(cJSON *to);

protected:
	LONG m_postion[2];
	BIND_MOUSEBUTTONKIND  m_button;
	bool m_push;

};
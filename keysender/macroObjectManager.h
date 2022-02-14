#pragma once
#include"macroObject.h"
#include<vector>
#include<string>
#include "json.h"




class macroObjectList
{
public:
	static DWORD getStringToVK(KeyBindSTATE kind,const char *msg);

public:
	macroObjectList(TCHAR *name, KeyBindSTATE kind, DWORD value, bool repeat,bool castHelp = false)
	{
		m_repeat = repeat;
		m_title = name;
		m_kind = kind;
		m_BindValue = value;
		m_isCastHelper = castHelp;
	}
	virtual ~macroObjectList()
	{
		for (auto v : m_list)
		{
			delete v;
		}
		m_list.clear();
	}

public:
	static macroObject *makeKeyboard(DWORD vk, bool a, bool c, bool s, DWORD delay)
	{
		macroObjectKeyboard *obj = new macroObjectKeyboard(vk, a, c, s, delay);
		return (macroObject *)obj;
	}

	static macroObject *makeMouseDrag(LONG x, LONG y, BIND_MOUSEBUTTONKIND btn, bool push, DWORD delay)
	{
		auto *obj = new macroObjectMouseDrag(x, y, btn, push,delay);
		return (macroObject *)obj;
	}
	static macroObject *makeMouseMove(LONG x, LONG y, DWORD delay)
	{
		auto *obj = new macroObjectMouseMove(x, y, delay);
		return (macroObject *)obj;
	}
	static macroObject *makeMouseClick(BIND_MOUSEBUTTONKIND btn, DWORD delay)
	{
		auto *obj = new macroObjectMouseClick(btn, delay);
		return (macroObject *)obj;
	}

	static macroObject *makePurge(DWORD delay)
	{
		auto *obj = new macroObjectEnd(delay);
		return (macroObject *)obj;
	}
	static macroObject *makeDelay(DWORD delay)
	{
		auto *obj = new macroObject(delay);
		return (macroObject *)obj;
	}


public:
	DWORD getBindValue(void) { return m_BindValue; }
	KeyBindSTATE getBindState(void) { return m_kind; }
	void changeRepeat(bool rep) { m_repeat = rep; }
	bool isRepeat(void) { return m_repeat; }
	void changeCastHelpOption(bool rep)
	{
		if (rep == true) m_repeat = false;
		m_isCastHelper = rep; 
	}
	bool isCastHelpOption(void) { return m_isCastHelper; }
	
	std::wstring getBindString();
	std::wstring getTitle() { return m_title; }
	void changetTitle(LPCTSTR msg) { m_title = msg; }
	macroObject *get(int index)
	{
		if (index >= m_list.size()) return nullptr;
		return m_list.at(index);
	}
	std::vector<macroObject *> *getList(void) { return &m_list; }
	int getSize(void) { return m_list.size(); }

	void insertMacro(macroObject *obj) { m_list.push_back(obj); }

	bool isSame(KeyBindSTATE kind, DWORD state)
	{
		return  (kind == m_kind  && state == m_BindValue);
	}

	bool isCastHelper(void) { return m_isCastHelper; }
	void updateBindKey(KeyBindSTATE kind, DWORD state)
	{
		m_kind = kind;
		m_BindValue = state;
	}

	void loadByJSON(cJSON *keylist);
	void saveToJSON(cJSON *to);
	void changeObjectFromList(macroObject *oldObject, macroObject *newObject);

protected:
	bool         m_isCastHelper;
	bool		 m_repeat;
	std::wstring m_title;
	KeyBindSTATE m_kind;
	DWORD		 m_BindValue;
	std::vector<macroObject *>m_list;
};


//// 활성화된 메크로 
class activiteMacroObject
{
public:
	activiteMacroObject() { }
	virtual ~activiteMacroObject(){}
	virtual void rest(void) = 0;
	virtual bool isSame(KeyBindSTATE kind, DWORD state) { return false;	}
	virtual bool resetIfSameBindKey(macroObjectList *is){ return false; }
	virtual bool onTick(DWORD tick) { return false; }
	virtual void stopAction(void);
	virtual void startAction(void);
	virtual bool isRepeat(void) = 0;
protected:
	int   m_currentCount;
	DWORD m_nextTick;
};


class activiteMacroObject_helper : public activiteMacroObject
{
public:
	activiteMacroObject_helper(macroObject *target, macroObject *cast)
	{
		mine = macroObjectList::makePurge(50);
		m_list.push_back(mine);
		m_list.push_back(target);
		m_list.push_back(cast);
		rest();
	}
	virtual ~activiteMacroObject_helper(void) {
		m_list.clear();
		delete mine;
	}

	void rest(void)
	{
		m_currentCount = 0;
		m_nextTick = m_list[0]->sendAction() + GetTickCount();
	}

	void stopAction(void)
	{
		if (m_list.size() > m_currentCount)
		{
			m_list[m_currentCount]->onActionEnd();
		}
		activiteMacroObject::stopAction();
	}

	bool onTick(DWORD tick)
	{
		m_list[m_currentCount]->onActionEnd();
		if (m_nextTick <= tick)
		{
			m_currentCount++;
			if (m_list.size() <= m_currentCount)
			{
				return false;
			}
			DWORD sec = m_list[m_currentCount]->sendAction();
			m_nextTick = sec + tick;
		}
		return true;
	}

	bool isRepeat(void)
	{
		return false;
	}

protected:
	macroObject *mine;
	std::vector<macroObject*> m_list;
};

// 매크로용
class activiteMacroObject_macro : public activiteMacroObject
{
public:
	activiteMacroObject_macro(macroObjectList *v)
	{
		m_macro = v;
		rest();
	}
	virtual ~activiteMacroObject_macro(void) {}

public:
	virtual void rest(void)
	{
		m_currentCount = 0;
		m_nextTick = m_macro->get(0)->sendAction() + GetTickCount();
	}

	bool isRepeat(void)
	{
		return m_macro->isRepeat();
	}

	virtual bool isSame(KeyBindSTATE kind, DWORD state)
	{
		return m_macro->isSame(kind, state);
	}
	virtual bool resetIfSameBindKey(macroObjectList *is)
	{
		if (m_macro == is)
		{
			rest();
		}

		return (bool)(m_macro == is);
	}

	void stopAction(void);

	virtual bool onTick(DWORD tick)
	{
		m_macro->get(m_currentCount)->onActionEnd();
		if (m_nextTick <= tick)
		{
			m_currentCount++;
			if (m_macro->getSize() <= m_currentCount)
			{
				if (m_macro->isRepeat() == true)
				{
					m_currentCount = 0;
				} else return false;
			}
			DWORD sec = m_macro->get(m_currentCount)->sendAction();
			m_nextTick = sec + tick;
		}
		return true;
	}

protected:
	macroObjectList *m_macro;
};

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// 메크로 리스트
class macroObjectManager
{
public:
	macroObjectManager();
	virtual ~macroObjectManager();
	static macroObjectManager *makedObjectManager;
	static macroObjectManager *getInstance()
	{
		return macroObjectManager::makedObjectManager;
	}


public:
	void removeAll(void);
	int getCount(void) { return m_list.size(); }
	bool Load(TCHAR *filename);
	bool Save(const TCHAR *filename);
	macroObjectList *checkBindKey(KeyBindSTATE key, DWORD state);
	std::vector<macroObjectList*> *getList(void) { return &m_list; }

public:
	void setTargetWindowClassName(LPSTR classname);	// to json
	CString m_targetWindowClassName;
protected:
	
	std::vector<macroObjectList*> m_list;
};


//////////////////////////////////////////////////////////////
// 실행되고 있는 메크로 리스트
class activeMacroObjectManager
{
public:
	static activeMacroObjectManager *instance;
	static POINT targetWindowPosition;
	static POINT targetWindowBorderSize;
	bool setTargetWindowWidthClassName(LPCTSTR classname);
	void setTargetWindowWidthHwnd(HWND hWnd);

public:
	activeMacroObjectManager(void)
	{
		activeMacroObjectManager::instance = this;
		m_TargetWindowClassName = _T("");
	}
	virtual ~activeMacroObjectManager(void);
	int getCount(void) { return m_list.size(); }
	bool stopBindKey(KeyBindSTATE key, DWORD state);
	void bindMacroObject(macroObjectList *kemo);
	void onTick(DWORD tick);
	void stopAllBindMacro(void);
	void stopRepeatMacro(void);
	void registStopAllBindMacro(void) { m_stopAllMacro = true; }
	void stopAllBindMacroexecptMe(activiteMacroObject *me);
	void pushBackHelp(activiteMacroObject *in)
	{
		m_list.push_back(in);
	}

	bool isActive(macroObjectList *ke);

protected:
	bool m_stopAllMacro;
	CString m_TargetWindowClassName;
	std::vector<activiteMacroObject*> m_list;
};



extern CString ConvertMultibyteToUnicode(char* pMultibyte);
#include "stdafx.h"
#include "macroObject.h"
#include "virtualCode.h"
#include "macroObjectManager.h"
extern CString ConvertMultibyteToUnicode(char* pMultibyte);



HWND macroObject::targetWindowHandle = NULL;

macroObject::macroObject()
{
	m_active = false;
	m_enable = false;
	m_type = BINDKIND::DELAY;
	m_delay = 0;
}


macroObject::~macroObject()
{
}

cJSON *macroObject::getWrite2JSON(void)
{
	cJSON * root = cJSON_CreateObject();
	insertDelayTime2JSON(root);
	this->insertItemInfo2JSON(root);
	return root;
}

void macroObject::insertDelayTime2JSON(cJSON *to)
{
	cJSON_AddNumberToObject(to, "delay", m_delay);
}

DWORD macroObject::sendAction(void)
{
	if (m_enable == true)
	{
		m_active = true;
		this->onAction();
	}
	return m_delay;
}


void macroObjectEnd::insertItemInfo2JSON(cJSON *to)
{
	cJSON_AddStringToObject(to, "purge", 0);
}

void macroObjectEnd::onAction(void)
{
	if (activeMacroObjectManager::instance != nullptr)
	{
		activeMacroObjectManager::instance->registStopAllBindMacro();
	}
}


///////////////////////////////////////////////////////////////
// 마우스 클릭

std::wstring macroObjectMouseClick::getValueString(void)
{
	if (m_click[0] == MOUSEEVENTF_LEFTDOWN)
	{
		return _T("왼쪽 버튼 클릭");
	}
	if (m_click[0] == MOUSEEVENTF_RIGHTDOWN)
	{
		return _T("오른쪽 버튼 클릭");
	}
	if (m_click[0] == MOUSEEVENTF_MIDDLEDOWN)
	{
		return _T("가운데 버튼 클릭");
	}

	return _T("알 수 없는 값이다.");
}
void macroObjectMouseClick::set(BIND_MOUSEBUTTONKIND click)
{
	m_enable = true;
	m_clickbtn = click;
	if (click == BIND_MOUSEBUTTONKIND::BTN_LEFT)
	{
		m_click[0] = MOUSEEVENTF_LEFTDOWN;
		m_click[1] = MOUSEEVENTF_LEFTUP;
	} else
	if (click == BIND_MOUSEBUTTONKIND::BTN_RIGHT)
	{
		m_click[0] = MOUSEEVENTF_RIGHTDOWN;
		m_click[1] = MOUSEEVENTF_RIGHTUP;
	}
	else
	if (click == BIND_MOUSEBUTTONKIND::BTN_MIDDLE)
	{
		m_click[0] = MOUSEEVENTF_MIDDLEDOWN;
		m_click[1] = MOUSEEVENTF_MIDDLEUP;
	}
}

std::wstring macroObjectKeyboard::getValueString(void)
{
	int addcount = 0;
	std::wstring ret = _T("");
	if (m_VK > 0)
	{
		for (auto &vkobject : VIRTULKEY_CHAR2VALUE_list)
		{
			if (vkobject.value == m_VK && vkobject.what == KeyBindSTATE::KEYBOARD_BIND)
			{
				addcount++;
				ret.append(_T("{"));
				ret.append(ConvertMultibyteToUnicode((char*)vkobject.name.c_str()).GetString());
				ret.append(_T("}"));
				break;
			}
		}
	}

	if (ControlKeys[0] == true)
	{
		if (addcount > 0)
		{
			ret.append(_T("+"));
		}
		ret.append(_T("[ALT]"));
		addcount++;
	}
	if (ControlKeys[1] == true)
	{
		if (addcount > 0)
		{
			ret.append(_T("+"));
		}
		ret.append(_T("[CTRL]"));
		addcount++;
	}
	if (ControlKeys[2] == true)
	{
		if (addcount > 0)
		{
			ret.append(_T("+"));
		}
		ret.append(_T("[SHIFT]"));
		addcount++;
	}
	return ret;
}
void macroObjectKeyboard::set(DWORD vk, bool alt, bool ctrl, bool shift)
{
	m_needDelay = 0;
	m_enable = true;
	int current = 0;
	m_size = 0;
	if (alt == true) m_size++;
	if (ctrl == true) m_size++;
	if (shift == true) m_size++;
	if (vk > 0) m_size++;
	 
	ControlKeys[0] = alt;
	ControlKeys[1] = ctrl;
	ControlKeys[2] = shift;
	m_VK = vk;

	m_size *= 2;
	if (inputList != nullptr)
	{
		delete []inputList;
	}
	inputList = new INPUT[m_size];
	ZeroMemory(inputList, sizeof(INPUT)*(m_size));
	

	DWORD flags = KEYEVENTF_SCANCODE;

	if (alt == true)
	{
		inputList[current].type = INPUT_KEYBOARD;
		inputList[current].ki.wVk = VK_MENU;
		inputList[current].ki.dwFlags = KEYEVENTF_SCANCODE;
		inputList[current].ki.wScan =  MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC);
		current++;
	}
	if (ctrl == true)
	{
		inputList[current].type = INPUT_KEYBOARD;
		inputList[current].ki.wVk = VK_CONTROL;
		inputList[current].ki.dwFlags = KEYEVENTF_SCANCODE;
		inputList[current].ki.wScan = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);
		current++; 
	}
	if (shift == true)
	{
		inputList[current].type = INPUT_KEYBOARD;
		inputList[current].ki.wVk = VK_SHIFT;
		inputList[current].ki.dwFlags = KEYEVENTF_SCANCODE;
		inputList[current].ki.wScan = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
		current++;
	}

	if (vk > 0)
	{
		inputList[current].type = INPUT_KEYBOARD;
		inputList[current].ki.wVk = vk;
		inputList[current].ki.dwFlags = KEYEVENTF_SCANCODE;
		inputList[current].ki.wScan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
		current++;
		inputList[current].type = INPUT_KEYBOARD;
		inputList[current].ki.wVk = vk;
		inputList[current].ki.wScan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
		inputList[current].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		current++;
	}

	m_needDelay = current;
	if (alt == true)
	{
		
		inputList[current].type = INPUT_KEYBOARD;
		inputList[current].ki.wVk = VK_MENU;
		inputList[current].ki.wScan =MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC);
		inputList[current].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		current++;
	}
	if (ctrl == true)
	{
		inputList[current].type = INPUT_KEYBOARD;
		inputList[current].ki.wVk = VK_CONTROL;
		inputList[current].ki.wScan =  MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);
		inputList[current].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		current++;
	}
	if (shift == true)
	{
		inputList[current].type = INPUT_KEYBOARD;
		inputList[current].ki.wVk = VK_SHIFT;
		inputList[current].ki.wScan =  MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
		inputList[current].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		current++;
	}

}


//////////////////////////////////////////////////////////////////////
void macroObjectMouseClick::insertItemInfo2JSON(cJSON *to)
{
	char MSG[2];
	MSG[1] = 0;
	switch (m_click[0])
	{
		case MOUSEEVENTF_LEFTDOWN:
		{
			MSG[0] = 'L';
		} break;
		case MOUSEEVENTF_RIGHTDOWN:
		{
			MSG[0] = 'R';
		} break;
		case MOUSEEVENTF_MIDDLEDOWN:
		{
			MSG[0] = 'M';
		} break;
		default:
		{
			MSG[0] = 'L';
		} break;
	}
	cJSON_AddStringToObject(to, "click", MSG);
}

void macroObjectMouseClick::onAction(void)
{
	INPUT    Input = { 0 };
	// left down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = m_click[0];
	::SendInput(1, &Input, sizeof(INPUT));

	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = m_click[1];
	::SendInput(1, &Input, sizeof(INPUT));
}

std::wstring macroObjectMouseMove::getValueString(void)
{
	TCHAR message[512];
	wsprintf(message, _T("마우스좌표 : %d, %d"), m_postion[0], m_postion[1]);

	return std::wstring(message);
}

void macroObjectMouseMove::insertItemInfo2JSON(cJSON *to)
{
	cJSON_AddNumberToObject(to, "movex", (int)m_postion[0]);
	cJSON_AddNumberToObject(to, "movey", (int)m_postion[1]);
}
void macroObjectMouseMove::onAction(void)
{
	INPUT    Input = { 0 };

	double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
	double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;
	double fx = (double)(m_postion[0] + activeMacroObjectManager::targetWindowPosition.x)*(65535.0f / fScreenWidth);
	double fy = (double)(m_postion[1] + activeMacroObjectManager::targetWindowPosition.y)*(65535.0f / fScreenHeight);
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	Input.mi.dx = fx;
	Input.mi.dy = fy;
	::SendInput(1, &Input, sizeof(INPUT));
}


//////////////////////////////////////////////////////////////////////
//
std::wstring macroObjectMouseDrag::getValueString(void)
{
	TCHAR message[512];
	if (m_push == true)
	{
		wsprintf(message, _T("드래그 시작 : %d, %d"), m_postion[0], m_postion[1]);
	}
	else
	{
		wsprintf(message, _T("드래그 끝 : %d, %d"), m_postion[0], m_postion[1]);
	}

	return std::wstring(message);
}
void macroObjectMouseDrag::insertItemInfo2JSON(cJSON *to)
{
	cJSON_AddNumberToObject(to, "movex", (int)m_postion[0]);
	cJSON_AddNumberToObject(to, "movey", (int)m_postion[1]);

	char MSG[2];
	MSG[1] = 0;
	switch (m_button)
	{
		case BTN_LEFT:
		{
			MSG[0] = 'L';
		} break;
		case BTN_RIGHT:
		{
			MSG[0] = 'R';
		} break;
		case BTN_MIDDLE:
		{
			MSG[0] = 'M';
		} break;
		default:
		{
			MSG[0] = 'L';
		} break;
	}

	cJSON_AddStringToObject(to, "click", MSG);
	cJSON_AddNumberToObject(to, "push", (int)m_push);
}
void macroObjectMouseDrag::onAction(void)
{
	INPUT    Input = { 0 };

	double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
	double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;
	double fx = (double)(m_postion[0] + activeMacroObjectManager::targetWindowPosition.x)*(65535.0f / fScreenWidth);
	double fy = (double)(m_postion[1] + activeMacroObjectManager::targetWindowPosition.y)*(65535.0f / fScreenHeight);
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

	switch (m_button)
	{
		case BTN_LEFT:
		{
			if (m_push == true) Input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
			else Input.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
		} break;
		case BTN_RIGHT:
		{
			if (m_push == true) Input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
			else Input.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
		} break;
		case BTN_MIDDLE:
		{
			if (m_push == true) Input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
			else Input.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
		} break;
	}

	Input.mi.dx = fx;
	Input.mi.dy = fy;
	::SendInput(1, &Input, sizeof(INPUT));
}

//macroObjectMouseDrag
/////////////////////////////////////////////////////////////////////////////////////
// keyboard
void macroObjectKeyboard::insertItemInfo2JSON(cJSON *to)
{
	if (m_VK == 0 && ControlKeys[0] == false && ControlKeys[1] == false && ControlKeys[2] == false)
	{
		return;
	}

	char keyname[256];

	if (m_VK != 0)
	{
		for (auto &vkobject : VIRTULKEY_CHAR2VALUE_list)
		{
			if (vkobject.value == m_VK && vkobject.what == KeyBindSTATE::KEYBOARD_BIND)
			{
				strcpy(keyname, vkobject.name.c_str());
				break;
			}
		}
	}
	else
	{
		sprintf(keyname, "none");
	}

	cJSON_AddStringToObject(to, "key", keyname);
	cJSON_AddNumberToObject(to, "alt", (int)ControlKeys[0]);
	cJSON_AddNumberToObject(to, "ctrl", (int)ControlKeys[1]);
	cJSON_AddNumberToObject(to, "shift", (int)ControlKeys[2]);
}

void macroObjectKeyboard::onAction(void)
{
	SendInput(m_needDelay, inputList, sizeof(INPUT));
}

void macroObjectKeyboard::onActionEnd(void)
{
	if (m_active == true)
	{
		if (m_size > m_needDelay)
		{
			SendInput(m_size - m_needDelay, inputList + m_needDelay, sizeof(INPUT));
		}
		m_active = false;
	} 
}
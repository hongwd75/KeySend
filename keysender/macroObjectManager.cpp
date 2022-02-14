#include "stdafx.h"
#include "macroObjectManager.h"
#include "virtualCode.h"
#include <fstream>
#include <string>
#include <iostream>
#include "resource.h"
#include "castHelperDLG.h"
#include "keysenderDlg.h"
/////////////////////////////////////////////////////////////
//
CString ConvertMultibyteToUnicode(char* pMultibyte)
{
	int nLen = strlen(pMultibyte);

	WCHAR pWideChar[512] = { 0, };

	int size = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pMultibyte, -1, pWideChar, nLen);
	//pWideChar[nLen] = 0;

	return CString(pWideChar);
}


/////////////////////////////////////////////////////////////
//
void activiteMacroObject::stopAction(void)
{
}

void activiteMacroObject::startAction(void)
{
}

void activiteMacroObject_macro::stopAction(void)
{
	if (m_currentCount < m_macro->getSize())
	{
		m_macro->get(m_currentCount)->onActionEnd();
	}
	activiteMacroObject::stopAction();

	CkeysenderDlg::updateListControlByObject(m_macro->getBindState(), m_macro->getBindValue(), false);
}


/////////////////////////////////////////////////////////////
//
void macroObjectList::changeObjectFromList(macroObject *oldObject, macroObject *newObject)
{
	auto itr = m_list.begin();

	while (itr != m_list.end())
	{
		if ((*itr) == oldObject)
		{
			(*itr) = newObject;
			delete oldObject;
			return;
		}
		itr++;
	}

	m_list.push_back(newObject);
}


/////////////////////////////////////////////////////////////
//
std::wstring macroObjectList::getBindString()
{
	std::wstring ret = _T("");

	if (m_BindValue == 0)
	{
		ret = ConvertMultibyteToUnicode((char*)VIRTULKEY_CHAR2VALUE_list[0].name.c_str()).GetString();
	} else 
	if (m_kind == KeyBindSTATE::KEYBOARD_BIND || m_kind == KeyBindSTATE::MOUSE_BIND)
	{
		for (auto &vkobject : VIRTULKEY_CHAR2VALUE_list)
		{
			if (vkobject.value == m_BindValue && vkobject.what == m_kind)
			{
				ret = ConvertMultibyteToUnicode((char*)vkobject.name.c_str()).GetString();
				break;
			}
		}
	}
	//else
	//if (m_kind == KeyBindSTATE::MOUSE_BIND)
	//{
	//	if (m_BindValue == BIND_MOUSEBUTTONKIND::BTN_LEFT)
	//	{
	//		ret = _T("L 버튼");
	//	} else 
	//	if (m_BindValue == BIND_MOUSEBUTTONKIND::BTN_MIDDLE)
	//	{
	//		ret = _T("M 버튼");
	//	}
	//	else
	//	if (m_BindValue == BIND_MOUSEBUTTONKIND::BTN_RIGHT)
	//	{
	//		ret = _T("R 버튼");
	//	}
	//}
	return ret;
}


DWORD macroObjectList::getStringToVK(KeyBindSTATE kind, const char *msg)
{
	DWORD ret = 0;
	if (kind == KeyBindSTATE::KEYBOARD_BIND)
	{
		if (strlen(msg) == 1)
		{
			auto vk = VkKeyScanA(msg[0]);
			if (vk != -1)
			{
				ret = vk & 0xFF;
			}
		} else 
		if (strlen(msg) > 1)
		{
			for (auto &value : VIRTULKEY_CHAR2VALUE_list)
			{
				if (_stricmp(value.name.c_str(), msg) == 0 && value.what == kind)
				{
					ret = value.value & 0xFF;
					break;
				}
			}
		}
	} else 
	if (kind == KeyBindSTATE::MOUSE_BIND)
	{
		if (_stricmp(msg, "M") == 0)
		{
			ret = (DWORD)BIND_MOUSEBUTTONKIND::BTN_MIDDLE;
		} else
		if (_stricmp(msg, "L") == 0)
		{
			ret = (DWORD)BIND_MOUSEBUTTONKIND::BTN_LEFT;
		}
		else
		if (_stricmp(msg, "R") == 0)
		{
			ret = (DWORD)BIND_MOUSEBUTTONKIND::BTN_RIGHT;
		}
		else
		if (_stricmp(msg, "B4") == 0)
		{
			ret = (DWORD)BIND_MOUSEBUTTONKIND::BTN_4;
		}
		else
		if (_stricmp(msg, "B5") == 0)
		{
			ret = (DWORD)BIND_MOUSEBUTTONKIND::BTN_5;
		}
	}

	return ret;
}


macroObjectManager *macroObjectManager::makedObjectManager = nullptr;
macroObjectManager::macroObjectManager()
{
	m_targetWindowClassName = _T("");
	macroObjectManager::makedObjectManager = this;
}


macroObjectManager::~macroObjectManager()
{
	removeAll();
}


void macroObjectManager::removeAll(void)
{
	for (auto &value : m_list)
	{
		delete value;
	}

	m_list.clear();
}


void macroObjectManager::setTargetWindowClassName(LPSTR classname)
{
	if (strlen(classname) <= 0)
	{
		m_targetWindowClassName = _T("");
	}
	else
	{
		m_targetWindowClassName = ConvertMultibyteToUnicode(classname);
	}
}


///////////////////////////////////////////////////////////////////////////////////////
//
bool macroObjectManager::Save(const TCHAR *filename)
{
	char mFilename[512];
	char *jsonFileText;
	int len = WideCharToMultiByte(CP_ACP, 0, filename, -1, mFilename, 512, NULL, NULL);

	cJSON *root = cJSON_CreateObject();
	cJSON *rootarray = cJSON_CreateArray();

	cJSON_AddItemToObject(root, "list", rootarray);
	for (auto item : m_list)
	{
		item->saveToJSON(rootarray);
	}
	char* text = cJSON_Print(root);
	std::ofstream out(mFilename);
	out << text;
	out.clear();
	free(text);
	cJSON_Delete(root);
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////
//
bool macroObjectManager::Load(TCHAR *filename)
{
	char mFilename[512];
	char *jsonFileText;
	int len = WideCharToMultiByte(CP_ACP, 0, filename, -1, mFilename, 512, NULL, NULL);
	mFilename[len] = 0;
	FILE *fp = fopen(mFilename, "rt");
	if (fp != nullptr)
	{
		unsigned long size;
		fseek(fp, 0, SEEK_END);
		long lSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		jsonFileText = new char[lSize + 4];
		lSize = fread(jsonFileText, sizeof(unsigned char), lSize, fp);
		jsonFileText[lSize] = 0;
		fclose(fp);

		cJSON *json = cJSON_Parse(jsonFileText);

		if (json == nullptr)
		{
			delete jsonFileText;
			return false;
		}

		removeAll();

		auto targetWindowJson = cJSON_GetObjectItem(json, "targetwindow");
		if (targetWindowJson != nullptr)
		{
			setTargetWindowClassName(targetWindowJson->valuestring);
		}
		else
		{
			setTargetWindowClassName("");
		}
		
		auto listJson = cJSON_GetObjectItem(json, "list");
		if (listJson != nullptr)
		{
			int arraySize = cJSON_GetArraySize(listJson);
			if (arraySize > 0)
			{
				for (int xi = 0; xi < arraySize; xi++)
				{
					cJSON *arrayObj = cJSON_GetArrayItem(listJson, xi);
					if (arrayObj != nullptr)
					{
						auto arryTitle = cJSON_GetObjectItem(arrayObj, "title");
						if (arryTitle != nullptr)
						{
							bool casthelp = false;
							bool repeat = false;
							DWORD bindVK = 0;
							KeyBindSTATE bindType = KeyBindSTATE::none;
							CString szTitle = ConvertMultibyteToUnicode(arryTitle->valuestring);

							auto arryType = cJSON_GetObjectItem(arrayObj, "type");
							auto arryRepeat = cJSON_GetObjectItem(arrayObj, "repeat");
							auto arryBind = cJSON_GetObjectItem(arrayObj, "bind");
							auto isCastHelper = cJSON_GetObjectItem(arrayObj, "casthelpkey");
							if (isCastHelper != nullptr)
							{
								casthelp = (bool)(isCastHelper->valueint != 0);
							}
							if (arryType != nullptr)
							{
								if (_stricmp(arryType->valuestring, "keyboard") == 0)
								{
									bindType = KeyBindSTATE::KEYBOARD_BIND;
								} else 
								if (_stricmp(arryType->valuestring, "mouse") == 0)
								{
									bindType = KeyBindSTATE::MOUSE_BIND;
								}
							}

							if (bindType != KeyBindSTATE::none && arryBind != nullptr)
							{
								bindVK = macroObjectList::getStringToVK(bindType, arryBind->valuestring);
							}

							if (arryRepeat != nullptr && arryRepeat->valueint != 0)
							{
								repeat = true;
							}

							
							if (bindVK > 0 && szTitle.GetLength() > 0)
							{
								macroObjectList *pl = new macroObjectList(szTitle.GetBuffer(), bindType, bindVK, repeat, casthelp);
								if (pl != nullptr)
								{
									m_list.push_back(pl);
									auto pushkeylist = cJSON_GetObjectItem(arrayObj, "keys");
									if (pushkeylist != nullptr)
									{
										pl->loadByJSON(pushkeylist);
									}
								}
							}
						}
					}
				}
			}
		}
		cJSON_Delete(json);
		delete jsonFileText;
	}
	else return false;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
//
macroObjectList *macroObjectManager::checkBindKey(KeyBindSTATE key, DWORD state)
{
	for (auto &value : m_list)
	{
		if (value->isSame(key, state) == true)
		{
			return value;
		}
	}
	return nullptr;
}


///////////////////////////////////////////////////////////////////////////////////////
//
void macroObjectList::saveToJSON(cJSON *to_root)
{
	char title[512];
	char bindkey[256];
	sprintf(bindkey, "none");

	cJSON * to = cJSON_CreateObject();
	int len = WideCharToMultiByte(CP_ACP, 0, this->m_title.c_str(), -1, title, 512, NULL, NULL);
	cJSON_AddStringToObject(to, "title", title);

	if (m_BindValue <= 0)
	{
		sprintf(title, "none");
	}
	else
	{
		if (KeyBindSTATE::KEYBOARD_BIND == this->m_kind)
		{
			sprintf(title, "keyboard");
			for (auto &vkobject : VIRTULKEY_CHAR2VALUE_list)
			{
				if (vkobject.value == m_BindValue && vkobject.what == KeyBindSTATE::KEYBOARD_BIND)
				{
					strcpy(bindkey, vkobject.name.c_str());
					break;
				}
			}
		}
		else
		{
			sprintf(title, "mouse");

			if (m_BindValue == (DWORD)BIND_MOUSEBUTTONKIND::BTN_MIDDLE)
			{
				sprintf(bindkey, "M");
			} else 
			if (m_BindValue == (DWORD)BIND_MOUSEBUTTONKIND::BTN_LEFT)
			{
				sprintf(bindkey, "L");
			}
			else
			if (m_BindValue == (DWORD)BIND_MOUSEBUTTONKIND::BTN_RIGHT)
			{
				sprintf(bindkey, "R");
			}
			else
				if (m_BindValue == (DWORD)BIND_MOUSEBUTTONKIND::BTN_4)
				{
					sprintf(bindkey, "B4");
				}
				else
					if (m_BindValue == (DWORD)BIND_MOUSEBUTTONKIND::BTN_5)
					{
						sprintf(bindkey, "B5");
					}
		}
	}

	cJSON_AddStringToObject(to, "type", title);
	cJSON_AddStringToObject(to, "bind", bindkey);
	cJSON_AddNumberToObject(to, "repeat", (int)(this->m_repeat == true && m_isCastHelper == false));
	cJSON_AddNumberToObject(to, "casthelpkey", (int)this->m_isCastHelper);

	if (m_list.size() > 0)
	{
		cJSON *root = cJSON_CreateArray();
		cJSON_AddItemToObject(to, "keys", root);
		for (auto item : m_list)
		{
			auto jsonitem = item->getWrite2JSON();
			if (jsonitem != nullptr)
			{
				cJSON_AddItemToArray(root, jsonitem);
			}
		}
	}
	cJSON_AddItemToArray(to_root, to);
}
void macroObjectList::loadByJSON(cJSON *listJson)
{
	int arraySize = cJSON_GetArraySize(listJson);
	if (arraySize > 0)
	{
		for (int xi = 0; xi < arraySize; xi++)
		{
			cJSON *arrayObj = cJSON_GetArrayItem(listJson, xi);
			if (arrayObj != nullptr)
			{
				DWORD mDelay = 0;
				BINDKIND bindKind = BINDKIND::DELAY;

				auto arryPurge = cJSON_GetObjectItem(arrayObj, "purge");
				if (arryPurge == nullptr)
				{
					auto arryDelay = cJSON_GetObjectItem(arrayObj, "delay");
					auto arryKey = cJSON_GetObjectItem(arrayObj, "key");
					auto arryClick = cJSON_GetObjectItem(arrayObj, "click");
					auto arryMove = cJSON_GetObjectItem(arrayObj, "movex");

					if (arryDelay != nullptr) mDelay = arryDelay->valueint;
					if (arryKey != nullptr)
					{
						bindKind = BINDKIND::KEYBOARD;
						bool EXKEY[3] = { false, false, false };
						auto arryALT = cJSON_GetObjectItem(arrayObj, "alt");
						auto arryCTRL = cJSON_GetObjectItem(arrayObj, "ctrl");
						auto arrySHIFT = cJSON_GetObjectItem(arrayObj, "shift");

						EXKEY[0] = (arryALT != nullptr && arryALT->valueint != 0);
						EXKEY[1] = (arryCTRL != nullptr && arryCTRL->valueint != 0);
						EXKEY[2] = (arrySHIFT != nullptr && arrySHIFT->valueint != 0);

						DWORD vk = 0;

						if (strcmp(arryKey->valuestring, "none") != 0)
						{
							vk = macroObjectList::getStringToVK(KeyBindSTATE::KEYBOARD_BIND, arryKey->valuestring);
						}
						insertMacro(macroObjectList::makeKeyboard(vk, EXKEY[0], EXKEY[1], EXKEY[2], mDelay));
					}
					else
						if (arryClick != nullptr && arryMove != nullptr)
						{
							bindKind = BINDKIND::MOSEDRAG;
							auto arryPush = cJSON_GetObjectItem(arrayObj, "push");
							DWORD vk = macroObjectList::getStringToVK(KeyBindSTATE::MOUSE_BIND, arryClick->valuestring);
							auto arryMoveY = cJSON_GetObjectItem(arrayObj, "movey");
							if (vk != 0 && arryMoveY != nullptr && arryPush != nullptr)
							{
								insertMacro(macroObjectList::makeMouseDrag(arryMove->valueint, arryMoveY->valueint, (BIND_MOUSEBUTTONKIND)vk, (bool)(arryPush->valueint != 0), mDelay));
							}

						} else 
						if (arryClick != nullptr)
						{
							bindKind = BINDKIND::MOUSECLICK;
							DWORD vk = macroObjectList::getStringToVK(KeyBindSTATE::MOUSE_BIND, arryClick->valuestring);
							if (vk != 0)
							{
								insertMacro(macroObjectList::makeMouseClick((BIND_MOUSEBUTTONKIND)vk, mDelay));
							}
						}
						else
							if (arryMove != nullptr)
							{
								bindKind = BINDKIND::MOSEMOVE;
								auto arryMoveY = cJSON_GetObjectItem(arrayObj, "movey");
								if (arryMoveY != nullptr)
								{
									insertMacro(macroObjectList::makeMouseMove(arryMove->valueint, arryMoveY->valueint, mDelay));
								}
							}

					if (bindKind == BINDKIND::DELAY)
					{
						insertMacro(macroObjectList::makeDelay(mDelay));
					}
				}
				else
				{
					insertMacro(macroObjectList::makePurge(100));
				}
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////

activeMacroObjectManager::~activeMacroObjectManager(void)
{
	stopAllBindMacro();
}

activeMacroObjectManager *activeMacroObjectManager::instance = nullptr;
POINT activeMacroObjectManager::targetWindowPosition = { 0, 0 };
POINT activeMacroObjectManager::targetWindowBorderSize = { 0, 0 };

bool activeMacroObjectManager::setTargetWindowWidthClassName(LPCTSTR classname)
{
	HWND fn = ::FindWindow((LPCTSTR)classname, NULL);
	if (fn != 0)
	{
		macroObject::targetWindowHandle = fn;
		m_TargetWindowClassName = classname;
		return true;
	}
	return false;
}

void activeMacroObjectManager::setTargetWindowWidthHwnd(HWND hWnd)
{
	activeMacroObjectManager::targetWindowBorderSize.x = 0;
	activeMacroObjectManager::targetWindowBorderSize.y = 0;

	if (hWnd != 0)
	{
		TCHAR		szClassName[512];
		long		lRet = 0;

		// Get the class name of the found window.
		macroObject::targetWindowHandle = hWnd;
		::GetClassName(hWnd, szClassName, sizeof(szClassName) - 1);
		m_TargetWindowClassName = szClassName;

		DWORD dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
		DWORD dwExStyle = (DWORD)GetWindowLong(hWnd, GWL_EXSTYLE);

		if ((WS_BORDER & dwStyle) > 0)
		{
			activeMacroObjectManager::targetWindowBorderSize.x = GetSystemMetrics(SM_CXSIZEFRAME);
			activeMacroObjectManager::targetWindowBorderSize.y = GetSystemMetrics(SM_CYSIZEFRAME);
		}

		if ((WS_CAPTION & dwStyle) > 0)
		{
			
			if ((WS_EX_TOOLWINDOW & dwExStyle) > 0)
			{
				activeMacroObjectManager::targetWindowBorderSize.y += GetSystemMetrics(SM_CYCAPTION);
			}
			else activeMacroObjectManager::targetWindowBorderSize.y += GetSystemMetrics(SM_CYCAPTION);
		}
	}
	else macroObject::targetWindowHandle = nullptr;
}

void activeMacroObjectManager::bindMacroObject(macroObjectList *kemo)
{
	for (auto vv : m_list)
	{
		if (vv->resetIfSameBindKey(kemo) == true)
		{
			return;
		}
	}

	if (kemo->isCastHelper() == true)
	{
		if (castHelperDLG::instance != nullptr)
		{
			activiteMacroObject_helper *helper = castHelperDLG::instance->getFirstAction();
			if (helper != nullptr)
			{
				m_list.push_back(helper);
			}
		}
	}
	else
	{
		activiteMacroObject_macro *newObject = new activiteMacroObject_macro(kemo);
		m_list.push_back(newObject);
	}

}



bool activeMacroObjectManager::isActive(macroObjectList *ke)
{
	for (auto &v : m_list)
	{
		if (v->isSame(ke->getBindState(),ke->getBindValue()) == true)
		{
			return true;
		}
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////
//
void activeMacroObjectManager::onTick(DWORD tick)
{
	auto itr = m_list.begin();

	while (itr != m_list.end())
	{
		m_stopAllMacro = false;
		if ((*itr)->onTick(tick) == false)
		{
			if (m_stopAllMacro == true)
			{
				m_stopAllMacro = false;
				stopAllBindMacroexecptMe((*itr));
				return;
			}
			(*itr)->stopAction();
			delete (*itr);
			itr = m_list.erase(itr);
			continue;
		}
		itr++;
	}
}

bool activeMacroObjectManager::stopBindKey(KeyBindSTATE key, DWORD state)
{
	auto itr = m_list.begin();

	while (itr != m_list.end())
	{
		if ((*itr)->isSame(key,state) == true)
		{
			(*itr)->stopAction();
			delete (*itr);
			itr = m_list.erase(itr);
			return true;
		}
		itr++;
	}

	return false;
}


void activeMacroObjectManager::stopAllBindMacroexecptMe(activiteMacroObject *me)
{
	auto itr = m_list.begin();

	while (itr != m_list.end())
	{
		if ((*itr) != me)
		{
			(*itr)->stopAction();
			delete (*itr);
			itr = m_list.erase(itr);
			continue;
		}
		itr++;
	}
}
void activeMacroObjectManager::stopAllBindMacro(void)
{
	for (auto &value : m_list)
	{
		value->stopAction();
		delete value;
	}

	m_list.clear();

}

void activeMacroObjectManager::stopRepeatMacro(void)
{
	auto itr = m_list.begin();

	while (itr != m_list.end())
	{
		itr++;
	}
}

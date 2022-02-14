#include "stdafx.h"
#include "keysender.h"
#include "keysenderDlg.h"
#include "afxdialogex.h"
#include "BindKeySetDlg.h"
#include "inputObjectDialog.h"


DWORD convertSameVKKey(DWORD vk)
{
	DWORD ret = vk;
	switch (vk)
	{
		case VK_LEFT:{	ret = 0x64;	} break;
		case VK_UP:	{ret = 0x68; } break;
		case VK_RIGHT: {ret = 0x66;	} break;
		case VK_DOWN: {	ret = 0x62;	} break;
		case VK_PRIOR: {ret = 0x69;	} break;
		case VK_NEXT: {	ret = 0x63;	}break;
		case VK_END: { ret = 0x61;	} break;
		case VK_HOME: {	ret = 0x67; } break;
		case VK_INSERT: { ret = 0x60;} break;
		case VK_DELETE: { ret = 0x6e;} break;
		case 0x0c: {return 0x65; } break;
	}

	return ret;
}
void CkeysenderDlg::StartEventCapture()
{
	RAWINPUTDEVICE rawInputDev[2];
	ZeroMemory(rawInputDev, sizeof(RAWINPUTDEVICE) * 2);

	// 키보드 RAWINPUTDEVICE 구조체 설정	
	rawInputDev[0].usUsagePage = 0x01;
	rawInputDev[0].usUsage = 0x06;
	rawInputDev[0].dwFlags = RIDEV_INPUTSINK;
	rawInputDev[0].hwndTarget = m_hWnd;

	//마우스 RAWINPUTDEVICE 구조체 설정
	rawInputDev[1].usUsagePage = 0x01;
	rawInputDev[1].usUsage = 0x02;
	rawInputDev[1].dwFlags = RIDEV_INPUTSINK;
	rawInputDev[1].hwndTarget = m_hWnd;

	if (FALSE == RegisterRawInputDevices(rawInputDev, 2, sizeof(RAWINPUTDEVICE)))
	{
		CString str;
		str.Format(_T("키보드 입력 검색 기능 활성화 실패! 오류코드: %d"), GetLastError());
		MessageBox(str);
	}
}

void CkeysenderDlg::EndEventCapture()
{
	RAWINPUTDEVICE rawInputDev[2];
	ZeroMemory(rawInputDev, sizeof(RAWINPUTDEVICE) * 2);

	// 키보드 RAWINPUTDEVICE 구조체 설정	
	rawInputDev[0].usUsagePage = 0x01;
	rawInputDev[0].usUsage = 0x06;
	rawInputDev[0].dwFlags = RIDEV_REMOVE;
	rawInputDev[0].hwndTarget = m_hWnd;

	//마우스 RAWINPUTDEVICE 구조체 설정
	rawInputDev[1].usUsagePage = 0x01;
	rawInputDev[1].usUsage = 0x02;
	rawInputDev[1].dwFlags = RIDEV_REMOVE;
	rawInputDev[1].hwndTarget = m_hWnd;

	RegisterRawInputDevices(rawInputDev, 2, sizeof(RAWINPUTDEVICE));
}


void CkeysenderDlg::updateListControlByObject(KeyBindSTATE bindKind, DWORD keyvalue,bool onActive)
{
	CkeysenderDlg::gDlg->paintListControl(bindKind, keyvalue, onActive);
}


//////////////////////////////////////////////////////////////////////////////////
//
void CkeysenderDlg::OnRawInput(UINT nInputcode, HRAWINPUT hRawInput)
{
	TRACE1("CkeysenderDlg::OnRawInput : %d\n", nInputcode);
	if (inputObjectDialog::inputObjectDLG != nullptr)
	{
		RAWINPUT Input;
		memset(&Input, 0, sizeof(RAWINPUT));
		UINT nSize = sizeof(RAWINPUT);
		GetRawInputData(hRawInput, RID_INPUT, &Input, &nSize, sizeof(RAWINPUTHEADER));

		if (Input.header.dwType == RIM_TYPEMOUSE)
		{
			bool MMButtonDown = (bool)(Input.data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN);
			if (MMButtonDown == true)
			{
				POINT pos;
				GetCursorPos(&pos);
				inputObjectDialog::inputObjectDLG->onCaptureMousePosition(pos.x,pos.y);
			}
		}
	} else 
	if (m_HoldCheckBind == false && nInputcode == RIM_INPUTSINK)
	{
		RAWINPUT Input;
		memset(&Input, 0, sizeof(RAWINPUT));
		UINT nSize = sizeof(RAWINPUT);
		GetRawInputData(hRawInput, RID_INPUT, &Input, &nSize, sizeof(RAWINPUTHEADER));

		DWORD motionValue = 0;
		KeyBindSTATE moation = KeyBindSTATE::none;

		if (Input.header.dwType == RIM_TYPEKEYBOARD && Input.data.keyboard.Flags == 0)
		{
			moation = KeyBindSTATE::KEYBOARD_BIND;
			if (Input.data.keyboard.VKey > 1)
			{
				TRACE1("CkeysenderDlg::OnRawInput : %d\n", Input.data.keyboard.VKey);
			}
			motionValue = convertSameVKKey(Input.data.keyboard.VKey);
		}
		else
		if (Input.header.dwType == RIM_TYPEMOUSE)
		{
			bool LMButtonDown = (bool)(Input.data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN);
			bool MMButtonDown = (bool)(Input.data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN);
			bool RMButtonDown = (bool)(Input.data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN);
			bool RMButtonDown4 = (bool)(Input.data.mouse.ulButtons & RI_MOUSE_BUTTON_4_DOWN);
			bool RMButtonDown5 = (bool)(Input.data.mouse.ulButtons & RI_MOUSE_BUTTON_5_DOWN);



			//Input.data.mouse.lLastX
			if ((LMButtonDown | MMButtonDown | RMButtonDown | RMButtonDown4 | RMButtonDown5) == true)
			{
				moation = KeyBindSTATE::MOUSE_BIND;
				if (LMButtonDown == true) motionValue = (DWORD)BIND_MOUSEBUTTONKIND::BTN_LEFT;
				if (MMButtonDown == true) motionValue = (DWORD)BIND_MOUSEBUTTONKIND::BTN_MIDDLE;
				if (RMButtonDown == true) motionValue = (DWORD)BIND_MOUSEBUTTONKIND::BTN_RIGHT;
				if (RMButtonDown4 == true) motionValue = (DWORD)BIND_MOUSEBUTTONKIND::BTN_4;
				if (RMButtonDown5 == true) motionValue = (DWORD)BIND_MOUSEBUTTONKIND::BTN_5;
			}
		}


		if (moation != KeyBindSTATE::none)
		{
			bool stopActiveMacro = true;
			auto retValue = m_MacroList.checkBindKey(moation, motionValue);

			if (m_activeMacro.isActive(retValue) == true)
			{
				if (retValue->isRepeat() == false)
				{
					stopActiveMacro = false;
				}
			}
			if (stopActiveMacro == true)
			{
				bool actmacro = m_activeMacro.stopBindKey(moation, motionValue);
				if (retValue != nullptr)
				{
					if (actmacro == false)
					{
						m_activeMacro.bindMacroObject(retValue);
						CkeysenderDlg::updateListControlByObject(moation, motionValue, !actmacro);
					}
				}
			}
		}
	}
	CDialogEx::OnRawInput(nInputcode, hRawInput);
}


//////////////////////////////////////////////////////////////////////////
//
void CkeysenderDlg::OnBnClickedEditBindobject()
{
	int selitem = m_bindList.GetSelectionMark();
	if (selitem != -1)
	{
		auto itemlist = m_MacroList.getList();
		int at = (int)m_bindList.GetItemData(selitem);
		if (itemlist->size() > at)
		{
			auto value = itemlist->at(at);
			BindKeySetDlg dlg;
			dlg.m_editTarget = value;
			if (dlg.DoModal() == IDOK)
			{
				UpdateBindList();
			}
		}
	}
}

void CkeysenderDlg::OnBnClickedNewBindobject()
{
	macroObjectList *newObject = new macroObjectList(_T("새로운 바인드"), KeyBindSTATE::none, 0, false);

	BindKeySetDlg dlg;
	dlg.m_editTarget = newObject;
	if(dlg.DoModal() ==IDOK)
	{
		m_MacroList.getList()->push_back(newObject);
		UpdateBindList();
	}
	else
	{
		delete newObject;
	}


}


/////////////////////////////////////////////////////////////////////////
//
void CkeysenderDlg::OnBnClickedRemoveBindobject()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

/////////////////////////////////////////////////////////////////////////
//
void CkeysenderDlg::OnBnClickedSaveBindlist()
{
	TCHAR szFilter[] = _T("Text (*.text, *.json, *.txt) | *.text;*.txt;*.json | All Files(*.*)|*.*||");

	m_HoldCheckBind = true;
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (dlg.DoModal() == IDOK)
	{
		CString savefilename = dlg.GetPathName();
		this->m_MacroList.Save(savefilename.GetString());
	}
	m_HoldCheckBind = false;
}

// castHelperDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "keysender.h"
#include "castHelperDLG.h"
#include "afxdialogex.h"
#include "macroObject.h"
#include "macroObjectManager.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "keysenderDlg.h"

//#define _TEST_ 1
// castHelperDLG 대화 상자입니다.
CString GetPath(int nFolder) // 0 : userdata / 1: current
{
	TCHAR szPath[1024] = { 0, };
	if (nFolder == 0)
	{
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
		{
			PathAppend(szPath, _T("Electronic Arts\\Dark Age of Camelot\\phoenix\\"));
		}
	}
	else
		if (nFolder == 1)
		{
			//GetWindowModuleFileName(macroObject::targetWindowHandle, szPath, 1024);
			DWORD dwProcessID = 0;
			::GetWindowThreadProcessId(macroObject::targetWindowHandle, &dwProcessID);
			// Get a handle to the process from the Process ID
			HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID);

			// Get the process name
			if (NULL != hProcess) {
				DWORD nExeName = sizeof(szPath) / sizeof(TCHAR);
				//  the QueryFullProcessImageNameW does not exist on W2K
				QueryFullProcessImageName(hProcess, 0, szPath, &nExeName);
				::CloseHandle(hProcess);
				std::wstring::size_type pos = std::wstring(szPath).find_last_of(_T("\\/"));
				wsprintf(szPath, _T("%s"), std::wstring(szPath).substr(0, pos).c_str());
			}
		}
	return CString(szPath);
}
/////////////////////////////////////////////////////////////////////////
std::vector<std::wstring> tokenize(std::wstring &in) {
	char sep = _T(',');
	std::wstring::size_type b = 0;
	std::vector<std::wstring> result;

	while ((b = in.find_first_not_of(sep, b)) != std::wstring::npos) {
		auto e = in.find_first_of(sep, b);
		result.push_back(in.substr(b, e - b));
		b = e;
	}
	return result;
}


////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(castHelperDLG, CDialogEx)
castHelperDLG *castHelperDLG::instance = nullptr;
castHelperDLG::castHelperDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(castHelperDLG::IDD, pParent)
{
	for (int i = 0; i < 8; i++)
	{
		m_miniGroup.player[i].key = nullptr;
	}
}

castHelperDLG::~castHelperDLG()
{

}
/*
std::vector<AUTOACTIONS> m_actionList;
std::vector<activePlayer> m_actiePlayer;
std::vector<CASTHELP_ACTION_KEY> m_helpActionKey;
*/
void castHelperDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAPTURE_RECT, m_captureDisplayRect);
	DDX_Control(pDX, IDC_COMBO_INILIST, m_iniListCombo);
}


BEGIN_MESSAGE_MAP(castHelperDLG, CDialogEx)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_INILIST, &castHelperDLG::OnSelchangeComboInilist)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &castHelperDLG::OnBnClickedButtonLoad)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_LOADFILE, &castHelperDLG::OnBnClickedButtonLoadfile)
END_MESSAGE_MAP()


// castHelperDLG 메시지 처리기입니다.

/////////////////////////////////////////////////////////////////////////
//
macroObject *castHelperDLG::_loadKEYValue(cJSON *from)
{
	if (from == nullptr)
	{
		return nullptr;
	}
	bool EXKEY[3] = { false, false, false };
	auto arryKey = cJSON_GetObjectItem(from, "VK");
	auto arryALT = cJSON_GetObjectItem(from, "alt");
	auto arryCTRL = cJSON_GetObjectItem(from, "ctrl");
	auto arrySHIFT = cJSON_GetObjectItem(from, "shift");

	EXKEY[0] = (arryALT != nullptr && arryALT->valueint != 0);
	EXKEY[1] = (arryCTRL != nullptr && arryCTRL->valueint != 0);
	EXKEY[2] = (arrySHIFT != nullptr && arrySHIFT->valueint != 0);

	DWORD vk = 0;

	if (strcmp(arryKey->valuestring, "none") != 0)
	{
		vk = macroObjectList::getStringToVK(KeyBindSTATE::KEYBOARD_BIND, arryKey->valuestring);
	}
	
	
	return  macroObjectList::makeKeyboard(vk, EXKEY[0], EXKEY[1], EXKEY[2], 100);
}


void castHelperDLG::_loadActionKeyList(cJSON *from)
{
	for (auto &kk : m_helpActionKey)
	{
		if (kk.key != nullptr)
		{
			delete kk.key;
		}
	}
	m_helpActionKey.clear();

	int arraySize = cJSON_GetArraySize(from);
	if (arraySize > 0)
	{
		for (int xi = 0; xi < arraySize; xi++)
		{
			cJSON *arrayObj = cJSON_GetArrayItem(from, xi);

			auto arryKey = cJSON_GetObjectItem(arrayObj, "action");

			CASTHELP_ACTION_KEY obj;
			obj.key = nullptr;
			obj.act = AUTOACTIONS::NONE;

			if (_stricmp(arryKey->valuestring, "demez") == 0)
			{
				obj.act = DEMEZ;
			}
			else
			if (_stricmp(arryKey->valuestring, "heal") == 0)
			{
				obj.act = HEAL;
			}
			else
			if (_stricmp(arryKey->valuestring, "dot") == 0)
			{
				obj.act = POISONED;
			}
			else
			if (_stricmp(arryKey->valuestring, "disease") == 0)
			{
				obj.act = DISEASED;
			}

			if (obj.act != AUTOACTIONS::NONE)
			{
				obj.key = _loadKEYValue(cJSON_GetObjectItem(arrayObj, "key"));

				if (obj.key != nullptr)
				{
					m_helpActionKey.push_back(obj);
				}
			}
		}
	}
}
void castHelperDLG::_loadActionList(cJSON *from)
{
	m_actionList.clear();
	int arraySize = cJSON_GetArraySize(from);
	if (arraySize > 0)
	{
		for (int xi = 0; xi < arraySize; xi++)
		{
			cJSON *arrayObj = cJSON_GetArrayItem(from, xi);

			if (_stricmp(arrayObj->valuestring, "demez") == 0)
			{
				m_actionList.push_back(DEMEZ);
			} else
			if (_stricmp(arrayObj->valuestring, "heal") == 0)
			{
				m_actionList.push_back(HEAL);
			}
			else
			if (_stricmp(arrayObj->valuestring, "dot") == 0)
			{
				m_actionList.push_back(POISONED);
			}
			else
			if (_stricmp(arrayObj->valuestring, "disease") == 0)
			{
				m_actionList.push_back(DISEASED);
			}
		}
	}
}
void castHelperDLG::_loadSlotInfo(cJSON *from)
{
	CString outString;
	miniGroupPlayerObject obj;
	int arraySize = cJSON_GetArraySize(from);
	if (arraySize > 0)
	{
		for (int xi = 0; xi < arraySize; xi++)
		{
			cJSON *arrayObj = cJSON_GetArrayItem(from, xi);
			if (arrayObj != nullptr)
			{
				auto slot = cJSON_GetObjectItem(arrayObj, "slot");
				auto size = cJSON_GetObjectItem(arrayObj, "size");
				auto bar = cJSON_GetObjectItem(arrayObj, "bar");
				auto name = cJSON_GetObjectItem(arrayObj, "name");



				obj.slot = slot->valueint;
				// full range //////////
				outString = ConvertMultibyteToUnicode(size->valuestring);
				auto vtRECT = tokenize(std::wstring(outString.GetString()));
				SetRect(&obj.rect,
					_ttoi(vtRECT[0].c_str()),
					_ttoi(vtRECT[1].c_str()),
					_ttoi(vtRECT[2].c_str()) + _ttoi(vtRECT[0].c_str()),
					_ttoi(vtRECT[3].c_str()) + _ttoi(vtRECT[1].c_str()));

				// hp ////////////////
				outString = ConvertMultibyteToUnicode(bar->valuestring);
				auto vtHP = tokenize(std::wstring(outString.GetString()));

				SetRect(&obj.HP,
					_ttoi(vtHP[0].c_str()),
					_ttoi(vtHP[1].c_str()),
					_ttoi(vtHP[0].c_str()) + m_miniGroup.HPWidth,
					_ttoi(vtHP[1].c_str()) + 5);
				
				// name //////////////
				outString = ConvertMultibyteToUnicode(name->valuestring);
				auto vtName = tokenize(std::wstring(outString.GetString()));

				SetRect(&obj.Name,
					_ttoi(vtName[0].c_str()),
					_ttoi(vtName[1].c_str()),
					_ttoi(vtName[0].c_str()) + m_miniGroup.nameWidth,
					_ttoi(vtName[1].c_str()) + 12);


				obj.key =_loadKEYValue(cJSON_GetObjectItem(arrayObj, "key"));

				if (m_miniGroup.player[xi].key != nullptr)
				{
					delete m_miniGroup.player[xi].key;
				}
				memcpy(&m_miniGroup.player[xi], &obj, sizeof(miniGroupPlayerObject));
			}
		}
	}
}


/////////////////////////////////////////////////////
//
bool getNameColor(RECT *namerect,int pitch, COLORREF &ret,int pixel,CImage *pIMG)
{
	int addArs = 0;
	BYTE R, G, B;
	int compareVal = 0;
	int top = namerect->top;

	BYTE *buffer = (BYTE*)pIMG->GetPixelAddress(namerect->left, top);
	int address = 0;
	for (int y = 0; y < 4; y++)
	{
		addArs = 0;
		for (int x = 0; x < 4; x++)
		{
			
			B = *(buffer + address + addArs);
			G = *(buffer + address + addArs + 1);
			R = *(buffer + address + addArs + 2);
			addArs += pixel;
			int cmpV = (int)(R + G + B) / 3;

			if (cmpV > 20)
			{
				if (compareVal < cmpV)
				{
					ret = RGB(R, G, B);
					compareVal = cmpV;
				}
			}
		}
		address += pitch;
	}
	return (compareVal != 0);
}

int getPlayerHP(RECT *hprect, CImage *pIMG, int pitch, int pixel)
{
	int ret = 0;
	BYTE *buffer = (BYTE*)pIMG->GetPixelAddress(hprect->left, hprect->top);
	int endAddress = (hprect->right - hprect->left) * pixel;
	BYTE R, G, B;
	for (int ad = 0; ad <= endAddress; ad += pixel)
	{
		B = *(buffer + ad);
		G = *(buffer + ad + 1);
		R = *(buffer + ad + 2);

		int cmpV = (R + G + B) / 3;
		if (cmpV < 200)
		{
			break;
		}
		ret++;
	}
	return ret;
}


/////////////////////////////////////////////////////
//
bool SortHighValue(const sortAction& info1, const sortAction& info2)
{
	return info1.sortOP < info2.sortOP;
}
activiteMacroObject_helper *castHelperDLG::getFirstAction(void)
{
	captureMiniGroup();
	if(checkPlayerSlot() == false) return nullptr;

	FIRSTACIONLIST actList;
	AUTOACTIONS outAct = AUTOACTIONS::NONE;

	int cnt = m_actionList.size() + 1;

	for (auto act : m_actionList)
	{
		outAct = act;
		for (auto &player : m_actiePlayer)
		{
			switch (act)
			{
				case DEMEZ:	
				{ 
					if (player.state == 2)
					{
						sortAction oo;
						oo.sortOP = (cnt * 1000 + 101 - player.hp) *10 + player.player.slot;
						oo.act = act;
						oo.obj = player.player.key;
						actList.push_back(oo);
					}
				} break;
				case HEAL: 	
				{ 
					if (player.state >= 0 && player.hp < m_miniGroup.DrawHealPercent)
					{
						sortAction oo;
						oo.sortOP = (cnt * 1000 + 101 - player.hp) * 10 + player.player.slot;
						oo.act = act;
						oo.obj = player.player.key;
						actList.push_back(oo);
					}
				} break;
				case POISONED: 
				{ 
					if (player.state == 3) 
					{
						sortAction oo;
						oo.sortOP = (cnt * 1000 + 101 - player.hp) * 10 + player.player.slot;
						oo.act = act;
						oo.obj = player.player.key;
						actList.push_back(oo);
					}
				} break;
				case DISEASED: 
				{ 
					if (player.state == 1) 
					{
						sortAction oo;
						oo.sortOP = (cnt * 1000 + 101 - player.hp) * 10 + player.player.slot;
						oo.act = act;
						oo.obj = player.player.key;
						actList.push_back(oo);
					}
				} break;
			}
		}
		cnt--;
	}

	if (actList.empty() == false)
	{

		std::sort(actList.begin(), actList.end(), SortHighValue);
		for (auto &aV : m_helpActionKey)
		{
			if (aV.act == (*actList.begin()).act)
			{
				return new activiteMacroObject_helper((*actList.begin()).obj, aV.key);
			}
		}
		
	}
	return nullptr;
}

bool castHelperDLG::checkPlayerSlot(void)
{
	if (m_capImage.IsNull() == true) return false;
	int pitch = m_capImage.GetPitch();

	int width = m_capImage.GetWidth();

	int pixel = abs(pitch) / width;

	COLORREF rgb;
	m_actiePlayer.clear();
	for (auto &slot : m_miniGroup.player)
	{
		// check name1
		if (getNameColor(&slot.Name, pitch, rgb, pixel, &m_capImage))
		{
			activePlayer player;
			memcpy(&player.player, &slot, sizeof(miniGroupPlayerObject));

			player.state = 0;
			player.hp = getPlayerHP(&slot.HP, &m_capImage, pitch, pixel) * 100 / m_miniGroup.HPWidth;
			if (rgb != RGB(255, 255, 255))
			{
				if (rgb == RGB(245, 0, 0) || player.hp <= 1) //dead
				{
					player.state = -1;
				}
				else
				{
					BYTE R = rgb & 0xFF;
					BYTE G = (rgb >> 8) & 0xFF;
					BYTE B = (rgb >> 16) & 0xFF;
					if (R > 200 && G < 160 && B < 60) // disease
					{
						player.state = 1;
					}
					else
					if (R < 20 && G > 200 && B > 200) // mez
					{
						player.state = 2;
					}
					else
					if (R < 160 && G > 200 && B < 150) // poisioned
					{
						player.state = 3;
					}
					
					
				}
			}
			m_actiePlayer.push_back(player);
		}
	}

	return (!m_actiePlayer.empty());
}


/////////////////////////////////////////////////////
//
void castHelperDLG::updateGroupInfo(cJSON *from)
{
	auto windowsize = cJSON_GetObjectItem(from, "size");
	auto hp = cJSON_GetObjectItem(from, "barwidth");
	auto name = cJSON_GetObjectItem(from, "namewidth");
	auto emptycolor = cJSON_GetObjectItem(from, "emptyRcolor");
	auto healpercent = cJSON_GetObjectItem(from, "healpercent");
	CString kk = ConvertMultibyteToUnicode(windowsize->valuestring);
	auto tk = tokenize(std::wstring(kk.GetString()));

	// base set
	if (healpercent != nullptr)
	{
		m_miniGroup.DrawHealPercent = healpercent->valueint;
	}
	else m_miniGroup.DrawHealPercent = 65;
	m_miniGroup.windowRect.x = _ttoi(tk[0].c_str());
	m_miniGroup.windowRect.y = _ttoi(tk[1].c_str());
	m_miniGroup.nameWidth = name->valueint;
	m_miniGroup.HPWidth = hp->valueint;
	m_miniGroup.HPemptyRColor = emptycolor->valueint;

	// array set
	auto enableaction = cJSON_GetObjectItem(from, "enableaction");
	auto slotlist = cJSON_GetObjectItem(from, "list");
	auto keylist = cJSON_GetObjectItem(from, "keylist");

	if (slotlist != nullptr)_loadSlotInfo(slotlist);
	if (enableaction != nullptr) _loadActionList(enableaction);
	if (keylist != nullptr) _loadActionKeyList(keylist);

}

/////////////////////////////////////////////////////////////////////////
//
void castHelperDLG::loadJSON(LPCTSTR filename)
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

		if (json != nullptr)
		{
			auto targetWindowJson = cJSON_GetObjectItem(json, "group");
			updateGroupInfo(targetWindowJson);
			cJSON_Delete(json);
		}
		///////////////////////////////////////////////

		delete jsonFileText;
	}
}

/////////////////////////////////////////////////////////////////////////
//
void castHelperDLG::loadFileList(void)
{
	CFileFind finder;
	auto path = GetPath(0) + _T("*.ini");
	m_iniListCombo.ResetContent();
	m_iniListCombo.AddString(_T("  케릭터파일을 선택해주세요  "));

	BOOL bWorking = finder.FindFile(path); //

	CString fileName;
	CString DirName;

	while (bWorking)
	{
		//다음 파일 / 폴더 가 존재하면다면 TRUE 반환
		bWorking = finder.FindNextFile();
		//파일 일때
		if (finder.IsArchived())
		{
			//파일의 이름
			CString _fileName = finder.GetFileName();

			// 현재폴더 상위폴더 썸네일파일은 제외
			if (_fileName == _T(".") || _fileName == _T("..")  ) continue;

			fileName = finder.GetFileTitle();
			if (finder.IsDirectory() == FALSE)
			{
				m_iniListCombo.AddString(fileName);
			}
	
		}
	}
	m_iniListCombo.SetCurSel(0);
}


/////////////////////////////////////////////////////////////////////////
//
void castHelperDLG::doShow(void)
{
	loadFileList();
}


/////////////////////////////////////////////////////////////////////////
//
BOOL castHelperDLG::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_captureDisplayRect.GetWindowRect(&m_cliRect);

	POINT tempP;
	tempP.x = m_cliRect.left;
	tempP.y = m_cliRect.top;
	::ScreenToClient(m_hWnd, &tempP);

	m_cliRect.right -= m_cliRect.left;
	m_cliRect.bottom -= m_cliRect.top;
	m_cliRect.left = tempP.x;
	m_cliRect.top = tempP.y;




#ifdef _TEST_
	castHelperDLG::instance = this;
	m_capImage.Load(_T("d:\\test.png"));
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


/////////////////////////////////////////////////////////////////////////
//
BOOL castHelperDLG::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return true;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


/////////////////////////////////////////////////////////////////////////
//
void castHelperDLG::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if (m_capImage.IsNull() == false)
	{
		m_capImage.Draw(dc.GetSafeHdc(), m_cliRect.left , m_cliRect.top);
	}
}


/////////////////////////////////////////////////////////////////////////
//
void castHelperDLG::OnSelchangeComboInilist()
{

}


/////////////////////////////////////////////////////////////////////////
//
void castHelperDLG::OnBnClickedButtonLoad()
{
	if (macroObject::targetWindowHandle == nullptr && CkeysenderDlg::g_isFullScrrenMode == false)
	{
		MessageBox(_T("적용할 윈도우를 선택해주세요."));
		return;
	}

	if (m_iniListCombo.GetCurSel() > 0)
	{
		CString filename;
		m_iniListCombo.GetWindowText(filename);
		CString fullpathname = GetPath(0) + filename +_T(".ini");
		TCHAR outString[512];
		GetPrivateProfileString(_T("Panels"), _T("MiniGroup"), _T("0,0"), outString, 256, fullpathname.GetString());

		auto value = tokenize(std::wstring(outString));

		if (value.size() > 2)
		{
			m_miniGroupPos.x = _tstoi(value[0].c_str());
			m_miniGroupPos.y = _tstoi(value[1].c_str());

			captureMiniGroup();
			this->InvalidateRect(NULL);
			castHelperDLG::instance = this;
		}
	}
}


void castHelperDLG::captureMiniGroup(void)
{
#ifdef _TEST_
	return;
#endif
	CRect poxx;

	if (CkeysenderDlg::g_isFullScrrenMode == false)	{
		poxx.SetRect(activeMacroObjectManager::targetWindowPosition.x + m_miniGroupPos.x,
			activeMacroObjectManager::targetWindowPosition.y + m_miniGroupPos.y,
			activeMacroObjectManager::targetWindowPosition.x + m_miniGroupPos.x + m_miniGroup.windowRect.x,
			activeMacroObjectManager::targetWindowPosition.y + m_miniGroupPos.y + m_miniGroup.windowRect.y);
	}
	else
	{
		poxx.SetRect(0, 0, m_miniGroup.windowRect.x, m_miniGroup.windowRect.y);
	}
	m_capImage.CaptureRect(poxx);
}

void castHelperDLG::OnDestroy()
{
	CDialogEx::OnDestroy();
	for (int i = 0; i < 8; i++)
	{
		if (m_miniGroup.player[i].key != nullptr) delete m_miniGroup.player[i].key;
	}

	for (auto &ak : m_helpActionKey)
	{
		if (ak.key != nullptr) delete ak.key;
	}

	m_helpActionKey.clear();
	m_actionList.clear();
	m_actiePlayer.clear();
}


void castHelperDLG::OnBnClickedButtonLoadfile()
{
	TCHAR szFilter[] = _T("Text (*.text;*.json;*.txt)|*.text;*.txt;*.json|All Files(*.*)|*.*||");

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

	if (IDOK == dlg.DoModal()) {
		this->loadJSON((TCHAR*)dlg.GetPathName().GetString());
	}
}

// inputObjectDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "keysender.h"
#include "inputObjectDialog.h"
#include "afxdialogex.h"
#include "virtualCode.h"
#include "BindKeySetDlg.h"
// inputObjectDialog 대화 상자입니다.
inputObjectDialog *inputObjectDialog::inputObjectDLG = nullptr;
IMPLEMENT_DYNAMIC(inputObjectDialog, CDialog)

inputObjectDialog::inputObjectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(inputObjectDialog::IDD, pParent)
{
	m_mouseMovePosition.x = 0;
	m_mouseMovePosition.y = 0;
	m_isNew = true;
	m_mouseBtn = 0;
	m_pushButton = false;
	m_extKey[0] = m_extKey[1] = m_extKey[2] =false;
	m_regikey = 0;
	m_delay = 0;
	m_returnValue = nullptr;
}

inputObjectDialog::~inputObjectDialog()
{
}


void inputObjectDialog::setDefualtValue(BINDKIND nType, macroObject *value)
{
	m_returnValue = value;
	m_inputType = nType;

	m_isNew = (bool)(value == nullptr);

	
	if (value != nullptr)
	{
		m_inputType = value->getType();
		m_delay = value->getDelay();


		switch (m_inputType)
		{
			case BINDKIND::KEYBOARD:
			{
				macroObjectKeyboard *pKey = dynamic_cast<macroObjectKeyboard*>(value);
				if (pKey != nullptr)
				{
					pKey->getState(m_regikey, m_extKey[0], m_extKey[1], m_extKey[2]);
				}
			} break;
			case BINDKIND::MOSEMOVE:
			{
				macroObjectMouseMove *pKey = dynamic_cast<macroObjectMouseMove*>(value);
				if (pKey != nullptr)
				{
					pKey->getState(m_mouseMovePosition.x, m_mouseMovePosition.y);
				}
			} break;
			case BINDKIND::MOSEDRAG:
			{
				macroObjectMouseDrag *pKey = dynamic_cast<macroObjectMouseDrag*>(value);
				if (pKey != nullptr)
				{
					pKey->getState(m_mouseMovePosition.x, m_mouseMovePosition.y, m_mouseBtn, m_pushButton);
				}
			} break;
			case BINDKIND::MOUSECLICK:
			{
				macroObjectMouseClick *pKey = dynamic_cast<macroObjectMouseClick*>(value); 
				if (pKey != nullptr)
				{
					pKey->getState(m_mouseBtn);
				}
			} break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//
void inputObjectDialog::onCaptureMousePosition(int x, int y)
{
	if (IsWindowEnabled() == FALSE)
	{
		return;
	}
	CRect poxx;
	CRect scr;

	poxx.SetRect(x - 28, y - 27, x + 28, y + 27);
	m_capImage.CaptureRect(poxx);
	scr.SetRect(m_imageStaticPoint.x, m_imageStaticPoint.y, m_imageStaticPoint.x + 56, m_imageStaticPoint.y + 54);

	this->InvalidateRect(&scr, 0);

	if (GetActiveWindow() != this)
	{
		SetActiveWindow();
	}

	m_mouseMovePosition.x = x;
	m_mouseMovePosition.y = y;
	if (m_inputKindCombo.GetCurSel() != BINDKIND::MOSEDRAG)
	{
		m_inputKindCombo.SetCurSel(BINDKIND::MOSEMOVE);
	}

	CString mmx;
	mmx.Format(_T("%d"), m_mouseMovePosition.x);
	SetDlgItemText(IDC_STATIC_X, mmx);

	mmx.Format(_T("%d"), m_mouseMovePosition.y);
	SetDlgItemText(IDC_STATIC_Y, mmx);

	inputObjectDialog::inputObjectDLG = nullptr;
}
void inputObjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_USE_CB, m_inputKindCombo);
	DDX_Control(pDX, IDC_COMBO2, m_keyboardListCombo);
	DDX_Control(pDX, IDC_MOUSEBTN_CB, m_mouseBtnCombo);
	DDX_Control(pDX, IDC_CAPTURE_RECT, m_captureDisplayRect);
}


BEGIN_MESSAGE_MAP(inputObjectDialog, CDialog)
	ON_BN_CLICKED(IDC_SAVE_BUTTON, &inputObjectDialog::OnBnClickedSaveButton)
	ON_BN_CLICKED(IDC_MOUSEMOVE_BTN, &inputObjectDialog::OnBnClickedMousemoveBtn)
	ON_WM_PAINT()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// inputObjectDialog 메시지 처리기입니다.



void inputObjectDialog::updateSelectBindKind(void)
{
	this->GetDlgItem(IDC_PAINT_KEYBOARD)->GetClientRect(&m_paintRect);
}

void inputObjectDialog::OnBnClickedSaveButton()
{
	macroObject *outObject = nullptr;
	CString outmessage;
	this->GetDlgItemText(IDC_DELAY, outmessage);
	DWORD delay = (DWORD)_ttoi(outmessage.GetString());

	m_inputType = (BINDKIND)m_inputKindCombo.GetCurSel();


	switch (m_inputType)
	{
		case BINDKIND::DELAY:
		{
			outObject = macroObjectList::makeDelay(delay);
		} break;
		case BINDKIND::KEYBOARD:
		{
			m_regikey = m_keyboardListCombo.GetItemData(m_keyboardListCombo.GetCurSel());
			if (m_regikey > 0)
			{
				m_extKey[0] = (bool)(this->IsDlgButtonChecked(IDC_CHECK_ALT) != 0);
				m_extKey[1] = (bool)(this->IsDlgButtonChecked(IDC_CHECK_CTRL) != 0);
				m_extKey[2] = (bool)(this->IsDlgButtonChecked(IDC_CHECK_SHIFT) != 0);
				outObject = macroObjectList::makeKeyboard(m_regikey, m_extKey[0], m_extKey[1], m_extKey[2], delay);
			}
			else
			{
				OnCancel();
				return;
			}
		} break;

		case BINDKIND::MOSEDRAG:
		{
			if (m_mouseBtnCombo.GetCurSel() > 0)
			{
				m_pushButton = (bool)(this->IsDlgButtonChecked(IDC_MOUSEBUTTONDOWN) != 0);
				m_mouseBtn = (BIND_MOUSEBUTTONKIND)m_mouseBtnCombo.GetCurSel();
				outObject = macroObjectList::makeMouseDrag(m_mouseMovePosition.x, m_mouseMovePosition.y, (BIND_MOUSEBUTTONKIND)m_mouseBtn, m_pushButton, delay);
			}
			else
			{
				OnCancel();
				return;
			}
		} break;
		case BINDKIND::MOUSECLICK:
		{
			if (m_mouseBtnCombo.GetCurSel()>0)
			{
				m_mouseBtn = (BIND_MOUSEBUTTONKIND)m_mouseBtnCombo.GetCurSel();
				outObject = macroObjectList::makeMouseClick((BIND_MOUSEBUTTONKIND)m_mouseBtn, delay);
			}
			else
			{
				OnCancel();
				return;
			}
		} break;

		case BINDKIND::MOSEMOVE:
		{
			outObject = macroObjectList::makeMouseMove(m_mouseMovePosition.x, m_mouseMovePosition.y, delay);
		} break;
	}

	if (outObject == nullptr)
	{
		OnCancel();
		return;
	}
	else
	{
		BindKeySetDlg::instance->m_editTarget->changeObjectFromList(m_returnValue, outObject);
	}

	OnOK();
}

BOOL inputObjectDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	int cnt = 0;
	int sel = 0;
	CString mmg;
	mmg.Format(_T("%d"), m_delay);
	SetDlgItemText(IDC_DELAY, mmg);
	///////// 입력 방식
	m_inputKindCombo.InsertString(BINDKIND::DELAY, _T("시간 지연"));
	m_inputKindCombo.InsertString(BINDKIND::KEYBOARD, _T("키보드입력"));
	m_inputKindCombo.InsertString(BINDKIND::MOUSECLICK, _T("마우스클릭"));
	m_inputKindCombo.InsertString(BINDKIND::MOSEMOVE, _T("마우스이동"));
	m_inputKindCombo.InsertString(BINDKIND::MOSEDRAG, _T("마우스드래그"));
	m_inputKindCombo.InsertString(BINDKIND::CLEAR_ALL_ACTIVE_ACTION, _T("반복행동취소"));
	
	m_inputKindCombo.SetCurSel((int)m_inputType);

	m_mouseBtnCombo.InsertString(0, _T("선택되지 않음"));
	m_mouseBtnCombo.InsertString(BIND_MOUSEBUTTONKIND::BTN_LEFT, _T("왼쪽 버튼"));
	m_mouseBtnCombo.InsertString(BIND_MOUSEBUTTONKIND::BTN_RIGHT, _T("오른쪽 버튼"));
	m_mouseBtnCombo.InsertString(BIND_MOUSEBUTTONKIND::BTN_MIDDLE, _T("중간 버튼"));

	if (m_inputType == BINDKIND::MOUSECLICK || m_inputType == BINDKIND::MOSEDRAG)
	{
		m_mouseBtnCombo.SetCurSel(m_mouseBtn);
	} else m_mouseBtnCombo.SetCurSel(0);

	if (m_inputType == BINDKIND::MOSEDRAG)
	{
		CheckDlgButton(IDC_MOUSEBUTTONDOWN, m_pushButton);
	}
	

	// 키보드
	std::wstring retv = _T("");
	for (auto &vkobject : VIRTULKEY_CHAR2VALUE_list)
	{
		if (vkobject.what == BINDKIND::KEYBOARD || vkobject.value == 0)
		{
			retv = ConvertMultibyteToUnicode((char*)vkobject.name.c_str()).GetString();
			int cur = m_keyboardListCombo.InsertString(cnt, retv.c_str());
			m_keyboardListCombo.SetItemData(cur, vkobject.value);

			if (m_inputType == BINDKIND::KEYBOARD)
			{
				if (vkobject.value == m_regikey)
				{
					sel = cur;
				}
			}
			cnt++;
		}
		
	}

	if (m_inputType == BINDKIND::KEYBOARD)//m_extKey
	{
		CheckDlgButton(IDC_CHECK_ALT, m_extKey[0]);
		CheckDlgButton(IDC_CHECK_CTRL, m_extKey[1]);
		CheckDlgButton(IDC_CHECK_SHIFT, m_extKey[2]);
	}

	m_keyboardListCombo.SetCurSel(sel);
	
	RECT cliRect;
	m_captureDisplayRect.GetWindowRect(&cliRect);
	m_imageStaticPoint.x = cliRect.left;
	m_imageStaticPoint.y = cliRect.top;
	::ScreenToClient(m_hWnd, &m_imageStaticPoint);

	if (m_inputType == BINDKIND::MOSEMOVE || m_inputType == BINDKIND::MOSEDRAG)
	{
		CString mmx;
		mmx.Format(_T("%d"), m_mouseMovePosition.x);
		SetDlgItemText(IDC_STATIC_X, mmx);

		mmx.Format(_T("%d"), m_mouseMovePosition.y);
		SetDlgItemText(IDC_STATIC_Y, mmx);

		CRect poxx;
		CRect scr;

		poxx.SetRect(m_mouseMovePosition.x - 28, m_mouseMovePosition.y - 27, m_mouseMovePosition.x + 28, m_mouseMovePosition.y + 27);
		m_capImage.CaptureRect(poxx);
		scr.SetRect(m_imageStaticPoint.x, m_imageStaticPoint.y, m_imageStaticPoint.x + 56, m_imageStaticPoint.y + 54);
	}

	updateSelectBindKind();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void inputObjectDialog::OnBnClickedMousemoveBtn()
{
	inputObjectDialog::inputObjectDLG = this;
}


void inputObjectDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if (m_capImage.IsNull() == false)
	{
		m_capImage.Draw(dc.GetSafeHdc(), m_imageStaticPoint.x, m_imageStaticPoint.y);
	}
}


void inputObjectDialog::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	inputObjectDialog::inputObjectDLG = nullptr;
	CDialog::OnClose();
}

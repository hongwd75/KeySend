// BindKeySetDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "keysender.h"
#include "BindKeySetDlg.h"
#include "afxdialogex.h"
#include "macroObjectManager.h"
#include "virtualCode.h"

#include "inputObjectDialog.h"
// BindKeySetDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(BindKeySetDlg, CDialog)
BindKeySetDlg *BindKeySetDlg::instance = nullptr;
BindKeySetDlg::BindKeySetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(BindKeySetDlg::IDD, pParent)
{
	m_isNew = false;
	m_editTarget = nullptr;
}

BindKeySetDlg::~BindKeySetDlg()
{
	BindKeySetDlg::instance = nullptr;
}

void BindKeySetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BINDKEYLIST, m_bindkeylist);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(BindKeySetDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_BTN, &BindKeySetDlg::OnBnClickedAddBtn)
	ON_BN_CLICKED(IDC_EDIT_BTN, &BindKeySetDlg::OnBnClickedEditBtn)
	ON_BN_CLICKED(IDC_REMOVE_BTN, &BindKeySetDlg::OnBnClickedRemoveBtn)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &BindKeySetDlg::OnDblclkList1)
	ON_BN_CLICKED(IDC_SAVE_BTN, &BindKeySetDlg::OnBnClickedSaveBtn)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// BindKeySetDlg �޽��� ó�����Դϴ�.


BOOL BindKeySetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int cnt = 0;
	int sel = 0;
	int cur = 0;
	std::wstring retv = _T("");
	for (auto &vkobject : VIRTULKEY_CHAR2VALUE_list)
	{
		if (vkobject.what != KeyBindSTATE::none && macroObjectManager::getInstance()->checkBindKey(vkobject.what, vkobject.value) != nullptr)
		{
			if (m_editTarget->isSame(vkobject.what, vkobject.value) == false)
			{
				continue;
			}
			else
			{
				sel = cnt;
			}
		}

		DWORD value = ((int)vkobject.what << 16) | (vkobject.value);

		retv = ConvertMultibyteToUnicode((char*)vkobject.name.c_str()).GetString();
		cur = m_bindkeylist.InsertString(cnt, retv.c_str());
		m_bindkeylist.SetItemData(cur, value);
		cnt++;
	}

	m_bindkeylist.SetCurSel(sel);
	CheckDlgButton(IDC_CHECK_REPEAT, (BOOL)(m_editTarget->isRepeat() == true));
	CheckDlgButton(IDC_CHECK_CASTHELP, (BOOL)(m_editTarget->isCastHelpOption() == true));
	
	SetDlgItemText(IDC_EDIT_TITLE, m_editTarget->getTitle().c_str());
	
	// set item list control
	m_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT); // Ÿ��Ʋ ���� 
	m_list.InsertColumn(0, _T("�Է±�"), LVCFMT_CENTER, 80, -1);
	m_list.InsertColumn(1, _T("����"), LVCFMT_RIGHT, 80, -1);
	m_list.InsertColumn(2, _T("�Է°�"), LVCFMT_LEFT, 200, -1);

	BindKeySetDlg::instance = this;
	updateItemList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


///////////////////////////////////////////////////////////////////////////////////////
//
void BindKeySetDlg::updateItemList(void)
{
	int cnt = 0;
	int itemcnt = 0;
	m_list.DeleteAllItems();
	auto itemlist = m_editTarget->getList();

	TCHAR timesize[64];
	for (auto &item : *itemlist)
	{
		wsprintf(timesize, _T("%d"), item->getDelay());
		cnt = m_list.InsertItem(cnt, item->getActionString().c_str());
		m_list.SetItem(cnt, 1, LVIF_TEXT, timesize, 0, 0, 0, NULL);
		m_list.SetItem(cnt, 2, LVIF_TEXT, item->getValueString().c_str(), 0, 0, 0, NULL);
		m_list.SetItemData(cnt, itemcnt);
		cnt++;
		itemcnt++;
	}
}


///////////////////////////////////////////////////////////////////////////////////////
//
void BindKeySetDlg::OnBnClickedAddBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	inputObjectDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		this->updateItemList();
	}
}


///////////////////////////////////////////////////////////////////////////////////////
//
void BindKeySetDlg::OnBnClickedEditBtn()
{
	int selitem = m_list.GetSelectionMark();
	if (selitem != -1)
	{
		auto itemlist = m_editTarget->getList();
		int at = (int)m_list.GetItemData(selitem);
		if (itemlist->size() > at)
		{
			auto value = itemlist->at(at);
			inputObjectDialog dlg;
			dlg.setDefualtValue(BINDKIND::DELAY, value);
			if (dlg.DoModal() == IDOK)
			{
				this->updateItemList();
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////
//
void BindKeySetDlg::OnBnClickedRemoveBtn()
{
	int selitem = m_list.GetSelectionMark();
	if (selitem != -1)
	{
		auto itemlist = m_editTarget->getList();
		int at = (int)m_list.GetItemData(selitem);
		if (itemlist->size() > at)
		{
			auto value = itemlist->at(at);

			auto itr = itemlist->begin();
			while (itr != itemlist->end())
			{
				if ((*itr) == value)
				{
					delete (*itr);
					value = nullptr;
					itemlist->erase(itr);
					m_list.DeleteItem(selitem);
					updateItemList();
					return;
				}
				itr++;
			}

		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////
//
void BindKeySetDlg::OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem >= 0)
	{
		OnBnClickedEditBtn();
	}
	else
	{
		OnBnClickedAddBtn();
	}
	*pResult = 0;
}


///////////////////////////////////////////////////////////////////////////////////////
//
void BindKeySetDlg::OnBnClickedSaveBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString retText;
	GetDlgItemText(IDC_EDIT_TITLE, retText);
	m_editTarget->changetTitle(retText.GetString());

	DWORD value = m_bindkeylist.GetItemData(m_bindkeylist.GetCurSel());
	KeyBindSTATE state = (KeyBindSTATE)(value >> 16);
	int keyValue = value & 0xFFFF;
	bool isCastHelp = (bool)(IsDlgButtonChecked(IDC_CHECK_CASTHELP) != 0);
	if (m_editTarget->getSize() == 0 && isCastHelp == false)
	{
		if (MessageBox(_T("��ϵ� ������ �����ϴ�. �׷��� ����Ͻðڽ��ϱ�?"), _T("���� ���"), MB_YESNO) == IDNO)
		{
			CDialog::OnCancel();
			return;
		}
	}

	m_editTarget->updateBindKey(state, keyValue);
	m_editTarget->changeRepeat((bool)(IsDlgButtonChecked(IDC_CHECK_REPEAT) != 0));
	m_editTarget->changeCastHelpOption(isCastHelp);
	CDialog::OnOK();
}


void BindKeySetDlg::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	BindKeySetDlg::instance = nullptr;
	CDialog::OnClose();
}

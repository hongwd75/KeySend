#pragma once
#include "ScreenImage.h"
#include "afxwin.h"
#include <vector>
#include "json.h"
#include "macroObjectManager.h"

enum AUTOACTIONS
{
	NONE = 0,
	DEMEZ = 1,
	HEAL = 2,
	POISONED = 3,
	DISEASED = 4
};

// castHelperDLG ��ȭ �����Դϴ�.
typedef struct
{
	int slot;
	RECT rect;
	RECT HP;
	RECT Name;

	macroObject *key;
} miniGroupPlayerObject;

typedef struct
{
	POINT windowRect;
	int HPWidth;
	int nameWidth;
	int HPemptyRColor;
	int DrawHealPercent;		// ���� �� %���� �ϴ°�.
	miniGroupPlayerObject  player[8];
} miniGroupObject;

////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	miniGroupPlayerObject player;
	int hp;
	int state;
} activePlayer;

/////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	AUTOACTIONS act;
	macroObject *key;
} CASTHELP_ACTION_KEY;


typedef struct
{
	int sortOP; //�켱���� 1 * 1000 + (101 - hp)
	AUTOACTIONS act;
	macroObject *obj;
} sortAction;

typedef std::vector<miniGroupPlayerObject> USINGPLAYER_SOLT;	// ����ϴ� ����
typedef std::vector<sortAction> FIRSTACIONLIST;					// ������ ���� �� ���ΰ�.

/////////////////////////////////////////////////////////////////////////////////////
class castHelperDLG : public CDialogEx
{
	DECLARE_DYNAMIC(castHelperDLG)

public:
	static castHelperDLG *instance;
	castHelperDLG(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~castHelperDLG();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FASTCASTGROUPDLG };

protected:
	miniGroupObject m_miniGroup;
	USINGPLAYER_SOLT m_players;
	std::vector<AUTOACTIONS> m_actionList;
	std::vector<activePlayer> m_actiePlayer;
	std::vector<CASTHELP_ACTION_KEY> m_helpActionKey;

public:
	activiteMacroObject_helper *getFirstAction(void);

protected:
	void captureMiniGroup(void);
	bool checkPlayerSlot(void);
	void updateGroupInfo(cJSON *from);
	void _loadSlotInfo(cJSON *from);
	void _loadActionList(cJSON *from);
	void _loadActionKeyList(cJSON *from);
	macroObject *_loadKEYValue(cJSON *from);
	POINT m_miniGroupPos;
	RECT m_cliRect;
	CScreenImage m_capImage;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	void loadJSON(LPCTSTR filename);
	void loadFileList(void);
	DECLARE_MESSAGE_MAP()
public:
	void doShow(void);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CStatic m_captureDisplayRect;
	afx_msg void OnPaint();
	afx_msg void OnSelchangeComboInilist();
	afx_msg void OnBnClickedButtonLoad();
	CComboBox m_iniListCombo;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonLoadfile();
};

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

// castHelperDLG 대화 상자입니다.
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
	int DrawHealPercent;		// 힐은 몇 %에서 하는가.
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
	int sortOP; //우선순위 1 * 1000 + (101 - hp)
	AUTOACTIONS act;
	macroObject *obj;
} sortAction;

typedef std::vector<miniGroupPlayerObject> USINGPLAYER_SOLT;	// 사용하는 슬롯
typedef std::vector<sortAction> FIRSTACIONLIST;					// 무엇을 먼저 할 것인가.

/////////////////////////////////////////////////////////////////////////////////////
class castHelperDLG : public CDialogEx
{
	DECLARE_DYNAMIC(castHelperDLG)

public:
	static castHelperDLG *instance;
	castHelperDLG(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~castHelperDLG();

// 대화 상자 데이터입니다.
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

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
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

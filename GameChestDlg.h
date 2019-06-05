#if !defined(AFX_GAMECHESTDLG_H__EC996EA1_8E04_11D6_9662_84CDA925FE4F__INCLUDED_)
#define AFX_GAMECHESTDLG_H__EC996EA1_8E04_11D6_9662_84CDA925FE4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GameChestDlg.h : header file
//

#include "resource.h"
#include "HydrofinDlg.h"	// Added by ClassView

extern char strHydrofinPath[MAX_PATH];	// путь откуда запускается эта программа без названия файла (Пример: "c:\tmp\hydrofin\" )
extern char strRunGame[MAX_PATH]; // запускаемая игра (путь и расширение)
extern char strRunGameName[MAX_PATH]; // запускаемая игра (только имя без пути и расширения)
extern char strRunGamePath[MAX_PATH]; // запускаемая игра (тоько путь)
extern int iOldiniFile_GameChest_NumberOfGames;	// староое знаение iniFile.GameChest.NumberOfGames

#define MAX_GAMES 2000 // повторяется в iniFile.cpp, Hydrofin_Input.cpp, Init.cpp, ColorsConfig.cpp

extern void OpenIniFile( void );

extern struct structIniFile {
	struct {
		char PythonDLLPath[MAX_PATH];
		char HydraPath[MAX_PATH];
		char PythonLibsPath[MAX_PATH];
		int ConfigLanguage;
	} ConfigGeneral;
	struct {
		int UndoSlots;
		long UndoMemory;
		int UndoMemoryLimitMethod;
		int TextLines;
		long TextMemory;
		int TextLimitMethod;
	} ConfigMemory;
	struct {
		char Text[255];
		int Blinking;
		int BlinkingTime;
		int BlinkingBgColor;
		int Lines;
	} ConfigMore;
	struct {
		int Background;
		int Normal;
		int Italics;
		int Bold;
		int Underscore;
		int Inverted;
		int StatusLineBackground;
		int StatusLineNormal;
		int StatusLineItalics;
		int StatusLineBold;
		int StatusLineUnderscore;
		int StatusLineInverted;
	} ConfigColors;
	struct {
		int InsertModeLines;
		int OverwriteModeLines;
	} ConfigCursor;
	struct {
		int PTag;
		int CyrillicR;
		int Tabs;
	} ConfigInputOutput;
	struct {
		char DefaultGamesDirectory[MAX_PATH];
		char LastGamesDirectory[MAX_PATH];
		int StartingGamesDirectorySavingMethod;
		int ColumnWidth1;
		int ColumnWidth2;
		int ColumnWidth3;
		int NumberOfGames;
		char *pGame[MAX_GAMES]; // MAX_GAMES = 2000
	} GameChest;
} iniFile;

extern int Error( char *strError ); // выодит текст ошибки и завершает игру

/////////////////////////////////////////////////////////////////////////////
// GameChestDlg dialog

class GameChestDlg : public CDialog
{
// Construction
public:
	GameChestDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(GameChestDlg)
	enum { IDD = IDD_GAMECHEST };
	CButton	m_cRemoveGame;
	CButton	m_cRunGame;
	CListCtrl m_cGameList;
	CString	m_strGameComment;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GameChestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(GameChestDlg)
	afx_msg void OnAddgamefile();
	afx_msg void OnClickGamelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReturnGamelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownGamelist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDblclkGamelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoGamelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRemovegame();
	afx_msg void OnRclickGamelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedGamelist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool bCalledFromKeyDown;
	typedef struct myitem_tag { 
		LPSTR aCols[3]; 
	} MYITEM;

	void RemoveGameFromList( int iEntry );
	LPSTR DupString(LPSTR lpsz);
	bool m_bHeaderEnabled;
	void OnExit( bool bOk );
	int iLastClicked; // последняя игра на которую кликнули в списке игр
	int iItemsSelectedCount; //	количество выбранных элементов в списке игр
	int iSelectedItem[32000]; // все выбранные элементы начиная с верхнего
	char *ListGameName[MAX_GAMES];
	char *ListText[MAX_GAMES];
	char *ListGameType[MAX_GAMES];
	char *ListGamePath[MAX_GAMES];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMECHESTDLG_H__EC996EA1_8E04_11D6_9662_84CDA925FE4F__INCLUDED_)

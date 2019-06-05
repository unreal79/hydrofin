
#if !defined(AFX_HYDROFIN_H__C778D8ED_32BA_11D6_9662_C93ABB095B68__INCLUDED_)
#define AFX_HYDROFIN_H__C778D8ED_32BA_11D6_9662_C93ABB095B68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "GameChestDlg.h"

#define ATTR_SPACE			1
#define ATTR_NORMAL			2
#define ATTR_BOLD			4
#define ATTR_ITALIC			8
#define ATTR_UNDERSCORE		16
// #define ATTR_END			32	//��-�������� ���� ������� �� �����
#define ATTR_SKIP			128

#define SCRIPT_ALL				0 // �������� ��� � ����
#define SCRIPT_TEXT_ONLY		1 // �������� ������ ����� ����
#define SCRIPT_COMMANDS_ONLY	2 // �������� ������ ������� ������


// Global variables:
int iCurrentScroll = 0;	// ������� ����� ������ ���� �������� � ������� ���������� ����� �������
char strOutText[64000]; // �������� �����, ���������� �� ������������� � ������� ScrOut() ������. ���������� ���������� ����� �� ��������/����������� ������ ������ ��� ��� ������ ScrOut()
unsigned char OutAttr[64000]; // �������� �������� ���������� ������
unsigned char PrevTag[1000]; // �������� ���������� �������� �������� �����
bool bNormalFlag = true;		// ��� ParseOutputString(). ���������� �����
bool bBoldFlag = false;			// ���������� ������ (<b>, </b>) �����
bool bItalicFlag = false;		//	.	.	. ������
bool bUnderscoreFlag = false;	//	.	.	. ������������
char *pOutputStringText[64000]; // 64000 ���������� �� ������ � ������� ������� (����� �� �������?)
char *pOutputStringAttr[64000];
int iOutputHistoryCount = 0; // ��������� ��������� �� *pOutputStringText[64000] ������� ����� ����������� ��� �������� ���������� (�����������) ������� ������
int iScrollDownCount = 2; // ������ ���������� ����� �������, ��� �������� ������� ������
char strPromptStringText[255]; // ��� �������� ����� ������ ����� (� Prompt) ��� ��������� ���������� �������
char strPromptStringAttr[255];
int iStatusNumLines = 0; // ���������� ����� � ��������� ������

FILE *hndlScriptFile = NULL;	// ������� ����� ����������
int iScriptType; // ��� ����������� (SCRIPT_ALL, SCRIPT_TEXT_ONLY, SCRIPT_COMMANDS_ONLY)
bool bScripting = false; // ���� �� ������ �����������
static char strScriptFileName[MAX_PATH]; // ��� �������� ����� ��������� ������������ (� �����������) �����	�����������
bool bPrintingCommandHistory = false; // ���� �� � ������ ������ ����� ������� ������ (��� ������� ArrowUp/Down). ��� ��������� ����� � ������� � ���� ������������ �������� �������� ������� ������

char strHydrofinPath[MAX_PATH];	// ���� ������ ����������� ��� ��������� ��� �������� ����� (������: "c:\tmp\hydrofin\" )

static char strSaveCurrent[255] = ""; // ��� ��� ������ ����� ������ ��� ������� Print() ����� ���������
static char AttrSaveCurrent[255] = ""; // �������� ��������� ������ ��� ����������� ��������� � ����� �������

static bool bPyErrorCatching = false;

void ClearLine( int iLineNum );	// ������� ���� ������ iLineNum
int StatusLineOut( int iNumLines, char *strStatusLine ); // ����� ��������� ������ (��� ����������)
int ScrOut( char *strBuf ); // ������ ������, ������������ � ��� �������, ������ ���� �� ����� 64000 �������� (������ ����)
void print( char *strPrint, int x, int y );	// ��� ������� ������������ �������� ����� ������
void print( char *strPrint );
void Print( char *strPrint, int x, int y );
void Print( char *strPrint );
void SetCurPos( int x, int y ); // ��������� ������� �������
void SetCursorVisibility( bool bOnOff ); // ��������� �������� ��������� �������
void ScrollUp( int iScrollLines, int iScrollStartLine, int iScrollEndLine ); // ������������� (�����������) ����� �����

void ScriptChar( char c ); // ������ ������ ������� � ���� �����������
void StartTranscript( int iTranscriptType );	// ������ ����������������� �������� ����
void StopTranscript( FILE * hndlFile); // ������������� �����������������
void ContinueTranscript( char *strFileName ); // ���������� �����������������
void EndTranscript( FILE *hndlFile ); // ��������� �����������������

int Error( char *strError ); // ������ ����� ������ � ��������� ����
void ExitGame( void ); // ��������� ����, ���������� ������ � ��.

char strRunGame[MAX_PATH]; // ����������� ���� (���� � ����������)
char strRunGamePath[MAX_PATH]; // ����������� ���� (����� ����)
char strRunGameName[MAX_PATH]; // ����������� ���� (������ ��� ��� ���� � ����������)

// For Hydrofin_Input.cpp

extern char strInput[255]; // ������ ��� �������� � �������� � Hydra ����� ������
extern bool bCtrlPressed; // ������ �� Ctrl
extern bool bInsertMode; // ��������� ����� �����
extern int iInputCurPos; // ������� ��������� ������� �� X �� ������ ����� ������������ ���������� �������
extern char *pInputString[64000]; // ��� �������� ������� ������
extern int iHistoryCount; // ��������� ��������� �� *pInputString[64000] ������� ����� ����������� ��� �������� ������� ������
extern int iHistoryBack; // ��� ������� uparrow/downarrow �� ������� �������� � ������� ������ �� ���������
extern bool bPromptJustTyped; // ������ ��� (�.�. ��� �� ���� ������) ���� ������� �������

extern void SetCursorInsert( bool bInsert ); // ���������� ������ ������� ���� � ��������� ������� (������� ������) ���� � ���������� �����
extern int ScrIn( char *strPrompt ); // ���������� ��� ����� ������� �������
extern bool ProcessKey( WORD wKey, char cKey, DWORD dwCntrlKeys, BOOL bPressed ); // ��������� ������� �������. ���������� �� ScrIn
extern void EscPressed( void ); // ������ Esc
extern void BackToPrompt( void ); // ��� �������� ������� ������� ���������� ����� � ����� �������
extern void SetCursorInsert( bool bInsert ); // �������� ����� �������

// For Init.cpp
extern HANDLE hndlInput; // ������� ������ ����� (����������)
extern HANDLE hndlOutput; // ������� ������ ������ (������)
extern CONSOLE_SCREEN_BUFFER_INFO scbiOldCosoleInfo; // ��� �������� ���������� � ���������� �������� �������� �������
extern int iWindowsVersion; // ������ Windows

extern void Init( char *strHydrofinFile ); // ������������ ������������� Hydrofin
extern void GetFileTitle( char *strIn, char *strOut, int iBufLen ); // ���������� ����� ����� ��� ���� � ����������
extern void SetHydrofinPath( char *argv0 ); // ������������� ���� � hydrofin.exe
extern int GetWindowsVersion( void ); // ������� ������ Windows
extern CONSOLE_SCREEN_BUFFER_INFO resizeConBufAndWindow( HANDLE hConsole, 
												SHORT xSize, SHORT ySize, // �������� ������� ������ � ���� ������
												bool bAllowError = true );

// For iniFile.cpp

extern void OpenIniFile( void );
extern void SaveIniFile( void );
extern struct structIniFile iniFile;

// For ColorsConfig.cpp
extern void ColorsConfig( void );

#endif // !defined(AFX_HYDROFIN_H__C778D8ED_32BA_11D6_9662_C93ABB095B68__INCLUDED_)


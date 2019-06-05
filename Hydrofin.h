
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
// #define ATTR_END			32	//по-видимому этот атрибут не нужен
#define ATTR_SKIP			128

#define SCRIPT_ALL				0 // выводить все в файл
#define SCRIPT_TEXT_ONLY		1 // выводить только текст игры
#define SCRIPT_COMMANDS_ONLY	2 // выводить только команды игрока


// Global variables:
int iCurrentScroll = 0;	// Сколько строк текста было выведено с момента последнего ввода команды
char strOutText[64000]; // Содержит текст, копируемый из передаваемого в функцию ScrOut() текста. Переменная глобальная чтобы не выделять/освобеждать память каждый раз при вызове ScrOut()
unsigned char OutAttr[64000]; // Содержит атрибуты выводимого текста
unsigned char PrevTag[1000]; // Содержит предыдущие значения вложеных тэгов
bool bNormalFlag = true;		// Для ParseOutputString(). Нормальный текст
bool bBoldFlag = false;			// Обозначает жирный (<b>, </b>) текст
bool bItalicFlag = false;		//	.	.	. курсив
bool bUnderscoreFlag = false;	//	.	.	. подчеркнутый
char *pOutputStringText[64000]; // 64000 указателей на строки в истории экранов (Может не хватить?)
char *pOutputStringAttr[64000];
int iOutputHistoryCount = 0; // следующий указатель из *pOutputStringText[64000] который будет использован для хранения предыдущих (заскроленых) строчек вывода
int iScrollDownCount = 2; // Отсчет количества строк отступа, при листании экранов вывода
char strPromptStringText[255]; // Для хранения одной строки вводи (с Prompt) при просмотре предыдущих экранов
char strPromptStringAttr[255];
int iStatusNumLines = 0; // количество строк в статусной строке

FILE *hndlScriptFile = NULL;	// Хэндлер файла стенораммы
int iScriptType; // Тип стенограммы (SCRIPT_ALL, SCRIPT_TEXT_ONLY, SCRIPT_COMMANDS_ONLY)
bool bScripting = false; // Идет ли запись стенограммы
static char strScriptFileName[MAX_PATH]; // Для хранения имени регулярно открываемого (и зарываемого) файла	стенограммы
bool bPrintingCommandHistory = false; // Идет ли в данный момент вывод истории команд (при нажатии ArrowUp/Down). Для излечения глюка с выводом в файл транскрипции процесса листания истории команд

char strHydrofinPath[MAX_PATH];	// путь откуда запускается эта программа без названия файла (Пример: "c:\tmp\hydrofin\" )

static char strSaveCurrent[255] = ""; // Эти две строки нужны только для функции Print() чтобы содержать
static char AttrSaveCurrent[255] = ""; // значения выводимой строки для дальнейшего занесения в буфер экранов

static bool bPyErrorCatching = false;

void ClearLine( int iLineNum );	// Очистка всей строки iLineNum
int StatusLineOut( int iNumLines, char *strStatusLine ); // Вывод статусной строки (или нескольких)
int ScrOut( char *strBuf ); // размер строки, передаваемый в эту функцию, должен быть не более 64000 символов (считая тэги)
void print( char *strPrint, int x, int y );	// эта функция осуществляет основной вывод текста
void print( char *strPrint );
void Print( char *strPrint, int x, int y );
void Print( char *strPrint );
void SetCurPos( int x, int y ); // установка позиции курсора
void SetCursorVisibility( bool bOnOff ); // установка признака видимости курсора
void ScrollUp( int iScrollLines, int iScrollStartLine, int iScrollEndLine ); // скроллировать (передвигать) экран вверх

void ScriptChar( char c ); // запись одного символа в файл стенограммы
void StartTranscript( int iTranscriptType );	// начать стенографирования процесса игры
void StopTranscript( FILE * hndlFile); // приостановить стенографирование
void ContinueTranscript( char *strFileName ); // проболжить стенографирование
void EndTranscript( FILE *hndlFile ); // закончить стенографирование

int Error( char *strError ); // выодит текст ошибки и завершает игру
void ExitGame( void ); // завершает игру, освобождая память и пр.

char strRunGame[MAX_PATH]; // запускаемая игра (путь и расширение)
char strRunGamePath[MAX_PATH]; // запускаемая игра (тоько путь)
char strRunGameName[MAX_PATH]; // запускаемая игра (только имя без пути и расширения)

// For Hydrofin_Input.cpp

extern char strInput[255]; // строка для хранения и передачи в Hydra ввода игрока
extern bool bCtrlPressed; // Нажата ли Ctrl
extern bool bInsertMode; // Действует режим ввода
extern int iInputCurPos; // Текущее положение курсора по X на строке ввода относительно начального символа
extern char *pInputString[64000]; // Для хранения истории команд
extern int iHistoryCount; // следующий указатель из *pInputString[64000] который будет использован для хранения истории команд
extern int iHistoryBack; // При нажатии uparrow/downarrow на сколько отходить в истории команд от последней
extern bool bPromptJustTyped; // Только что (т.е. еще не было вывода) была введена команда

extern void SetCursorInsert( bool bInsert ); // установить размер курсора либо в положение вставки (большой курсор) либо в нормальный режим
extern int ScrIn( char *strPrompt ); // Вызывается для ввода команды игроком
extern bool ProcessKey( WORD wKey, char cKey, DWORD dwCntrlKeys, BOOL bPressed ); // обработка нажатой клавиши. Выхывается из ScrIn
extern void EscPressed( void ); // Нажата Esc
extern void BackToPrompt( void ); // при листании истории экранов возвращает назад к вводу команды
extern void SetCursorInsert( bool bInsert ); // Изменить режим курсора

// For Init.cpp
extern HANDLE hndlInput; // хэндлер буфера ввода (клавиатура)
extern HANDLE hndlOutput; // хэндлер буфера вывода (экрана)
extern CONSOLE_SCREEN_BUFFER_INFO scbiOldCosoleInfo; // для хранения информации о предыдущем значении размеров консоли
extern int iWindowsVersion; // Версия Windows

extern void Init( char *strHydrofinFile ); // Осуществляет инициализацию Hydrofin
extern void GetFileTitle( char *strIn, char *strOut, int iBufLen ); // извлечение имени файла без пути и расширения
extern void SetHydrofinPath( char *argv0 ); // Устанавливает путь к hydrofin.exe
extern int GetWindowsVersion( void ); // находим версию Windows
extern CONSOLE_SCREEN_BUFFER_INFO resizeConBufAndWindow( HANDLE hConsole, 
												SHORT xSize, SHORT ySize, // изменяет размеры буфера и окна вывода
												bool bAllowError = true );

// For iniFile.cpp

extern void OpenIniFile( void );
extern void SaveIniFile( void );
extern struct structIniFile iniFile;

// For ColorsConfig.cpp
extern void ColorsConfig( void );

#endif // !defined(AFX_HYDROFIN_H__C778D8ED_32BA_11D6_9662_C93ABB095B68__INCLUDED_)


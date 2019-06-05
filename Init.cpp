#include "hydrofin.h"
//#include "init.h"
#include "stdafx.h"

HANDLE hndlInput;
HANDLE hndlOutput;
CONSOLE_SCREEN_BUFFER_INFO scbiOldCosoleInfo;
int iWindowsVersion; // ¬ерси€ Windows

extern void print( char *strPrint, int x, int y);
extern void Print( char *strPrint, int x, int y);
extern void Print( char *strPrint );
extern void print( char *strPrint );
extern void SetCursorVisibility( bool bOnOff );
extern void SetCurPos( int x, int y );
extern int Error( char *strRrror );
extern void ExitGame( void );

extern char strHydrofinPath[MAX_PATH];	// путь откуда запускаетс€ эта программа без названи€ файла (ѕример: "c:\tmp\hydrofin\" )

#define MAX_GAMES 2000 // повтор€етс€ в iniFile.cpp, Hydrofin_Input.cpp, GameChestDlg.h, ColorsConfig.cpp

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

void ClearScreen( void );


/*********************************************************************
* FUNCTION: resizeConBufAndWindow(HANDLE hConsole, SHORT xSize,      *
*                                 SHORT ySize)                       *
*                                                                    *
* PURPOSE: resize both the console output buffer and the console     *
*          window to the given x and y size parameters               *
*                                                                    *
* INPUT: the console output handle to resize, and the required x and *
*        y size to resize the buffer and window to.                  *
*                                                                    *
* COMMENTS: Note that care must be taken to resize the correct item  *
*           first; you cannot have a console buffer that is smaller  *
*           than the console window.                                 *
*********************************************************************/

CONSOLE_SCREEN_BUFFER_INFO resizeConBufAndWindow( HANDLE hConsole, 
												  SHORT xSize, SHORT ySize,
												  bool bAllowError = true ) // дл€ предотвращени€ повторного освобождени€ пам€ти при ошибке восстановлени€ консоли в ExitGame()
{
  CONSOLE_SCREEN_BUFFER_INFO csbi, /* hold current console buffer info */
							 csbi1;	// дл€ созранени€ первоначального значени€ размера консоли
  BOOL bSuccess;
  SMALL_RECT srWindowRect; /* hold the new console size */
  COORD coordScreen;

  bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
  if ( !bSuccess )
	if ( bAllowError )
		Error( "Fatal error: fail to GetConsoleScreenBufferInfo()" );
	else {
		printf( "Fatal error: fail to GetConsoleScreenBufferInfo()" );
		return csbi;
	}
  csbi1 = csbi;
  /* get the largest size we can size the console window to */
  coordScreen = GetLargestConsoleWindowSize(hConsole);
  if ( !(coordScreen.X | coordScreen.Y) ) 
	if ( bAllowError )
		Error( "Fatal error: fail to GetLargestConsoleWindowSize()" );
	else {
		printf( "Fatal error: fail to GetLargestConsoleWindowSize()" );
		return csbi1;
	}
  /* define the new console window size and scroll position */
  srWindowRect.Right = (SHORT) (min(xSize, coordScreen.X) - 1);
  srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);
  srWindowRect.Left = srWindowRect.Top = (SHORT) 0;
  /* define the new console buffer size */
  coordScreen.X = xSize;
  coordScreen.Y = ySize;
  /* if the current buffer is larger than what we want, resize the */
  /* console window first, then the buffer */
  if ( (DWORD) csbi.dwSize.X * csbi.dwSize.Y > (DWORD) xSize * ySize )
  {
    bSuccess = SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect);
    if ( !bSuccess )
		if ( bAllowError )
			Error( "Fatal error: fail to SetConsoleWindowInfo()" );
		else {
			printf( "Fatal error: fail to SetConsoleWindowInfo()" );
			return csbi1;
		}
    bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);
	if ( !bSuccess )
		if ( bAllowError )
			Error( "Fatal error: fail to SetConsoleScreenBufferSize()" );
		else {
			printf( "Fatal error: fail to SetConsoleScreenBufferSize()" );
			return csbi1;
		}
  }
  /* if the current buffer is smaller than what we want, resize the */
  /* buffer first, then the console window */
  if ( (DWORD) csbi.dwSize.X * csbi.dwSize.Y < (DWORD) xSize * ySize )
  {
    bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);
	if ( !bSuccess )
		if ( bAllowError )
			Error( "Fatal error: fail to SetConsoleScreenBufferSize()" );
		else {
			printf( "Fatal error: fail to SetConsoleScreenBufferSize()" );
			return csbi1;
		}
    bSuccess = SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect); // под Win98 вроде работает, но возвращает bSuccsess == false
	if ( !bSuccess )
		if ( bAllowError )
			Error( "Fatal error: fail to SetConsoleWindowInfo()" );
		else {
			printf( "Fatal error: fail to SetConsoleWindowInfo()" );
			return csbi1;
		}
  }
  /* if the current buffer *is* the size we want, don't do anything! */
  return csbi1;
}

BOOL CtrlHandler( DWORD dwCtrlType ) { 
	switch ( dwCtrlType ) { 
	case CTRL_BREAK_EVENT:
	case CTRL_C_EVENT: /* Handle the CTRL+C signal. */ 
						// почему-то не работает
	case CTRL_CLOSE_EVENT: /* CTRL+CLOSE: confirm that the user wants to exit. */ 
						   // почему-то не работает	под Win98
		MessageBeep( MB_ICONEXCLAMATION );
		ExitGame();
		return TRUE; 

	/* Pass other signals to the next handler. */ 

	case CTRL_LOGOFF_EVENT: 

	case CTRL_SHUTDOWN_EVENT: 

	default: 
		return FALSE; 
	} 
} 

void GetFileName( char *strIn, char *strOut, int iBufLen ) {
	int iLastSlash = 0;
	int i, j;

	for ( i = 0; i < iBufLen && i < lstrlen( strIn ); i++ )
		if ( strIn[i] == '\\' )
			iLastSlash = i + 1;
	for ( j = 0, i = iLastSlash; i < iBufLen && i < lstrlen( strIn ); i++, j++ )
		strOut[j] =	strIn[i];
	strOut[j] = 0;
}

void GetFileTitle( char *strIn, char *strOut, int iBufLen ) {
	GetFileName( strIn, strOut, iBufLen );
	
	int i;
	int iLastDot = lstrlen( strOut ) + 1;

	for ( i = 0; i < iBufLen && i < lstrlen( strIn ); i++ )
		if ( strIn[i] == '.' )
			iLastDot = i + 1;
	lstrcpyn( strOut, strIn, iLastDot );
//	for ( i = 0; i < iLastDot && i < iBufLen; i++ )
//		strOut[i] =	strIn[i];
//	strOut[i] = 0;
}

void Init( char *strHydrofinFile ) {

//	SECURITY_ATTRIBUTES saOutput;
//	COORD coordOutput;
//	SMALL_RECT srctOutput;
	char strTitle[255] = "";
	char strTmp[1000];
	
	hndlInput = GetStdHandle(STD_INPUT_HANDLE);
	if ( hndlInput == INVALID_HANDLE_VALUE ) {
		CharToOem( strTmp, "Fatal error: невозможно получить хэндлер \
потока ввода\n");
		printf( strTmp );
		exit( 1 );
	}
	if ( !SetConsoleMode( hndlInput, 0L ) ) {
		CharToOem( strTmp, "Fatal error: невозможно установить режим \
потока ввода\n" );
		printf( strTmp );
		exit( 1 );
	}

	hndlOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	if ( hndlOutput == INVALID_HANDLE_VALUE ) {
		CharToOem( strTmp, "Fatal error: невозможно получить хэндлер \
потока вывода\n");
		printf( strTmp );
		exit( 1 );
	}

	bool bSuccess = SetConsoleCtrlHandler( // ƒл€ перехвата сообщений типа CTRL+BREAK, CTRL_CLOSE_EVENT
				(PHANDLER_ROUTINE) CtrlHandler,		/* handler function */ 
				TRUE );								/* add to list      */ 
	if ( !bSuccess ) {
		CharToOem( strTmp, "Fatal error: невозможно установить хэндлер \
сообщений типа CTRL+BREAK, CTRL_CLOSE_EVENT\n" );
		printf( strTmp );
		exit( 1 );
	}

	scbiOldCosoleInfo = resizeConBufAndWindow( hndlOutput, 80, 25 );

	GetFileTitle( strHydrofinFile, strTitle, MAX_PATH );
//	lstrcpy( strTitle, strHydrofinFile );
	lstrcat( strTitle, " - Hydrofin" );
	SetConsoleTitle( strTitle ); // устанавливаем заголовок окна Hydrofin

/*	ќказалось, все это не нужно. ѕока не нужно.
	saOutput.nLength = sizeof(saOutput);
	saOutput.lpSecurityDescriptor = NULL;
	saOutput.bInheritHandle = TRUE;
	hndlOutput = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			&saOutput,
			CONSOLE_TEXTMODE_BUFFER,
			NULL ) ;
	if ( hndlOutput == INVALID_HANDLE_VALUE)
		Error("Error: fail to set output handle\n");
	
	CONSOLE_SCREEN_BUFFER_INFO csbiTmp;

	if ( !GetConsoleScreenBufferInfo(hndlOutput, &csbiTmp) )
		printf("Bad");
	printf("%i %i %i %i %i",
		csbiTmp.dwMaximumWindowSize.X, csbiTmp.dwCursorPosition.X,
		csbiTmp.dwSize.X, csbiTmp.srWindow.Bottom, csbiTmp.wAttributes);

	srctOutput.Top = 2;
	srctOutput.Left = 2;
	srctOutput.Bottom = 20;
	srctOutput.Right = 79;
	if ( !SetConsoleWindowInfo(hndlOutput, FALSE, &srctOutput) )
		Error("Error: fail to set window size\n");

	coordOutput.X = 79;
	coordOutput.Y = 24;
	if ( !SetConsoleScreenBufferSize(hndlOutput, coordOutput) )
		Error("Error: fail to set buffer size\n");
*/

//	SetStdHandle( STD_INPUT_HANDLE, hndlInput );
//	SetCursorVisibility( false );
	ClearScreen();
	SetCurPos(0, 0);
}

void SetHydrofinPath( char *argv0 ) {
	char strStartingTitle[MAX_PATH];
	char strPathTmp[MAX_PATH];

	GetFileName( argv0, strStartingTitle, MAX_PATH );
	for ( int i = 0; i < lstrlen( argv0 ) - lstrlen( strStartingTitle ); i++ )
		strHydrofinPath[i] = argv0[i];
	strHydrofinPath[i] = 0;

	switch( iWindowsVersion ) {
	case 3:	// Win 3.x ?
	case 4:	// Win98
		break;
	case 5:	// Win2000
		GetCurrentDirectory( MAX_PATH, strPathTmp );
		lstrcat( strPathTmp, "\\" );
		if ( lstrcmp( strHydrofinPath, strPathTmp ) && strHydrofinPath[1] != ':' ) { // под Win2000 при старте например из FAR в argv[0] передаетс€ относительный путь
			lstrcat( strPathTmp, strHydrofinPath );
			lstrcpy( strHydrofinPath, strPathTmp );
		}
//	printf( "GetFileTitle():%s\n", strStartingTitle );
//	printf( "argv0:%s\nGetCurrentDirectory():%s\nFindExecutable():%s\n", 
//			argv0, strPathTmp, strHydrofinPath );
//	printf( "GetCommandLine():%s\n", GetCommandLine() );
//	FindExecutable( argv0, strHydrofinPath, strPathTmp );
//	printf( "FindExecutable()%s\n", strPathTmp );
	case 6:
	default:
		break;
	}
	
/*	
	LPSTR lp;
	lp = &strHydrofinPath[0];
	GetFullPathName( strStartingTitle, MAX_PATH, strHydrofinPath, &lp );
	printf( "GetFullPathName():%s:%s\n", strPathTmp, lp );

	WIN32_FIND_DATA wfdHydrofin;
	HANDLE hSearch = FindFirstFile( argv0, &wfdHydrofin );
	lstrcpy( strHydrofinPath, wfdHydrofin.cFileName );
	printf( "FindFirstFile:%s\n", strHydrofinPath );

	char *strTmp[200];
	SearchPath( strPathTmp, strHydrofinPath, NULL, MAX_PATH, strStartingFile, strTmp );
	printf( "SearchPath:%s\n", strStartingFile );

*/
}

void ClearScreen( void ) {
	DWORD dwTmp;
	COORD coordTmp;

	coordTmp.X = 0;
	coordTmp.Y = 0;
	FillConsoleOutputAttribute( hndlOutput, iniFile.ConfigColors.Background |
			iniFile.ConfigColors.Normal, 80*25, coordTmp, &dwTmp );
	FillConsoleOutputCharacter( hndlOutput, ' ', 80*25, coordTmp, &dwTmp );
}

int GetWindowsVersion( void ) {
	OSVERSIONINFO oviVersionInformation;
	oviVersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &oviVersionInformation );
	iWindowsVersion = oviVersionInformation.dwMajorVersion;
	printf( "Windows major version: %d\n", iWindowsVersion );
	return iWindowsVersion;
}
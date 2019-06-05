#include "hydrofin.h"
//#include "init.h"
#include "stdafx.h"

extern char strHydrofinPath[MAX_PATH]; // путь откуда запускается эта программа без названия файла (Пример: "c:\tmp\hydrofin\" )
extern int Error( char *strRrror );

int iOldiniFile_GameChest_NumberOfGames = -1; // староое знаение iniFile.GameChest.NumberOfGames

#define MAX_GAMES 2000 // повторяется в GameChestDlg.h, Hydrofin_Input.cpp, Init.cpp, ColorsConfig.cpp

struct structIniFile {
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
};

structIniFile iniFile;

void ReadAllFromIniFile( char *strIniFile ) {
	char strBuf[MAX_PATH];

	// [Config.General]
	GetPrivateProfileString( "Config.General", "PythonDLLPath", "python20.dll",
			iniFile.ConfigGeneral.PythonDLLPath, MAX_PATH, strIniFile );
	GetPrivateProfileString( "Config.General", "HydraPath", ".",
			iniFile.ConfigGeneral.HydraPath, MAX_PATH, strIniFile );
	GetPrivateProfileString( "Config.General", "PythonLibsPath", ".",
			iniFile.ConfigGeneral.PythonLibsPath, MAX_PATH, strIniFile );
	GetPrivateProfileString( "Config.General", "ConfigLanguage", "Russian",
			strBuf, MAX_PATH, strIniFile );
	if ( strcmp(strBuf, "Russian") == 0 )
		iniFile.ConfigGeneral.ConfigLanguage = 1;
	else if ( strcmp( strBuf, "English") == 0 )
		iniFile.ConfigGeneral.ConfigLanguage = 2;
	// [Config.Memory]
	iniFile.ConfigMemory.UndoSlots = GetPrivateProfileInt( "Config.Memory", 
			"UndoSlots", 1000, strIniFile );
	iniFile.ConfigMemory.UndoMemory = GetPrivateProfileInt( "Config.Memory", 
			"UndoMemory", 16777216, strIniFile );
	GetPrivateProfileString( "Config.Memory", "UndoMemoryLimitMethod", "Memory",
			strBuf, MAX_PATH, strIniFile );
	if ( strcmp( strBuf, "Memory" ) == 0 )
		iniFile.ConfigMemory.UndoMemoryLimitMethod = 1;
	else if ( strcmp( strBuf, "Slots" ) == 0 )
		iniFile.ConfigMemory.UndoMemoryLimitMethod = 2;
	iniFile.ConfigMemory.TextLines = GetPrivateProfileInt( "Config.Memory", 
			"TextLines", 64000, strIniFile );
	iniFile.ConfigMemory.TextMemory = GetPrivateProfileInt( "Config.Memory", 
			"TextMemory", 204800, strIniFile );
	GetPrivateProfileString( "Config.Memory", "TextLimitMethod", "Lines",
			strBuf, MAX_PATH, strIniFile );
	if ( strcmp( strBuf, "Lines" ) == 0 )
		iniFile.ConfigMemory.TextLimitMethod = 1;
	else if ( strcmp( strBuf, "Memory" ) == 0 )
		iniFile.ConfigMemory.TextLimitMethod = 2;
	// [Config.More]
	GetPrivateProfileString( "Config.More", "Text", "[MORE]",
			iniFile.ConfigMore.Text, 255, strIniFile );
	iniFile.ConfigMore.Lines = GetPrivateProfileInt( "Config.More", 
			"Lines", -1, strIniFile );
	GetPrivateProfileString( "Config.More", "Blinking", "True",
			strBuf, MAX_PATH, strIniFile );
	if ( strcmp( strBuf, "True" ) == 0 )
		iniFile.ConfigMore.Blinking	= 1;
	else if ( strcmp( strBuf, "False" ) == 0 )
		iniFile.ConfigMore.Blinking	= 0;
	iniFile.ConfigMore.BlinkingTime = GetPrivateProfileInt( "Config.More", 
			"BlinkingTime", 1000, strIniFile );
	iniFile.ConfigMore.BlinkingBgColor = GetPrivateProfileInt( "Config.More", 
			"BlinkingBgColor", 0, strIniFile ); // выбрать цвет
	// [Config.Colors]
	iniFile.ConfigColors.Background = GetPrivateProfileInt( "Config.Colors", 
			"Background",  BACKGROUND_BLUE, strIniFile );
	iniFile.ConfigColors.Normal = GetPrivateProfileInt( "Config.Colors", 
			"Normal", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, 
			strIniFile );
	iniFile.ConfigColors.Italics = GetPrivateProfileInt( "Config.Colors", 
			"Italics", FOREGROUND_INTENSITY | FOREGROUND_GREEN | 
			FOREGROUND_RED, strIniFile );
	iniFile.ConfigColors.Bold = GetPrivateProfileInt( "Config.Colors", 
			"Bold", FOREGROUND_INTENSITY | FOREGROUND_RED | 
			FOREGROUND_GREEN | FOREGROUND_BLUE, strIniFile );
	iniFile.ConfigColors.Underscore = GetPrivateProfileInt( "Config.Colors", 
			"Underscore", FOREGROUND_INTENSITY |	FOREGROUND_RED, strIniFile );
	iniFile.ConfigColors.Inverted = GetPrivateProfileInt( "Config.Colors", 
			"Inverted", 0, strIniFile );
	iniFile.ConfigColors.StatusLineBackground = GetPrivateProfileInt( "Config.Colors", 
			"StatusLineBackground", BACKGROUND_BLUE | BACKGROUND_GREEN | 
			BACKGROUND_RED, strIniFile );
	iniFile.ConfigColors.StatusLineNormal = GetPrivateProfileInt( "Config.Colors", 
			"StatusLineNormal", FOREGROUND_BLUE, strIniFile );
	iniFile.ConfigColors.StatusLineItalics = GetPrivateProfileInt( "Config.Colors", 
			"StatusLineItalics", FOREGROUND_INTENSITY | FOREGROUND_GREEN | 
			FOREGROUND_RED, strIniFile );
	iniFile.ConfigColors.StatusLineBold = GetPrivateProfileInt( "Config.Colors", 
			"StatusLineBold", FOREGROUND_INTENSITY | FOREGROUND_RED | 
			FOREGROUND_GREEN | FOREGROUND_BLUE, strIniFile );
	iniFile.ConfigColors.StatusLineUnderscore = GetPrivateProfileInt( "Config.Colors", 
			"StatusLineUnderscore", FOREGROUND_INTENSITY | FOREGROUND_RED, 
			strIniFile );
	iniFile.ConfigColors.StatusLineInverted = GetPrivateProfileInt( "Config.Colors", 
			"StatusLineInverted", 0, strIniFile );
	// [Config.Cursor]
	iniFile.ConfigCursor.InsertModeLines = GetPrivateProfileInt( "Config.Cursor", 
			"InsertModeLines", 50, strIniFile );
	iniFile.ConfigCursor.OverwriteModeLines = GetPrivateProfileInt( "Config.Cursor", 
			"OverwriteModeLines", 10, strIniFile );
	// [Config.Input&Output]
	GetPrivateProfileString( "Config.Input&Output", "</P>Tag", "NewLine",
			strBuf, MAX_PATH, strIniFile );
	if ( strcmp( strBuf, "NewLine" ) == 0 )
		iniFile.ConfigInputOutput.PTag = 1;
	else if ( strcmp( strBuf, "Ignore" ) == 0 )
		iniFile.ConfigInputOutput.PTag = 0;
	GetPrivateProfileString( "Config.Input&Output", "CyrillicR", "Replace",
			strBuf, MAX_PATH, strIniFile );
	if ( strcmp( strBuf, "Replace" ) == 0 )
		iniFile.ConfigInputOutput.CyrillicR = 1;
	else if ( strcmp( strBuf, "Ignore" ) == 0 )
		iniFile.ConfigInputOutput.CyrillicR = 0;
	GetPrivateProfileString( "Config.Input&Output", "Tabs", "Ignore",
			strBuf, MAX_PATH, strIniFile );
	if ( strcmp( strBuf, "Ignore" ) == 0 )
		iniFile.ConfigInputOutput.Tabs = 0;
	else if ( strcmp( strBuf, "Spaces" ) == 0 )
		iniFile.ConfigInputOutput.Tabs = 1;
	// [Game Chest]
	GetPrivateProfileString( "Game Chest", "DefaultGamesDirectory", "", 
			iniFile.GameChest.DefaultGamesDirectory, MAX_PATH, strIniFile );
	GetPrivateProfileString( "Game Chest", "StartingGamesDirectorySavingMethod", 
			"DefaultDir", strBuf, MAX_PATH, strIniFile );
	if ( strcmp( strBuf, "DefaultDir" ) == 0 )
		iniFile.GameChest.StartingGamesDirectorySavingMethod = 0;
	else if ( strcmp( strBuf, "LastDir" ) == 0 )
		iniFile.GameChest.StartingGamesDirectorySavingMethod = 1;
	iniFile.GameChest.ColumnWidth1 = GetPrivateProfileInt( "Game Chest", 
			"ColumnWidth1", 60,	strIniFile );
	iniFile.GameChest.ColumnWidth2 = GetPrivateProfileInt( "Game Chest", 
			"ColumnWidth2", 600, strIniFile );
	iniFile.GameChest.ColumnWidth3 = GetPrivateProfileInt( "Game Chest", 
			"ColumnWidth3", 60,	strIniFile );
	iniFile.GameChest.ColumnWidth3 = GetPrivateProfileInt( "Game Chest", 
			"ColumnWidth3", 60,	strIniFile );
	iniFile.GameChest.NumberOfGames = GetPrivateProfileInt( "Game Chest", 
			"NumberOfGames", 0, strIniFile );
	if ( iniFile.GameChest.NumberOfGames > MAX_GAMES )
		iniFile.GameChest.NumberOfGames = MAX_GAMES;
	char strGame[10];
	char strTmp[MAX_PATH * 3];
	for ( int i = 1; i <= iniFile.GameChest.NumberOfGames; i++ ) {
		wsprintf( strGame, "Game%i", i );
		GetPrivateProfileString( "Game Chest", strGame, "Ошибка в Hydrofin.ini",
				strBuf, MAX_PATH, strIniFile );
		wsprintf( strTmp, "\"%s\"", strBuf ); // Восттанавливам кавычки
		iniFile.GameChest.pGame[i - 1] = (char *)malloc( lstrlen( strTmp ) + 1 );
		if ( iniFile.GameChest.pGame[i - 1] == NULL )
			Error( "Фатальная ошибка: невозможно отвести память для информации \
из \"Hydrofin.ini\"" );
		lstrcpy( iniFile.GameChest.pGame[i - 1], strTmp );
	}
//	WritePrivateProfileString( "Config.Cursor", "InsertLines", 
//			"tmp blah blah", strIniFile ); works fine

//	exit(0);
}

void WriteAllToIniFile( char *strIniFile ) {
	char strBuf[MAX_PATH];

	//TODO
	WritePrivateProfileString( "Config.General", "PythonDLLPath", 
			iniFile.ConfigGeneral.PythonDLLPath, strIniFile );
	// [Config.General]
	WritePrivateProfileString( "Config.General", "PythonDLLPath",
			iniFile.ConfigGeneral.PythonDLLPath, strIniFile );
	WritePrivateProfileString( "Config.General", "HydraPath",
			iniFile.ConfigGeneral.HydraPath, strIniFile );
	WritePrivateProfileString( "Config.General", "PythonLibsPath",
			iniFile.ConfigGeneral.PythonLibsPath, strIniFile );
	switch ( iniFile.ConfigGeneral.ConfigLanguage ) {
	case 1:
		lstrcpy( strBuf, "Russian" );
		break;
	case 2:
		lstrcpy( strBuf, "English" );
		break;
	default:
		lstrcpy( strBuf, "Russian" );
	}
	WritePrivateProfileString( "Config.General", "ConfigLanguage",
			strBuf, strIniFile );
	// [Config.Memory]
	wsprintf( strBuf, "%d", iniFile.ConfigMemory.UndoSlots );
	WritePrivateProfileString( "Config.Memory", "UndoSlots", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigMemory.UndoMemory );
	WritePrivateProfileString( "Config.Memory", "UndoMemory", 
			strBuf, strIniFile );
	switch ( iniFile.ConfigMemory.UndoMemoryLimitMethod ) {
	case 1:
		lstrcpy( strBuf, "Memory" );
		break;
	case 2:
		lstrcpy( strBuf, "Slots" );
		break;
	default:
		lstrcpy( strBuf, "Memory" );
	}
	WritePrivateProfileString( "Config.Memory", "UndoMemoryLimitMethod", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigMemory.TextLines );
	WritePrivateProfileString( "Config.Memory", "TextLines", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigMemory.TextMemory );
	WritePrivateProfileString( "Config.Memory", "TextMemory", 
			strBuf, strIniFile );
	switch ( iniFile.ConfigMemory.TextLimitMethod ) { 
	case 1:
		lstrcpy( strBuf, "Lines" );
		break;
	case 2:
		lstrcpy( strBuf, "Memory" );
		break;
	default:
		lstrcpy( strBuf, "Lines" );
	}
	WritePrivateProfileString( "Config.Memory", "TextLimitMethod",
			strBuf, strIniFile );
	// [Config.More]
	WritePrivateProfileString( "Config.More", "Text",
			iniFile.ConfigMore.Text, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigMore.Lines );
	WritePrivateProfileString( "Config.More", "Lines", strBuf, strIniFile );
	switch ( iniFile.ConfigMore.Blinking ) {
	case 1:
		lstrcpy( strBuf, "True" );
		break;
	case 0:
		lstrcpy( strBuf, "False" );
		break;
	default:
		lstrcpy( strBuf, "True" );
	}
	WritePrivateProfileString( "Config.More", "Blinking", strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigMore.BlinkingTime );
	WritePrivateProfileString( "Config.More", "BlinkingTime", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigMore.BlinkingBgColor );
	WritePrivateProfileString( "Config.More", "BlinkingBgColor", 
			strBuf, strIniFile );
	// [Config.Colors]
	wsprintf( strBuf, "%d", iniFile.ConfigColors.Background );
	WritePrivateProfileString( "Config.Colors", "Background", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigColors.Normal );
	WritePrivateProfileString( "Config.Colors", "Normal", strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigColors.Italics );
	WritePrivateProfileString( "Config.Colors", "Italics", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigColors.Bold );
	WritePrivateProfileString( "Config.Colors", "Bold", strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigColors.Underscore );
	WritePrivateProfileString( "Config.Colors", "Underscore", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigColors.Inverted );
	WritePrivateProfileString( "Config.Colors", "Inverted", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigColors.StatusLineBackground );
	WritePrivateProfileString( "Config.Colors", "StatusLineBackground", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigColors.StatusLineNormal );
	WritePrivateProfileString( "Config.Colors", "StatusLineNormal", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigColors.StatusLineItalics );
	WritePrivateProfileString( "Config.Colors", "StatusLineItalics", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigColors.StatusLineBold );
	WritePrivateProfileString( "Config.Colors", "StatusLineBold", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigColors.StatusLineUnderscore );
	WritePrivateProfileString( "Config.Colors", "StatusLineUnderscore", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigColors.StatusLineInverted );
	WritePrivateProfileString( "Config.Colors", "StatusLineInverted", 
			strBuf, strIniFile );
	// [Config.Cursor]
	wsprintf( strBuf, "%d", iniFile.ConfigCursor.InsertModeLines );
	WritePrivateProfileString( "Config.Cursor", "InsertModeLines", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.ConfigCursor.OverwriteModeLines );
	WritePrivateProfileString( "Config.Cursor", "OverwriteModeLines", 
			strBuf, strIniFile );
	// [Config.Input&Output]
	switch ( iniFile.ConfigInputOutput.PTag ) {
	case 1:
		lstrcpy( strBuf, "NewLine" );
		break;
	case 0:
		lstrcpy( strBuf, "Ignore" );
		break;
	default:
		lstrcpy( strBuf, "NewLine" );
	}
	WritePrivateProfileString( "Config.Input&Output", "</P>Tag",
			strBuf, strIniFile );
	switch ( iniFile.ConfigInputOutput.CyrillicR ) {
	case 1:
		lstrcpy( strBuf, "Replace" );
		break;
	case 0:
		lstrcpy( strBuf, "Ignore" );
		break;
	default:
		lstrcpy( strBuf, "Replace" );
	}
	WritePrivateProfileString( "Config.Input&Output", "CyrillicR", 
			strBuf, strIniFile );
	switch ( iniFile.ConfigInputOutput.Tabs ) {
	case 0:
		lstrcpy( strBuf, "Ignore" );
		break;
	case 1:
		lstrcpy( strBuf, "Spaces" );
		break;
	default:
		lstrcpy( strBuf, "Ignore" );
	}
	WritePrivateProfileString( "Config.Input&Output", "Tabs", 
			strBuf, strIniFile );
	// [Game Chest]
	WritePrivateProfileString( "Game Chest", "DefaultGamesDirectory", 
			iniFile.GameChest.DefaultGamesDirectory, strIniFile );
	switch ( iniFile.GameChest.StartingGamesDirectorySavingMethod ) {
	case 0:
		lstrcpy( strBuf, "DefaultDir" );
		break;
	case 1:
		lstrcpy( strBuf, "LastDir" );
		break;
	default:
		lstrcpy( strBuf, "DefaultDir" );
	}
	WritePrivateProfileString( "Game Chest", 
			"StartingGamesDirectorySavingMethod", strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.GameChest.ColumnWidth1 );
	WritePrivateProfileString( "Game Chest", "ColumnWidth1", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.GameChest.ColumnWidth2 );
	WritePrivateProfileString( "Game Chest", "ColumnWidth2", 
			strBuf, strIniFile );
	wsprintf( strBuf, "%d", iniFile.GameChest.ColumnWidth3 );
	WritePrivateProfileString( "Game Chest", "ColumnWidth3", 
			strBuf,	strIniFile );
	wsprintf( strBuf, "%d", iniFile.GameChest.ColumnWidth3 );
	WritePrivateProfileString( "Game Chest", "ColumnWidth3", 
			strBuf,	strIniFile );
	if ( iniFile.GameChest.NumberOfGames > MAX_GAMES )
		iniFile.GameChest.NumberOfGames = MAX_GAMES;
	wsprintf( strBuf, "%d", iniFile.GameChest.NumberOfGames );
	WritePrivateProfileString( "Game Chest", "NumberOfGames", 
			strBuf, strIniFile );
	for ( int i = 1; i <= iniFile.GameChest.NumberOfGames; i++ ) {
		char strGame[10];
		wsprintf( strGame, "Game%i", i );
		WritePrivateProfileString( "Game Chest", strGame, 
				iniFile.GameChest.pGame[i - 1], strIniFile );
	}
	for ( int j = i; j <= iOldiniFile_GameChest_NumberOfGames; j++ ) {
		char strGame[10];
		wsprintf( strGame, "Game%i", j );
		WritePrivateProfileString( "Game Chest", strGame, NULL, strIniFile );
	}
}

bool CreateDefaultIniFile( char *strIniFile ) {
	FILE *hndlIniFile = NULL;
	char strTmp[255];	  

	if ( (hndlIniFile = fopen(strIniFile, "w")) == NULL )
		return false;
	fclose( hndlIniFile );
	wsprintf( strTmp, "Создание файла %s\n", strIniFile );
	CharToOem( strTmp, strTmp );
	printf( strTmp );
	ReadAllFromIniFile( strIniFile );
	return true;
}

void OpenIniFile( void ) {
	char strIniFile[MAX_PATH]; // полный путь до .ini файла
	FILE *hndlIniFile = NULL;

	lstrcpy( strIniFile, strHydrofinPath );
	lstrcat( strIniFile, "hydrofin.ini" );

	if ( (hndlIniFile = fopen(strIniFile, "r")) == NULL ) {
		if ( !CreateDefaultIniFile( strIniFile ) ) {
			char strTmp[200];
			CharToOem( "Фатальная ошибка: невозможно открыть или создать \
файл \"hydrofin.ini\"", strTmp );
			printf( strTmp );
			exit( 1 );
		}
	} else {
		fclose( hndlIniFile );
		char strTmp[255];
		wsprintf( strTmp, "Чтение файла %s\n", strIniFile );
		CharToOem( strTmp, strTmp );
		printf( strTmp );
		ReadAllFromIniFile( strIniFile );
	}
}

void SaveIniFile( void ) {
		char strIniFile[MAX_PATH]; // полный путь до .ini файла
	FILE *hndlIniFile = NULL;

	lstrcpy( strIniFile, strHydrofinPath );
	lstrcat( strIniFile, "hydrofin.ini" );

	if ( (hndlIniFile = fopen(strIniFile, "r")) == NULL ) {
		if ( !CreateDefaultIniFile( strIniFile ) ) {
			char strTmp[200];
			CharToOem( "Фатальная ошибка: невозможно открыть или создать \
файл \"hydrofin.ini\"", strTmp );
			printf( strTmp );
			exit( 1 );
	}
	} else {
		fclose( hndlIniFile );
		char strTmp[255];
		CharToOem( "Запись hydrofin.ini файла\n", strTmp );
		printf( strTmp );
		WriteAllToIniFile( strIniFile );
	}
}

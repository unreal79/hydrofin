#include "hydrofin.h"
#include "stdafx.h"

extern HANDLE hndlInput;
extern HANDLE hndlOutput;
extern void SetCursorVisibility( bool bOnOff );
extern void SetCurPos( int x, int y ); // установка позиции курсора
extern int StatusLineOut( int iNumLines, char *strStatusLine );
extern int StatusLineOut( int iNumLines = 0 );
extern int StatusLineOut( char *strStatusLine );
extern void PrintXY( char x, char y, char *strSource, int color, 
					 bool bCharToOem = true );

#define MAX_GAMES 2000 // повторяется в iniFile.cpp, Hydrofin_Input.cpp, Init.cpp, GameChestDlg.h

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

/*
void ColorsConfig_RefreshScreen( int iBitFlag, int Colors[7] ) {
	if ( iBitFlag == 1 )
		iBitFlag = 0xFFF;
	if ( iBitFlag & 2 )	{
		PrintXY( 3, 1, "Обычный текст", Colors[0] | Colors[1] );
		PrintXY( 0, 6, "Normal", Colors[0] | Colors[1] );
	}

};*/

void ColorsConfig_WriteMenu( bool bBorder = true, bool bBrightBorder = false, bool bWriteText = true ) { // Вспомогательная функция для отрисовки текста в меню
	if ( bBorder ) {
		int iCol;
		if ( bBrightBorder )
			iCol = 15 | 16;
		else
			iCol = 7 | 16;
		if ( bWriteText )
			for ( int j = 0; j < 20; j++ ) // очищаем место под меню
				PrintXY( 8, 3 + j, "                                          ",
						iCol, false );
		// Рисуем рамку вокруг текста
		PrintXY( 8, 3, "\311",	iCol, false );
		for ( int j = 1; j < 9; j++ )
			PrintXY( 8 + j, 3, "\315", iCol, false );
		PrintXY( 17, 3, " Цвета основного текста ", iCol );
		for ( j = 1; j < 9; j++ )
			PrintXY( 40 + j, 3, "\315", iCol, false );
		PrintXY( 49, 3, "\273",	iCol, false );

		for ( j = 1; j < 19; j++ ) // левая граница
			PrintXY( 8, 3 + j, "\272", iCol, false );
		for ( j = 1; j < 19; j++ ) // правая граница
			PrintXY( 49, 3 + j, "\272", iCol, false );

		PrintXY( 8, 11, "\314",	iCol, false );
		for ( j = 1; j < 6; j++ )
			PrintXY( 8 + j, 11, "\315", iCol, false );
		PrintXY( 14, 11, " Цвета текста статусной линии ", iCol );
		for ( j = 1; j < 6; j++ )
			PrintXY( 43 + j, 11, "\315", iCol, false );
		PrintXY( 49, 11, "\271", iCol, false );

		PrintXY( 8, 16, "\314",	iCol, false );
		PrintXY( 49, 16, "\271", iCol, false );
		for ( j = 1; j < 41; j++ )
			PrintXY( 8 + j, 16, "\315", iCol, false );		

		PrintXY( 8, 22, "\310",	iCol, false );
		PrintXY( 49, 22, "\274", iCol, false );
		for ( j = 1; j < 41; j++ ) // нижняя граница
			PrintXY( 8 + j, 22, "\315", iCol, false );
	}
	if ( bWriteText ) {
		PrintXY( 9, 5, " Фон                                    ", 16 | 7 );
		PrintXY( 9, 6, " Нормальный текст                       ", 16 | 7 );
		PrintXY( 9, 7, " Жирный текст (тэг <b>)                 ", 16 | 7 );
		PrintXY( 9, 8, " Курсивный текст (тэг <i>)              ", 16 | 7 );
		PrintXY( 9, 9, " Подчеркнутый текст (тэг <u>)           ", 16 | 7 );

		PrintXY( 9, 13, " Фон                                    ", 16 | 7 );
		PrintXY( 9, 14, " Нормальный текст                       ", 16 | 7 );
/*		PrintXY( 10, 13, "Настроить цвет жирного текста статусной линии (тэг <b>)",
				iniFile.ConfigColors.Background | iniFile.ConfigColors.Bold );
		PrintXY( 10, 14, "Настроить цвет курсивного текста статусной линии (тэг <i>)",
				iniFile.ConfigColors.Background | iniFile.ConfigColors.Italics );
		PrintXY( 10, 15, "Настроить цвет подчеркнутого текста статусной линии (тэг <u>)",
				iniFile.ConfigColors.Background | iniFile.ConfigColors.Underscore );
*/

		PrintXY( 9, 18, " Установить цвета по умолчанию          ", 16 | 7 );
		PrintXY( 9, 19, " Сохранить цветовые настройки и выйти   ", 16 | 7 );
		PrintXY( 9, 20, " Выйти не сохранив цветовые настройки   ", 16 | 7 );
	}
}

int ColorsConfig_EnterSubmenu( int iEntry, int Colors[7], bool bColors = true ) {
	int i;
	int iPos; // позиция стрелок в меню
	char strSymbol[2];

	if ( bColors ) { // входим в подменю
		ColorsConfig_WriteMenu( true, false );
		for ( int j = 0; j < 20; j++ )
			PrintXY( 53, 3 + j, "           ", 16 | 15, false );
		// рисуем рамку вокруг цветов
		PrintXY( 53, 3, "\311",	16 | 15, false );
		for ( j = 1; j < 3; j++ )
			PrintXY( 53 + j, 3, "\315", 16 | 15, false );
		PrintXY( 56, 3, " Цвет ", 16 | 15 );
		for ( j = 1; j < 2; j++ )
			PrintXY( 61 + j, 3, "\315", 16 | 15, false );
		PrintXY( 63, 3, "\273",	16 | 15, false );

		for ( j = 1; j < 19; j++ ) // левая граница
			PrintXY( 53, 3 + j, "\272", 16 | 15, false );
		for ( j = 1; j < 19; j++ ) // правая граница
			PrintXY( 63, 3 + j, "\272", 16 | 15, false );

		PrintXY( 53, 22, "\310",	16 | 15, false );
		PrintXY( 63, 22, "\274", 16 | 15, false );
		for ( j = 1; j < 10; j++ )
			PrintXY( 53 + j, 22, "\315", 16 | 15, false );
		strSymbol[1] = 0;
		switch( iEntry ) {
		case 0:
			strSymbol[0] = ' ';
			iPos = Colors[0] / 16;
			break;
		case 1:
			strSymbol[0] = 'X';
			iPos = Colors[1];
			break;
		case 2:
			strSymbol[0] = 'X';
			iPos = Colors[2];
			break;
		case 3:
			strSymbol[0] = 'X';
			iPos = Colors[3];
			break;
		case 4:
			strSymbol[0] = 'X';
			iPos = Colors[4];
			break;
		case 8:
			strSymbol[0] = ' ';
			iPos = Colors[5] / 16;
			break;
		case 9:
			strSymbol[0] = 'X';
			iPos = Colors[6];
			break;
		}
		for ( i = 0; i < 16; i++ ) {
			if ( strSymbol[0] == ' ' )
				PrintXY( 58, 5 + i, strSymbol, 7 | i*16 );
			else {
				if ( iEntry == 9 )
					PrintXY( 58, 5 + i, strSymbol, i | Colors[5] );
				else
					PrintXY( 58, 5 + i, strSymbol, i | Colors[0] );
			}
		}
		PrintXY( 56, 5 + iPos, ">", 15 | 16 );
		PrintXY( 60, 5 + iPos, "<", 15 | 16 );
	} else { // выходим из подменю
		for ( int j = 0; j < 20; j++ )
			PrintXY( 53, 3 + j, "           ", iniFile.ConfigColors.Background
					| iniFile.ConfigColors.Normal, false );
		ColorsConfig_WriteMenu( true, true, false );
	}
	return iPos;
}

void ColorsConfig_Exit( CHAR_INFO chiTmp1[25 * 80], 
					    CONSOLE_SCREEN_BUFFER_INFO csbiTmp,
						CONSOLE_CURSOR_INFO cciCursor ) {
	COORD coordBuf;
	COORD coordBufInsertPoint;
	SMALL_RECT srcrSource;
	
	SetCurPos( csbiTmp.dwCursorPosition.X, csbiTmp.dwCursorPosition.Y );
	SetConsoleCursorInfo( hndlOutput, &cciCursor );

	coordBuf.X = 80;
	coordBuf.Y = 25;
	coordBufInsertPoint.X = 0;
	coordBufInsertPoint.Y = 0;
	srcrSource.Left = 0;
	srcrSource.Right = 79;
	srcrSource.Top = 0;
	srcrSource.Bottom = 24;

	WriteConsoleOutput( hndlOutput, chiTmp1, coordBuf, coordBufInsertPoint,
			&srcrSource );
}

void ColorsConfig( void ) {
	int iBgCol = iniFile.ConfigColors.Background;
	int iNormalCol = iniFile.ConfigColors.Normal;
	int iItalicsCol = iniFile.ConfigColors.Italics;
	int iBoldCol = iniFile.ConfigColors.Bold;
	int iUnderscoreCol = iniFile.ConfigColors.Underscore;
	int iInvertedCol = iniFile.ConfigColors.Inverted;
	int iStatBgCol = iniFile.ConfigColors.StatusLineBackground;
	int iStatNormalCol = iniFile.ConfigColors.StatusLineNormal;
	int iStatItalicsCol = iniFile.ConfigColors.StatusLineItalics;
	int iStatBoldCol = iniFile.ConfigColors.StatusLineBold;
	int iStatUnderscoreCol = iniFile.ConfigColors.StatusLineUnderscore;
	int iStatnvertedCol = iniFile.ConfigColors.StatusLineInverted;

	char strStatus[80];

	CHAR_INFO chiTmp1[25 * 80];
	COORD coordBuf;
	COORD coordBufInsertPoint;
	SMALL_RECT srcrSource;
	CONSOLE_SCREEN_BUFFER_INFO csbiTmp;
	CONSOLE_CURSOR_INFO cciCursor;

	GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
	GetConsoleCursorInfo( hndlOutput, &cciCursor );
	SetCursorVisibility( false ); // не уверен

	coordBuf.X = 80;
	coordBuf.Y = 25;
	coordBufInsertPoint.X = 0;
	coordBufInsertPoint.Y = 0;
	srcrSource.Left = 0;
	srcrSource.Right = 79;
	srcrSource.Top = 0;
	srcrSource.Bottom = 24;

	ReadConsoleOutput( hndlOutput, chiTmp1, coordBuf, coordBufInsertPoint,
			&srcrSource );

	for ( int i = 0; i < 25; i++ )
		PrintXY( 0, i, "                                                                                ",
				iniFile.ConfigColors.Background | iniFile.ConfigColors.Normal );

	CharToOem( " Настройка цветов Hydrofin", strStatus );
//	lstrcpy( strStatus, strTmp );
	StatusLineOut( 1, strStatus );
//	strStatus[81] = 't'; strStatus[82] = 'e'; strStatus[83] = 's'; 
//	strStatus[84] = 't'; strStatus[85] = 0;

	ColorsConfig_WriteMenu( true, true );
	PrintXY( 9, 5, " Фон                                    ", 112 | 1 );

	bool bMenu = true; // режим перемещения в меню/цветах
	bool bExit = false;
	int ch;
	int iPos = 0;
	int iSubPos;
	INPUT_RECORD irInBuf;
	DWORD dwNumChars;

	while( !bExit ) {
		ReadConsoleInput( hndlInput, &irInBuf, 1, &dwNumChars );
		if ( irInBuf.Event.KeyEvent.bKeyDown && 
				irInBuf.EventType == KEY_EVENT ) {

			int Colors[7] = { iBgCol, iNormalCol, iBoldCol, iItalicsCol,
						iUnderscoreCol, iStatBgCol, iStatNormalCol };
			ch = irInBuf.Event.KeyEvent.wVirtualKeyCode;
			if ( bMenu ) {
				switch( ch ) {
				case 9: //Tab
					if ( iPos != 13 && iPos != 14 && iPos != 15 ) {
						iSubPos = ColorsConfig_EnterSubmenu( iPos, Colors );
						bMenu = false;
					}
					break;
				case 13: //Enter
					switch ( iPos ) {
					case 13:
						iBgCol = 16;
						iNormalCol = 7;
						iItalicsCol = 14;
						iBoldCol = 15;
						iUnderscoreCol = 12;
						iInvertedCol = 0; // TODO
						iStatBgCol = 112;
						iStatNormalCol = 1;
						iStatItalicsCol = 14;
						iStatBoldCol = 14;
						iStatUnderscoreCol = 12;
						iStatnvertedCol = 0; // TODO
						break;
					case 14:
						iniFile.ConfigColors.Background = iBgCol;
						iniFile.ConfigColors.Normal = iNormalCol;
						iniFile.ConfigColors.Italics = iItalicsCol;
						iniFile.ConfigColors.Bold = iBoldCol;
						iniFile.ConfigColors.Underscore = iUnderscoreCol;
						iniFile.ConfigColors.Inverted = iInvertedCol;
						iniFile.ConfigColors.StatusLineBackground = iStatBgCol;
						iniFile.ConfigColors.StatusLineNormal = iStatNormalCol;
						iniFile.ConfigColors.StatusLineItalics = iStatItalicsCol;
						iniFile.ConfigColors.StatusLineBold = iStatBoldCol;
						iniFile.ConfigColors.StatusLineUnderscore = iStatUnderscoreCol;
						iniFile.ConfigColors.StatusLineInverted = iStatnvertedCol;
						bExit = true;
						break;
					case 15:
						bExit = true;
						break;
					default:
						iSubPos = ColorsConfig_EnterSubmenu( iPos, Colors );
						bMenu = false;
					}
					break;
				case 27: //Esc
					bExit = true;
					break;
				case 32: //Space
					if ( iPos != 13 && iPos != 14 && iPos != 15 ) {
						iSubPos = ColorsConfig_EnterSubmenu( iPos, Colors );
						bMenu = false;
					}
					break;
//				case 37: //Arrow Left
				case 39: //Arrow Right
					if ( iPos != 13 && iPos != 14 && iPos != 15 ) {
						iSubPos = ColorsConfig_EnterSubmenu( iPos, Colors );
						bMenu = false;
					}
					break;
				case 38: //Arrow Up
					if ( iPos > 0 )
						iPos--;
					else
						iPos = 15;
					if ( iPos == 7 )
						iPos = 4;
					if ( iPos == 12 )
						iPos = 9;
					break;
				case 40: //Arrow Down
					if ( iPos < 15 )
						iPos++;
					else
						iPos = 0;
					if ( iPos == 5 )
						iPos = 8;
					if ( iPos == 10 )
						iPos = 13;
					break;
				}
				if ( bExit )
					break;
				ColorsConfig_WriteMenu( false );
				switch( iPos ) {
				case 0:
					PrintXY( 9, 5, " Фон                                    ", 113 );
					break;
				case 1:
					PrintXY( 9, 6, " Нормальный текст                       ", 113 );
					break;
				case 2:
					PrintXY( 9, 7, " Жирный текст (тэг <b>)                 ", 113 );
					break;
				case 3:
					PrintXY( 9, 8, " Курсивный текст (тэг <i>)              ", 113 );
					break;
				case 4:
					PrintXY( 9, 9, " Подчеркнутый текст (тэг <u>)           ", 113 );
					break;
				case 8:
					PrintXY( 9, 13, " Фон                                    ", 113 );
					break;
				case 9:
					PrintXY( 9, 14, " Нормальный текст                       ", 113 );
					break;
				case 13:
					PrintXY( 9, 18, " Установить цвета по умолчанию          ", 113 );
					break;
				case 14:
					PrintXY( 9, 19, " Сохранить цветовые настройки и выйти   ", 113 );
					break;
				case 15:
					PrintXY( 9, 20, " Выйти не сохранив цветовые настройки   ", 113 );
					break;
				}
				if ( ch == '1' )
					break;
			} else { // Подменю
				PrintXY( 56, 5 + iSubPos, " ", 15 | 16 );
				PrintXY( 60, 5 + iSubPos, " ", 15 | 16 );
				switch( ch ) {
				case 9: //Tab
					ColorsConfig_EnterSubmenu( iPos, Colors, false );
					bMenu = true;
					break;
				case 13: //Enter
					ColorsConfig_EnterSubmenu( iPos, Colors, false );
					bMenu = true;
					break;
				case 27: //Esc
					ColorsConfig_EnterSubmenu( iPos, Colors, false );
					bMenu = true;
					break;
				case 32: //Space
					ColorsConfig_EnterSubmenu( iPos, Colors, false );
					bMenu = true;
					break;
				case 37: //Arrow Left
					ColorsConfig_EnterSubmenu( iPos, Colors, false );
					bMenu = true;
					break;
//				case 39: //Arrow Right
				case 38: //Arrow Up
					if ( iSubPos > 0 )
						iSubPos--;
					else
						iSubPos = 15;
					break;
				case 40: //Arrow Down
					if ( iSubPos < 15 )
						iSubPos++;
					else
						iSubPos = 0;
					break;
				}
//				int iTmp = 1 << iPos;
//				ColorsConfig_RefreshWriteScreen( iTmp, Colors ); // TODO: выводить динамическое изменение некоторых элементов экрана
				switch ( iPos ) {
				case 0:
					iBgCol = iSubPos * 16;
					break;
				case 1:
					iNormalCol = iSubPos;
					break;
				case 2:
					iBoldCol = iSubPos;
					break;
				case 3:
					iItalicsCol = iSubPos;
					break;
				case 4:
					iUnderscoreCol = iSubPos;
					break;
				case 8:
					iStatBgCol = iSubPos * 16;
					break;
				case 9:
					iStatNormalCol = iSubPos;
					break;
				}
				if ( !bMenu ) {
					PrintXY( 56, 5 + iSubPos, ">", 15 | 16 );
					PrintXY( 60, 5 + iSubPos, "<", 15 | 16 );
				}
			}
		}
//		char strTmp[5];	
//		wsprintf( strTmp, "key: %d, pos: %d", ch, iPos );
//		PrintXY( 0, 2, strTmp, 16 | 7 );
	}
	ColorsConfig_Exit( chiTmp1, csbiTmp, cciCursor );
}

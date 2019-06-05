#include "hydrofin.h"
//#include "hydrofin_input.h"
#include "stdafx.h"

#define SCRIPT_ALL				0 // �������� ��� � ����
#define SCRIPT_TEXT_ONLY		1 // �������� ������ ����� ����
#define SCRIPT_COMMANDS_ONLY	2 // �������� ������ ������� ������

extern HANDLE hndlInput;
extern HANDLE hndlOutput;
extern int iCurrentScroll;
extern int iOutputHistoryCount;
extern int iScrollDownCount;
extern int iScriptType;
extern bool bScripting;
extern bool bPrintingCommandHistory;
extern int iStatusNumLines;
extern void ExitGame( void ); // ��������� ����, ���������� ������ � ��.
extern void ColorsConfig( void );
extern FILE *hndlScriptFile; // ������� ����� ����������
extern void StartTranscript( int iTranscriptType );	// ������ ����������������� �������� ����
extern void EndTranscript( FILE *hndlFile ); // ��������� �����������������
extern char strRunGameName[MAX_PATH]; // ����������� ����


#define MAX_GAMES 2000 // ����������� � GameChestDlg.h, iniFile.cpp, Init.cpp, ColorsConfig.cpp

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

extern struct structIniFile iniFile;

extern void ScrollDown( bool iScrollDirectionDown, int iScrollLines, 
					   int iScrollStartLine, int iScrollEndLine );
extern void SetCurPos( int x, int y );
extern void SetCursorVisibility( bool bOnOff );
extern void SaveInOutputHistory( char *strString, char *Attributes );
extern void print( char *strPrint, int x, int y );
extern void Print( char *strPrint, int x, int y );
extern void Print( char *strPrint );
extern void print( char *strPrint );
extern void ScriptChar( char c );
extern int Error( char *strRrror );

char strInput[255];
bool bInsertMode = true; // ����� �������
int iInputCurPos = 0; // ������� ��������� ������� �� X �� ������ ����� ������������ ���������� �������
bool bCtrlPressed = false; // ������ ������ Ctrl
char *pInputString[64000]; // ��� �������� ���������� �� ������ ������� ������
int iHistoryCount = 0; // ��������� ��������� �� *pInputString[64000] ������� ����� ����������� ��� �������� ������� ������
int iHistoryBack = 0; // ��� ������� uparrow/downarrow �� ������� �������� � ������� ������ �� ���������
bool bPromptJustTyped = false;
bool ProcessKey( WORD wKey, char cKey, DWORD dwCntrlKeys, BOOL bPressed );

void SetCursorInsert( bool bInsert ) { // ���������� ������ ������� ���� � ��������� ������� (������� ������) ���� � ���������� �����
	CONSOLE_CURSOR_INFO cciCursor;

	GetConsoleCursorInfo( hndlOutput, &cciCursor );
	if ( bInsert ) {
		bInsertMode = false;
		cciCursor.dwSize = iniFile.ConfigCursor.InsertModeLines;
	} else {
		bInsertMode = true;
		cciCursor.dwSize = iniFile.ConfigCursor.OverwriteModeLines;
	}
	SetConsoleCursorInfo( hndlOutput, &cciCursor );
}

void BackToPrompt( void ) {
	if ( iScrollDownCount > 2 ) {
		ScrollDown( false, iScrollDownCount - 2, iStatusNumLines, 23 );
		SetCursorVisibility( true );
	}
}

void BackspacePressed( void ) {
	if ( iInputCurPos > 0 ) {

		CONSOLE_SCREEN_BUFFER_INFO csbiTmp;
		CHAR_INFO chiTmp;
		COORD coordTmp1, coordTmp2;
		SMALL_RECT srcrSource;
		
		chiTmp.Attributes = iniFile.ConfigColors.Background |
				iniFile.ConfigColors.Normal;
		coordTmp1.X = 1;
		coordTmp1.Y = 1;
		coordTmp2.X = 0;
		coordTmp2.Y = 0;
		GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
		srcrSource.Top = csbiTmp.dwCursorPosition.Y;
		srcrSource.Bottom = csbiTmp.dwCursorPosition.Y;
		
		for ( int i1 = iInputCurPos; i1 < lstrlen(strInput); i1++ ) {
			chiTmp.Char.AsciiChar = strInput[i1];
			strInput[i1 - 1] = strInput[i1];
			srcrSource.Left = csbiTmp.dwCursorPosition.X - 
					iInputCurPos + i1 - 1;
			srcrSource.Right = csbiTmp.dwCursorPosition.X - 
					iInputCurPos + i1 - 1;
			WriteConsoleOutput( hndlOutput, &chiTmp, coordTmp1, 
					coordTmp2, &srcrSource );
		}
		strInput[lstrlen(strInput) - 1] = 0;
		srcrSource.Left = csbiTmp.dwCursorPosition.X - iInputCurPos + 
				lstrlen(strInput);
		srcrSource.Right = csbiTmp.dwCursorPosition.X - iInputCurPos + 
				lstrlen(strInput);
		chiTmp.Char.AsciiChar = ' ';

		WriteConsoleOutput( hndlOutput, &chiTmp, coordTmp1, 
				coordTmp2, &srcrSource );
		SetCurPos( csbiTmp.dwCursorPosition.X - 1, 
				csbiTmp.dwCursorPosition.Y );
		iInputCurPos--;
	}
}

void DeletePressed( void ) {
	if ( iInputCurPos < lstrlen(strInput) ) {

		CONSOLE_SCREEN_BUFFER_INFO csbiTmp;
		CHAR_INFO chiTmp;
		COORD coordTmp1, coordTmp2;
		SMALL_RECT srcrSource;

		chiTmp.Attributes = iniFile.ConfigColors.Background |
				iniFile.ConfigColors.Normal;
		coordTmp1.X = 1;
		coordTmp1.Y = 1;
		coordTmp2.X = 0;
		coordTmp2.Y = 0;
		GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
		srcrSource.Top = csbiTmp.dwCursorPosition.Y;
		srcrSource.Bottom = csbiTmp.dwCursorPosition.Y;
		
		for ( int i1 = iInputCurPos; i1 + 1 < lstrlen(strInput); i1++ ) {
			chiTmp.Char.AsciiChar = strInput[i1 + 1];
			strInput[i1] = strInput[i1 + 1];
			srcrSource.Left = csbiTmp.dwCursorPosition.X - 
					iInputCurPos + i1;
			srcrSource.Right = csbiTmp.dwCursorPosition.X - 
					iInputCurPos + i1;
			WriteConsoleOutput( hndlOutput, &chiTmp, coordTmp1, 
					coordTmp2, &srcrSource );
		}
		strInput[lstrlen(strInput) - 1] = 0;
		srcrSource.Left = csbiTmp.dwCursorPosition.X - iInputCurPos + 
				lstrlen(strInput);
		srcrSource.Right = csbiTmp.dwCursorPosition.X - iInputCurPos + 
				lstrlen(strInput);
		chiTmp.Char.AsciiChar = ' ';

		WriteConsoleOutput( hndlOutput, &chiTmp, coordTmp1, 
				coordTmp2, &srcrSource );
	}
}

void EscPressed( void ) {
	if ( lstrlen(strInput) ) {

		CONSOLE_SCREEN_BUFFER_INFO csbiTmp;
		CHAR_INFO chiTmp[80];
		COORD coordTmp1, coordTmp2;
		SMALL_RECT srcrSource;

		for ( int i = 0 ; i < lstrlen(strInput) ; i++ ) {
			chiTmp[i].Attributes = iniFile.ConfigColors.Background |
					iniFile.ConfigColors.Normal;
			chiTmp[i].Char.AsciiChar = ' ';
		}
		coordTmp1.X = lstrlen(strInput);
		coordTmp1.Y = 1;
		coordTmp2.X = 0;
		coordTmp2.Y = 0;
		GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
		srcrSource.Top = csbiTmp.dwCursorPosition.Y;
		srcrSource.Bottom = csbiTmp.dwCursorPosition.Y;
		
		srcrSource.Left = csbiTmp.dwCursorPosition.X - iInputCurPos;
		srcrSource.Right = csbiTmp.dwCursorPosition.X - iInputCurPos +
				lstrlen(strInput);

		WriteConsoleOutput( hndlOutput, chiTmp, coordTmp1, 
				coordTmp2, &srcrSource );
		SetCurPos( csbiTmp.dwCursorPosition.X - iInputCurPos, 
				csbiTmp.dwCursorPosition.Y );
		iInputCurPos = 0;
		strInput[0] = 0;
	}
/*	int i3, iTmp; // ���� ������� ������ �������� � ������������� ������

	iTmp = iInputCurPos;
	for ( i3 = 0 ; i3 < iTmp ; i3++ )
		BackspacePressed();
	iTmp = lstrlen(strInput);
	for ( i3 = iInputCurPos ; i3 < iTmp ; i3++ )
		DeletePressed();
*/
}

void RestoreFromInputHistory( int iBack ) {
	EscPressed();
	if ( iBack == 0 )
		lstrcpy( strInput, "" );
	else
		lstrcpy( strInput, pInputString[iHistoryCount - iBack] );
	iInputCurPos = lstrlen( strInput );
	bPrintingCommandHistory = true;
	print( strInput );
	bPrintingCommandHistory = false;
}

void SaveInInputHistory( char *strCommand ) { // TODO: ��������� ���� ������ � ����������� �������� ������
	if ( iHistoryCount > 63999 )
		Error( "Fatal error: too long command history (maximum 64000 allowed)" );
	pInputString[iHistoryCount] = (char*)malloc( lstrlen(strCommand) + 1 );
	if ( pInputString == NULL )
		Error( "Fatal error: can't allocate memory to store command history" );
	lstrcpy( pInputString[iHistoryCount], strCommand );
	if ( bScripting && ( iScriptType == SCRIPT_ALL || 
			iScriptType == SCRIPT_COMMANDS_ONLY  ) ) {
		for ( int i = 0; i < lstrlen(strCommand); i++ )
			ScriptChar( strCommand[i] );
	}
	iHistoryCount++;
}

int ScrIn( char *strPromptIn ) {

	DWORD dwNumChars;
	INPUT_RECORD irInBuf;
	char AsciiChar;
	char strPrompt[255];
//	bool bHFlag; // ��� ������ � ������ ���� "�" (���������) � "h"
	CONSOLE_SCREEN_BUFFER_INFO csbiTmp;

	lstrcpy( strPrompt, strPromptIn );
	print( strPrompt );

	strInput[0] = 0;
	iCurrentScroll = 0;
	iInputCurPos = 0;
//	SetCursorInsert( !bInsertMode );
	SetCursorVisibility( true );
//	StopTranscript( hndlScriptFile ); // TODO: ��������� ����� ����������� ����������/���������� ����� ������������ ��� ��������� ����������

	while ( 1 ) {

/*		PeekConsoleInput( hndlInput, &irInBuf, 1, &dwNumChars );
		ReadConsole(hndlInput, &AsciiChar, 1, &dwNumChars, NULL);
		WriteConsoleInput( hndlInput, &irInBuf, 1, &dwNumChars );
*/
//		SetCursorVisibility( true );
		ReadConsoleInput( hndlInput, &irInBuf, 1, &dwNumChars );
		AsciiChar = irInBuf.Event.KeyEvent.uChar.AsciiChar;
//		bHFlag = false;
//		printf("%c%i", AsciiChar, irInBuf.Event.KeyEvent.wVirtualKeyCode );

//		PeekConsoleInput( hndlInput, &irInBuf, 1, &dwNumChars ); //�������� ������ ��� ����� �������� ����� � ������ "�"
		if ( irInBuf.EventType == KEY_EVENT ) { // keyboard input

/*			if (irInBuf.Event.KeyEvent.wVirtualKeyCode == 72 &&
					irInBuf.Event.KeyEvent.bKeyDown) {
//				ReadConsole(hndlInput, &AsciiChar, 1, &dwNumChars, NULL);
				FlushConsoleInputBuffer(hndlOutput); // �� ������, �� ��� ������������� ������ �������������� ���������� �� �������
				bHFlag = FALSE;
//				printf("|%c|", AsciiChar);
			} else {
				ReadConsoleInput( hndlInput, &irInBuf, 1, &dwNumChars );
				FlushConsoleInputBuffer(hndlOutput); // �� ������, �� ��� ������������� ������ �������������� ���������� �� �������
				AsciiChar = irInBuf.Event.KeyEvent.uChar.AsciiChar;
				bHFlag = TRUE;
			}
*/				
			if ( ProcessKey(irInBuf.Event.KeyEvent.wVirtualKeyCode, 
//					irInBuf.Event.KeyEvent.uChar.
					AsciiChar,
					irInBuf.Event.KeyEvent.dwControlKeyState,
					irInBuf.Event.KeyEvent.bKeyDown) )
				break;
		}
//		FlushConsoleInputBuffer(hndlOutput); // �� ������, �� ��� ������������� ������ �������������� ���������� �� �������
	}

//printf(" %s\n", strInput);

	SaveInInputHistory( strInput );
	iHistoryBack = 0;
	bPromptJustTyped = true;
	GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
	SetCurPos( csbiTmp.dwCursorPosition.X - iInputCurPos + 
			lstrlen(strInput), csbiTmp.dwCursorPosition.Y );
	print( "\n" );
//	SetCursorVisibility( false );

	return NULL;
}

bool ProcessKey(WORD wKey, char cKey, DWORD dwCntrlKeys, BOOL bPressed) { //, bool bHFlag) {

	WORD NormalKeys[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 
			 96, 97, 98, 99, 100, 101, 102, 103, 104, 105,
			 106, 191, 187, 189, 220, 107, 109, 81, 87, 69, 82, 84, 89, 85,
			 73, 79, 80, 219, 221, 65, 83, 68, 70, 71, 72, 74, 75, 76, 186,
			 222, 90, 88, 67, 86, 66, 78, 77, 188, 190, 32, 192, 110, 111};

	char str[2] = "";
	bool NormalKeyFlag = false;
		
	CONSOLE_SCREEN_BUFFER_INFO csbiTmp;
	CHAR_INFO chiTmp;
	COORD coordTmp1, coordTmp2;
	SMALL_RECT srcrSource;

	if ( !bPressed ) { // ����� ������� ������� ���� ��������
		switch ( wKey ) {
		case 17: // Ctrl
			bCtrlPressed = false;
			break;
		case 45: // Insert
			if ( bInsertMode )
				SetCursorInsert( true );
			else
				SetCursorInsert( false );
			break;
		default: // ��������� ������ ��� �� ����������
			break;
		}
		return false; // ���� ������ ������ ������������
	}
	switch ( wKey ) {
	case 8: //Backspace
		BackToPrompt();
		if ( dwCntrlKeys & RIGHT_CTRL_PRESSED || 
				dwCntrlKeys & LEFT_CTRL_PRESSED || bCtrlPressed ) {
			int i3, iTmp;

			iTmp = iInputCurPos;
			for ( i3 = 0 ; i3 < iTmp ; i3++ )
				BackspacePressed();
		} else
			BackspacePressed();
		break;
	case 9: //Tab
		break;
	case 13: //Enter
		CHAR_INFO chiTmp1[80];
		COORD coordBuf;
		COORD coordBufInsertPoint;
		
		char strTmp1[81];
		char strTmp2[81];
		int i1;

		BackToPrompt();

		GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
		coordBuf.X = 80;
		coordBuf.Y = 1;
		coordBufInsertPoint.X = 0;
		coordBufInsertPoint.Y = 0;
		srcrSource.Left = 0;
		srcrSource.Right = 79;
		srcrSource.Bottom = csbiTmp.dwCursorPosition.Y;
		srcrSource.Top = csbiTmp.dwCursorPosition.Y;

		ReadConsoleOutput( hndlOutput, chiTmp1, coordBuf, coordBufInsertPoint,
				&srcrSource );

		for ( i1 = 0 ; i1 < 80 ; i1++) {
			strTmp1[i1] = chiTmp1[i1].Char.AsciiChar;
			strTmp2[i1] = (unsigned char)chiTmp1[i1].Attributes;
		}
		strTmp1[80] = 0;
		strTmp2[80] = 0;

		SaveInOutputHistory( strTmp1, strTmp2 );

		return true; // ����� ����� ������
	case 17: // Ctrl
		bCtrlPressed = true; // ����� ������ ����� � ������ ��� ������������� ������� ������ � ��������� � Ctrl
		break;
	case 27: //Esc
		BackToPrompt();
		EscPressed();
		break;
	case 33: //Page Up
		GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
		if ( dwCntrlKeys & SHIFT_PRESSED ) {
			if (iOutputHistoryCount - iScrollDownCount + iStatusNumLines > 22 &&
					csbiTmp.dwCursorPosition.Y == 24) {
				SetCursorVisibility( false );
				ScrollDown( true, (24 - iStatusNumLines)/2, iStatusNumLines, 23 );
			}
		} else {
			if (iOutputHistoryCount - iScrollDownCount + iStatusNumLines > 22 &&
					csbiTmp.dwCursorPosition.Y == 24) {
				SetCursorVisibility( false );
				ScrollDown( true, 24 - iStatusNumLines, iStatusNumLines, 23 );
			}
		}
		break;
	case 34: //Page Down
		SetCursorVisibility( false );
		if ( dwCntrlKeys & SHIFT_PRESSED )
			ScrollDown( false, (24 - iStatusNumLines)/2, iStatusNumLines, 23 );
		else
			ScrollDown( false, 24 - iStatusNumLines, iStatusNumLines, 23 );
		break;
	case 35: //End
		BackToPrompt();

		if ( iInputCurPos < lstrlen(strInput) ) {
			GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
			SetCurPos( csbiTmp.dwCursorPosition.X - iInputCurPos + 
					lstrlen(strInput), csbiTmp.dwCursorPosition.Y );
			iInputCurPos = lstrlen(strInput);
		}
		break;
	case 36: //Home
		BackToPrompt();

		if ( iInputCurPos > 0 ) {
			GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
			SetCurPos( csbiTmp.dwCursorPosition.X - iInputCurPos, 
					csbiTmp.dwCursorPosition.Y );
			iInputCurPos = 0;
		}
		break;
	case 37: //Arrow Left
		BackToPrompt();

		if ( iInputCurPos > 0 ) {
			GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );

			if ( dwCntrlKeys & RIGHT_CTRL_PRESSED ||
					dwCntrlKeys & LEFT_CTRL_PRESSED || bCtrlPressed ) { // ������ ������� �� ����� � �����
				int i;
				bool bNonSpaceWas = false;
				for ( i = iInputCurPos - 1; i > 0; i-- ) {
					if ( strInput[i] != ' ' )
						bNonSpaceWas = true;
					if ( strInput[i - 1] == ' ' && bNonSpaceWas )
						break;
				}
				SetCurPos( csbiTmp.dwCursorPosition.X - iInputCurPos + i,
						csbiTmp.dwCursorPosition.Y );
				iInputCurPos = i;
			} else {
				SetCurPos( csbiTmp.dwCursorPosition.X - 1,
						csbiTmp.dwCursorPosition.Y );
				iInputCurPos--;
			}
		}
		break;
	case 38: //Arrow Up
		if ( dwCntrlKeys & SHIFT_PRESSED ) {
			GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
			if (iOutputHistoryCount - iScrollDownCount + iStatusNumLines > 22 &&
					csbiTmp.dwCursorPosition.Y == 24) {
				SetCursorVisibility( false );
				ScrollDown( true, 1, iStatusNumLines, 23 );
			} else
				break;
		} else {
			BackToPrompt();
			if ( iHistoryBack < iHistoryCount && iHistoryCount ) { // TODO: ����������� � ���������
				iHistoryBack++;
				RestoreFromInputHistory( iHistoryBack );
			}
		}
		break;
	case 39: //Arrow Right
		BackToPrompt();

		if ( iInputCurPos < lstrlen(strInput) ) {
			GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
			if ( dwCntrlKeys & RIGHT_CTRL_PRESSED ||
					dwCntrlKeys & LEFT_CTRL_PRESSED || bCtrlPressed ) { // ������ ������� �� ����� � �����
				int i;
				bool bNonSpaceWas = false;
				bool bSpaceWas = false;
				for ( i = iInputCurPos + 1; i < lstrlen(strInput); i++ ) {
					if ( strInput[i] != ' ' )
						bNonSpaceWas = true;
					if ( strInput[i] == ' ' )
						bSpaceWas = true;
					if ( strInput[i] != ' ' && bNonSpaceWas && bSpaceWas)
						break;
				}
				SetCurPos( csbiTmp.dwCursorPosition.X - iInputCurPos + i,
						csbiTmp.dwCursorPosition.Y );
				iInputCurPos = i;
			} else {
				SetCurPos( csbiTmp.dwCursorPosition.X + 1, 
						csbiTmp.dwCursorPosition.Y );
				iInputCurPos++;
			}
		}
		break;
	case 40: //Arrow Down
		if ( dwCntrlKeys & SHIFT_PRESSED ) {
			if ( iScrollDownCount > 2 ) {
				SetCursorVisibility( false );
				ScrollDown( false, 1, iStatusNumLines, 23 );
			} else
				break;
		} else {
			BackToPrompt();
			if ( iHistoryBack - 1 < iHistoryCount && iHistoryBack ) { // TODO: ����������� � ���������
				iHistoryBack--;
				RestoreFromInputHistory( iHistoryBack );
			}
		}
		break;
//	case 45: //Insert
//		break;
	case 46: //Delete
		BackToPrompt();

		if ( dwCntrlKeys & RIGHT_CTRL_PRESSED || 
				dwCntrlKeys & LEFT_CTRL_PRESSED || bCtrlPressed ) {
			int i3, iTmp;

			iTmp = lstrlen(strInput);
			for ( i3 = iInputCurPos ; i3 < iTmp ; i3++ )
				DeletePressed();
		} else
			DeletePressed();
		break;
	case 112: // F1
		char strTmp[1000];
		EscPressed();
		CharToOem( "\n������ Hydrofin:\n\n\
Alt-H  ������ Hydrofin\n\
Alt-T  ������/���������� ����������������\n\
Alt-X  ����� �� ����\n\
F1     ������ Hydrofin\n\
F9     ��������� ������\n\n\
����������� ����...\n",
				strTmp );
		print( strTmp );
		break;
	case 120: // F9
		ColorsConfig();
		break;
	default:
		if ( dwCntrlKeys & RIGHT_CTRL_PRESSED || 
				dwCntrlKeys & LEFT_CTRL_PRESSED || bCtrlPressed ) {
			// ���������� ������ ���� ��� ���� ������ ������������ � Ctrl
			if ( wKey == 17 ) // Ctrl
				bCtrlPressed = true; // ����� ������ ����� � ������ ��� ������������� ������� ������ � ��������� � Ctrl
			if ( wKey == 67 ) // Ctrl+C
				ExitGame();
			return false;
		} else if ( dwCntrlKeys & RIGHT_ALT_PRESSED ||
				dwCntrlKeys & LEFT_ALT_PRESSED ) {
			switch ( wKey ) {
			case 88: // X
				ExitGame();
				break;
			case 72: // H
				char strTmp[1000];
				EscPressed();
				CharToOem( "\n������ Hydrofin:\n\n\
Alt-H  ������ Hydrofin\n\
Alt-T  ������/���������� ����������������\n\
Alt-X  ����� �� ����\n\
F1     ������ Hydrofin\n\
F9     ��������� ������\n\n\
����������� ����...\n",
						strTmp );
				print( strTmp );
				break;
//			case 78: // N
//			case 80: // P
//			case 82: // R
			case 84: // T
				EscPressed();
				if ( bScripting ) {
					EndTranscript( hndlScriptFile );
					CharToOem( "\n����������������� ����������. ����������� \
����...\n", strTmp );
					print( strTmp );
				} else {
					char strTmp[20];
					print( "\n" );
					StartTranscript( SCRIPT_ALL );
					CharToOem( "����������� ����...\n", strTmp );
					print( strTmp );
					SetCursorVisibility( true );
				}
				break;
			}
		} else {
			for (int i = 0 ; i < sizeof(NormalKeys)/sizeof(WORD) ; i++)
				if ( wKey == NormalKeys[i] ) {
					BackToPrompt();
					NormalKeyFlag = true;
					break;
				}
			GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
			if ( NormalKeyFlag ) {
				if ( bInsertMode && csbiTmp.dwCursorPosition.X - 
						iInputCurPos + lstrlen(strInput) >= 79	||
						( !bInsertMode && csbiTmp.dwCursorPosition.X >= 79 ))
					break;
				if ( iniFile.ConfigInputOutput.CyrillicR == 1 && 
						wKey == 72 && cKey == 'h' ) // �������������� ������ ������� 'h' �� '�' (���������) (��� ������ ����� � WIn 98)
					wsprintf( str, "�" );
				else
					wsprintf( str, "%c", cKey );
/*					if (wKey == 72 && bHFlag && // �� ����� ������ ����� "h", "H" � "�" �����������, �� ������ ��� ���������� ���������
						( cKey == 'h' || cKey == 'H' || cKey == '�') ) 
					str[0] = 0;//'\0';
*/
				chiTmp.Attributes = iniFile.ConfigColors.Background |
						iniFile.ConfigColors.Normal;
				coordTmp1.X = 1;
				coordTmp1.Y = 1;
				coordTmp2.X = 0;
				coordTmp2.Y = 0;
				GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );
				srcrSource.Top = csbiTmp.dwCursorPosition.Y;
				srcrSource.Bottom = csbiTmp.dwCursorPosition.Y;
				strInput[lstrlen(strInput) + 1] = 0;

				if ( iInputCurPos < lstrlen(strInput) ) { // �.�. ����� ������� � �������� ������
					if ( !bInsertMode )
						DeletePressed();
					for ( int i2 = lstrlen(strInput); i2 > iInputCurPos; 
							i2-- ) {
						strInput[i2] = strInput[i2 - 1];
						srcrSource.Left = csbiTmp.dwCursorPosition.X - 
								iInputCurPos + i2;
						srcrSource.Right = csbiTmp.dwCursorPosition.X - 
								iInputCurPos + i2;
						chiTmp.Char.AsciiChar = strInput[i2];
						WriteConsoleOutput( hndlOutput, &chiTmp, 
								coordTmp1, coordTmp2, &srcrSource );
					}
					strInput[iInputCurPos] = str[0];
					chiTmp.Char.AsciiChar = str[0];
					srcrSource.Left = csbiTmp.dwCursorPosition.X;
					srcrSource.Right = csbiTmp.dwCursorPosition.X;
					WriteConsoleOutput( hndlOutput, &chiTmp, coordTmp1, 
							coordTmp2, &srcrSource );
				} else {
					strInput[lstrlen(strInput)] = str[0];
					chiTmp.Char.AsciiChar = str[0];
					srcrSource.Left = csbiTmp.dwCursorPosition.X;
					srcrSource.Right = csbiTmp.dwCursorPosition.X;
					WriteConsoleOutput( hndlOutput, &chiTmp, coordTmp1, 
							coordTmp2, &srcrSource );
				}


/*				srcrSource.Left = csbiTmp.dwCursorPosition.X - 
					iInputCurPos + lstrlen(strInput) - 1;
				srcrSource.Right = csbiTmp.dwCursorPosition.X - 
						iInputCurPos + lstrlen(strInput) - 1;
				chiTmp.Char.AsciiChar = strInput[lstrlen(strInput) - 1];
				WriteConsoleOutput( hndlOutput, &chiTmp, coordTmp1, 
						coordTmp2, &srcrSource );
*/
//				GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp );

				SetCurPos( csbiTmp.dwCursorPosition.X + 1,
						csbiTmp.dwCursorPosition.Y );
				iInputCurPos++;
			} else // ��������� ������ ��� �� ����������
				break;
		} // ����� ��������� ��� Ctrl �� ������
	} // Switch
//	printf( "Char: %c Scan code: %d\n", cKey, wKey );

	return false;
};


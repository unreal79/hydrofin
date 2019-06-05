// Hydrofin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HydrofinDlg.h"
#include "Hydrofin.h"

#include "Python20dev/include/Python.h"
#define PERR(bSuccess, api)
#define RETURN_NONE return (Py_INCREF(Py_None), Py_None);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

int ScrOut( char *strOut ) {
	if ( lstrlen(strOut) > 64000 )
		Error( "Fatal error: выводимая строка больше чем 64000 символов" );//output string too long (longer than 64000 chars)" );
	lstrcpy( strOutText, strOut ); // т.к. strOutText изменяется, а строки в Python меняться не должны
	SetCursorVisibility( false );
	print( strOutText );

	return NULL;
}

int StatusLineOut( int iNumLines = 0 ) {
	return StatusLineOut( 0, "[BUG]: you may not see that" );
}

int StatusLineOut( char *strStatusLine ) {
	return StatusLineOut( 1, strStatusLine );
}

int StatusLineOut( int iNumLines, char *strStatusLine ) {
	if ( iNumLines == 0 ) {
		if ( iStatusNumLines > iNumLines ) { // удаляем лишние статусные линии
			for ( int i = iNumLines; i < iStatusNumLines; i++ )
				ClearLine( i );	// TODO: сделать не просто очистку уже ненужных статусных линий, а восстановление из истории экранов
		}
		iStatusNumLines = iNumLines;
		return NULL;
	}

	CONSOLE_SCREEN_BUFFER_INFO csbiTmp;
	if ( !GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp ) ) {
		char strTmp[1000];
		wsprintf( strTmp, "Fatal error: невозможно получить информацию о буфере экрана" );//fail to get screen buffer info\n");
		CharToOem( strTmp, strTmp );
		printf( strTmp );
		exit( 1 );
	}
//	if ( csbiTmp.dwCursorPosition.Y <= iNumLines ) // TODO: Сделать перенос ввода не выводом новых линий. а	скроллированием экрана
//		ScrollDown( true, iNumLines - csbiTmp.dwCursorPosition.Y, iStatusNumLines, iNumLines );
	for ( int i = 0; i < iNumLines - csbiTmp.dwCursorPosition.Y ; i++ )
		print( "\n" );

	iStatusNumLines = iNumLines;

	CHAR_INFO chiStatusLine[80];
	COORD coordBuf;
	COORD coordBufInsertPoint;
	SMALL_RECT srcrSource;

	bool bZeroReached = false;

	coordBufInsertPoint.X = 0;
	coordBufInsertPoint.Y = 0;
	coordBuf.X = 80;
	coordBuf.Y = 1;
	srcrSource.Left = 0;
	srcrSource.Right = 79;

	for ( int j = 0; j < iNumLines; j++ ) {
		srcrSource.Top = j;
		srcrSource.Bottom = j;

//		strStatusLine[(j + 1)*80] = 0;

		for ( int i = 0; i < 80; i++ ) {
			
			if ( strStatusLine[j*80 + i + 1*(j > 0)] == 0 )
				bZeroReached = true;
			if ( bZeroReached )
				strStatusLine[j*80 + i + 1] = ' ';

			chiStatusLine[i].Char.AsciiChar = strStatusLine[j*80 + i + 1*(j > 0)];
			chiStatusLine[i].Attributes = iniFile.ConfigColors.StatusLineBackground |
					iniFile.ConfigColors.StatusLineNormal;
		}
		
		WriteConsoleOutput( hndlOutput, chiStatusLine, coordBuf, 
				coordBufInsertPoint, &srcrSource );
		bZeroReached = false;
	}
	return NULL;
}

void ParseOutputString( char *strOutput ) {

// TODO: сделать правильное распознавание вложенных тягов 
// типа: "<U>текст<I>-- еще </U>, все</I>"
	
	int j = 0;

	for ( int i1 = 0; i1 < sizeof(OutAttr); i1++)
		OutAttr[i1] = 0;

	for ( int i = 0; i < lstrlen(strOutput); i++) {
		if ( bNormalFlag )
			OutAttr[i] |= ATTR_NORMAL;
		if ( bBoldFlag )
			OutAttr[i] |= ATTR_BOLD;
		if ( bItalicFlag )
			OutAttr[i] |= ATTR_ITALIC;
		if ( bUnderscoreFlag )
			OutAttr[i] |= ATTR_UNDERSCORE;

		switch (strOutput[i]) {
		case ' ':
			OutAttr[i] |= ATTR_SPACE;
			break;
		case '\r': // CR
			strOutput[i] = '\n';
		case '\t': // Tab
			if ( iniFile.ConfigInputOutput.Tabs == 0 ) // Ignore
				OutAttr[i] |= ATTR_SKIP;
			else {
				strOutput[i] = ' ';
				OutAttr[i] |= ATTR_SPACE;
			}
			break;
		case '&':
			if ( (strOutput[i + 1] == 'n' || strOutput[i + 1] == 'N') &&
					(strOutput[i + 2] == 'b' || strOutput[i + 2] == 'B') &&
					(strOutput[i + 3] == 's' || strOutput[i + 3] == 'S') &&
					(strOutput[i + 4] == 'p' || strOutput[i + 4] == 'P') &&
					(strOutput[i + 5] == ';') ) { //&nbsp;
				strOutput[i] = ' ';
				OutAttr[i] |= ATTR_SPACE;
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				OutAttr[i + 3] |= ATTR_SKIP;
				OutAttr[i + 4] |= ATTR_SKIP;
				OutAttr[i + 5] |= ATTR_SKIP;
			}
			if ( (strOutput[i + 1] == 'l' || strOutput[i + 1] == 'L') &&
					(strOutput[i + 2] == 't' || strOutput[i + 2] == 'T') &&
					(strOutput[i + 3] == ';') ) { //&lt;
				strOutput[i] = '<';
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				OutAttr[i + 3] |= ATTR_SKIP;
			}
			if ( (strOutput[i + 1] == 'g' || strOutput[i + 1] == 'G') &&
					(strOutput[i + 2] == 't' || strOutput[i + 2] == 'T') &&
					(strOutput[i + 3] == ';') ) { //&gt;
				strOutput[i] = '>';
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				OutAttr[i + 3] |= ATTR_SKIP;
			}
			if ( (strOutput[i + 1] == 'a' || strOutput[i + 1] == 'A') &&
					(strOutput[i + 2] == 'm' || strOutput[i + 2] == 'M') &&
					(strOutput[i + 3] == 'p' || strOutput[i + 3] == 'P') &&
					(strOutput[i + 4] == ';') ) { //&amp;
				strOutput[i] = '&';
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				OutAttr[i + 3] |= ATTR_SKIP;
				OutAttr[i + 4] |= ATTR_SKIP;
			}
			if ( (strOutput[i + 1] == 'q' || strOutput[i + 1] == 'Q') &&
					(strOutput[i + 2] == 'u' || strOutput[i + 2] == 'U') &&
					(strOutput[i + 3] == 'o' || strOutput[i + 3] == 'O') &&
					(strOutput[i + 4] == 't' || strOutput[i + 4] == 'T') &&
					(strOutput[i + 5] == ';') ) { //&quot;
				strOutput[i] = (char)' \"';
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				OutAttr[i + 3] |= ATTR_SKIP;
				OutAttr[i + 4] |= ATTR_SKIP;
				OutAttr[i + 5] |= ATTR_SKIP;
			}
			break;
		case '<':
			////////////////// Bold ///////////////////
			if ( (strOutput[i + 1] == 'b' || strOutput[i + 1] == 'B') && 
					strOutput[i + 2] == '>' ) {
				OutAttr[i] |= ATTR_SKIP;
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				if ( bNormalFlag ) {
					bBoldFlag = true;
					bNormalFlag = false;
					PrevTag[j++] = ATTR_NORMAL;
				}
				if ( bItalicFlag ) {
					bBoldFlag = true;
					bItalicFlag = false;
					PrevTag[j++] = ATTR_ITALIC;
				}
				if ( bUnderscoreFlag ) {
					bBoldFlag = true;
					bUnderscoreFlag = false;
					PrevTag[j++] = ATTR_UNDERSCORE;
				}
				if ( j > 999 )
					Error( "Ощибка программирования Hydra: слишком велика вложенность тэгов (> 999)" );//"Programming error: too many tags.");
			}

			// Закрывающий тэг </b>
			if ( strOutput[i + 1] == '/' && (strOutput[i + 2] == 'b' || 
					strOutput[i + 2] == 'B') && strOutput[i + 3] == '>' ) {
				OutAttr[i] |= ATTR_SKIP;
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				OutAttr[i + 3] |= ATTR_SKIP;
				if ( PrevTag[j - 1] == ATTR_NORMAL ) {
					bNormalFlag = true;
					bBoldFlag = false;
					j--;
				}
				if ( PrevTag[j - 1] == ATTR_ITALIC ) {
					bItalicFlag = true;
					bBoldFlag = false;
					j--;
				}
				if ( PrevTag[j - 1] == ATTR_UNDERSCORE ) {
					bUnderscoreFlag = true;
					bBoldFlag = false;
					j--;
				}
				if ( j < 0 )
					Error( "Ощибка программирования Hydra: закрывающий тэг </b> без открывающего" );//"Programming error: too many tags.");
			}

			///////////////// Underscore ////////////////////
			if ( (strOutput[i + 1] == 'u' || strOutput[i + 1] == 'U') && 
					strOutput[i + 2] == '>' ) {
				OutAttr[i] |= ATTR_SKIP;
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				if ( bNormalFlag ) {
					bUnderscoreFlag = true;
					bNormalFlag = false;
					PrevTag[j++] = ATTR_NORMAL;
				}
				if ( bBoldFlag ) {
					bUnderscoreFlag = true;
					bBoldFlag = false;
					PrevTag[j++] = ATTR_BOLD;
				}
				if ( bItalicFlag ) {
					bUnderscoreFlag = true;
					bItalicFlag = false;
					PrevTag[j++] = ATTR_ITALIC;
				}
				if ( j > 999 )
					Error( "Ощибка программирования Hydra: слишком велика вложенность тэгов (> 999)" );
			}

			// Закрывающий тэг </u>
			if ( strOutput[i + 1] == '/' && (strOutput[i + 2] == 'u' || 
					strOutput[i + 2] == 'U') && strOutput[i + 3] == '>' ) {
				OutAttr[i] |= ATTR_SKIP;
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				OutAttr[i + 3] |= ATTR_SKIP;
				if ( PrevTag[j - 1] == ATTR_NORMAL ) {
					bNormalFlag = true;
					bUnderscoreFlag = false;
					j--;
				}
				if ( PrevTag[j - 1] == ATTR_BOLD ) {
					bBoldFlag = true;
					bUnderscoreFlag = false;
					j--;
				}
				if ( PrevTag[j - 1] == ATTR_ITALIC ) {
					bItalicFlag = true;
					bUnderscoreFlag = false;
					j--;
				}
				if ( j < 0 )
					Error( "Ощибка программирования Hydra: закрывающий тэг </u> без открывающего" );
			}

			///////////////// Тэг <i> ////////////////////
			if ( (strOutput[i + 1] == 'i' || strOutput[i + 1] == 'I') && 
					strOutput[i + 2] == '>' ) {
				OutAttr[i] |= ATTR_SKIP;
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				if ( bNormalFlag ) {
					bItalicFlag = true;
					bNormalFlag = false;
					PrevTag[j++] = ATTR_NORMAL;
				}
				if ( bBoldFlag ) {
					bItalicFlag = true;
					bBoldFlag = false;
					PrevTag[j++] = ATTR_BOLD;
				}
				if ( bUnderscoreFlag ) {
					bItalicFlag = true;
					bUnderscoreFlag = false;
					PrevTag[j++] = ATTR_UNDERSCORE;
				}
				if ( j > 999 )
					Error( "Ощибка программирования Hydra: слишком велика вложенность тэгов (> 999)" );
			}

			// Закрывающий тэг </i>
			if ( strOutput[i + 1] == '/' && (strOutput[i + 2] == 'i' || 
					strOutput[i + 2] == 'I') && strOutput[i + 3] == '>' ) {
				OutAttr[i] |= ATTR_SKIP;
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				OutAttr[i + 3] |= ATTR_SKIP;
				if ( PrevTag[j - 1] == ATTR_NORMAL ) {
					bNormalFlag = true;
					bItalicFlag = false;
					j--;
				}
				if ( PrevTag[j - 1] == ATTR_BOLD ) {
					bBoldFlag = true;
					bItalicFlag = false;
					j--;
				}
				if ( PrevTag[j - 1] == ATTR_UNDERSCORE ) {
					bUnderscoreFlag = true;
					bItalicFlag = false;
					j--;
				}
				if ( j < 0 )
					Error( "Ощибка программирования Hydra: закрывающий тэг </i> без открывающего" );
			}

			///////////////// Тэг <br> ////////////////////
			if ( (strOutput[i + 1] == 'b' || strOutput[i + 1] == 'B') && 
					(strOutput[i + 2] == 'r' || strOutput[i + 2] == 'R') &&
					strOutput[i + 3] == '>' ) {
				strOutput[i] = '\n';
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				OutAttr[i + 3] |= ATTR_SKIP;
			}
			///////////////// Тэг <p> ////////////////////
			if ( (strOutput[i + 1] == 'p' || strOutput[i + 1] == 'P') &&
					strOutput[i + 2] == '>' ) {
				strOutput[i] = '\n';
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
			}
			///////////////// Тэг </p> ////////////////////
			if ( strOutput[i + 1] == '/' &&
					(strOutput[i + 2] == 'p' || strOutput[i + 2] == 'P') &&
					strOutput[i + 3] == '>' && 
					iniFile.ConfigInputOutput.PTag == 1 ) {
				strOutput[i] = '\n';
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				OutAttr[i + 3] |= ATTR_SKIP;
			}
			///////////////// Тэг <hr> ////////////////////
			if ( (strOutput[i + 1] == 'h' || strOutput[i + 1] == 'H') &&
					(strOutput[i + 2] == 'r' || strOutput[i + 2] == 'R') &&
					strOutput[i + 3] == '>' ) {
				OutAttr[i] |= ATTR_SKIP;
				OutAttr[i + 1] |= ATTR_SKIP;
				OutAttr[i + 2] |= ATTR_SKIP;
				OutAttr[i + 3] |= ATTR_SKIP;
			}
			///////////////// Тэг <p align="center"> ////////////////////
			if ( (strOutput[i + 1] == 'p' || strOutput[i + 1] == 'P') &&
					strOutput[i + 2] == ' ' &&
					(strOutput[i + 3] == 'a' || strOutput[i + 3] == 'A') &&
					(strOutput[i + 4] == 'l' || strOutput[i + 4] == 'L') &&
					(strOutput[i + 5] == 'i' || strOutput[i + 5] == 'I') &&
					(strOutput[i + 6] == 'g' || strOutput[i + 6] == 'G') &&
					(strOutput[i + 7] == 'n' || strOutput[i + 7] == 'N') &&
					strOutput[i + 8] == '=' && strOutput[i + 9] == '\"' &&
					(strOutput[i + 10] == 'c' || strOutput[i + 10] == 'C') &&
					(strOutput[i + 11] == 'e' || strOutput[i + 11] == 'E') &&
					(strOutput[i + 12] == 'n' || strOutput[i + 12] == 'N') &&
					(strOutput[i + 13] == 't' || strOutput[i + 13] == 'T') &&
					(strOutput[i + 14] == 'e' || strOutput[i + 14] == 'E') &&
					(strOutput[i + 15] == 'r' || strOutput[i + 15] == 'R') &&
					strOutput[i + 16] == '\"' && strOutput[i + 17] == '>' ) {
				for ( int i1 = 0; i1 <= 17; i1++ )
					OutAttr[i + i1] |= ATTR_SKIP;
			}
		case '\n': // не уверен
			OutAttr[i] |= ATTR_SPACE;
			break;
		} // switch()
	}
}

void ClearLine( int iLineNum ) {
	DWORD dwTmp;
	COORD coordTmp;

//	SetCursorVisibility( false );
	coordTmp.X = 0;
	coordTmp.Y = iLineNum;
	FillConsoleOutputAttribute( hndlOutput, iniFile.ConfigColors.Background |
			iniFile.ConfigColors.Normal, 80, coordTmp, &dwTmp );
	FillConsoleOutputCharacter( hndlOutput, ' ', 80, coordTmp, &dwTmp );
}

int FindNextSpace( char *str, int iStart ) {
	int j = 0; // для учета пропускаемых символов

	for( int i = iStart; i < lstrlen(str); i++) {
		if ( OutAttr[i] & ATTR_SKIP )
			j++;
		else 
			if ( OutAttr[i] & ATTR_SPACE )
				return i - iStart - j;
	}
	return lstrlen(str) - iStart - j; // Пробелов не найдено
}

void More( void ) {
	bool bGoodProcessKey = false;

	DWORD dwNumChars;
	char AsciiChar;
	INPUT_RECORD irInBuf;

	CHAR_INFO chiTmp[80];
	COORD coordTmp1, coordTmp2;
	SMALL_RECT srcrSource;

//	SetCursorVisibility( false ); // не уверен

	for ( int i = 0; i < lstrlen( iniFile.ConfigMore.Text ) && i < 80; i++ ) {
		chiTmp[i].Char.AsciiChar = iniFile.ConfigMore.Text[i];
		chiTmp[i].Attributes = iniFile.ConfigColors.Background |
				iniFile.ConfigColors.Normal;
	}
	for ( i = lstrlen( iniFile.ConfigMore.Text ); i < 80; i++ ) {
		chiTmp[i].Char.AsciiChar = ' ';
		chiTmp[i].Attributes = iniFile.ConfigColors.Background |
				iniFile.ConfigColors.Normal;
	}
	coordTmp1.X = 80;
	coordTmp1.Y = 1;
	coordTmp2.X = 0;
	coordTmp2.Y = 0;
	srcrSource.Top = 24;
	srcrSource.Bottom = 24;
	srcrSource.Left = 0;
	srcrSource.Right = 79;

	WriteConsoleOutput( hndlOutput, chiTmp, coordTmp1, 
			coordTmp2, &srcrSource );

	while (1) {

//		FlushConsoleInputBuffer(hndlInput); // не работает
/*		GetNumberOfConsoleInputEvents( hndlOutput, &dwNumChars );
		for( int i1 = dwNumChars; i > 0; i-- ) {
			ReadConsoleInput( hndlInput, &irInBuf, 1, &dwNumChars );
		}
*/		
		WaitForSingleObject( hndlInput, INFINITE );
		FlushConsoleInputBuffer(hndlInput); // странно, но здесь работает
		ReadConsoleInput( hndlInput, &irInBuf, 1, &dwNumChars ); // не всегда ожидает нажатия клавиши без FlushConsoleInputBuffer
		AsciiChar = irInBuf.Event.KeyEvent.uChar.AsciiChar;
		if (irInBuf.EventType == KEY_EVENT) { // keyboard input
			switch ( AsciiChar ) {
				case 32: //Space
					iCurrentScroll = 0;
					bGoodProcessKey = true;
					break;
				case 13: // Enter
					iCurrentScroll--;
					bGoodProcessKey = true;
					break;
				default: // Остальные клавиши не действуют
					break;
			}
			if ( bGoodProcessKey )
				break;
		}
//		FlushConsoleInputBuffer(hndlInput);
	}

	for ( i = 0; i < 7; i++ ) // очищаем нижнюю строку
		chiTmp[i].Char.AsciiChar = ' ';
	WriteConsoleOutput( hndlOutput, chiTmp, coordTmp1, 
			coordTmp2, &srcrSource );

	SetCurPos(0, 24);
}

void ScrollUp( int iScrollLines, int iScrollStartLine, int iScrollEndLine ) {
	CHAR_INFO chiTmp[80];
	COORD coordBuf;
	COORD coordBufInsertPoint;
	SMALL_RECT srcrSource;

	coordBuf.X = 80;
	coordBuf.Y = 1;
	coordBufInsertPoint.X = 0;
	coordBufInsertPoint.Y = 0;
	srcrSource.Left = 0;
	srcrSource.Right = 79;

	for ( int j = 0; j < iScrollLines; j++) {
		for ( int i = iScrollStartLine; i <= iScrollEndLine; i++ ) {
			srcrSource.Top = i;
			srcrSource.Bottom = i;
			ReadConsoleOutput( hndlOutput, chiTmp, coordBuf, 
					coordBufInsertPoint, &srcrSource );
			srcrSource.Top = i - 1;
			srcrSource.Bottom = i - 1;
			WriteConsoleOutput( hndlOutput, chiTmp, coordBuf, 
					coordBufInsertPoint, &srcrSource );
		}
	}
	ClearLine( iScrollEndLine ); // не очень красиво, но работает -- очищаем нижнюю строку
}

void SaveInOutputHistory( char *strString, char *Attributes ) {
	if ( iOutputHistoryCount > 63999 )
		Error( "Fatal error: слишком длиная история экранов (максимум 64000 строк)" );
	pOutputStringText[iOutputHistoryCount] = (char*)malloc( lstrlen(strString) + 1 );
	if ( pOutputStringText == NULL )
		Error( "Fatal error: невозможно отвести память для хранения истории экранов (текст)" );
	lstrcpy( pOutputStringText[iOutputHistoryCount], strString );

	pOutputStringAttr[iOutputHistoryCount] = (char*)malloc( lstrlen(Attributes) + 1 );
	if ( pOutputStringAttr == NULL )
		Error( "Fatal error: невозможно отвести память для хранения истории экранов (атрибуты текста)" );

	lstrcpy( pOutputStringAttr[iOutputHistoryCount], Attributes );
	iOutputHistoryCount++;
}

void ScrollDown( bool bScrollDirectionDown, int iScrollLines, 
				int iScrollStartLine, int iScrollEndLine ) {
	CHAR_INFO chiTmp[80];
	COORD coordBuf;
	COORD coordBufInsertPoint;
	SMALL_RECT srcrSource;
	int i, iTmp;
	bool bReturnToPrompt = false;

	coordBuf.X = 80;
	coordBuf.Y = 1;
	coordBufInsertPoint.X = 0;
	coordBufInsertPoint.Y = 0;
	srcrSource.Left = 0;
	srcrSource.Right = 79;

	if ( iScrollDownCount == 2 && bScrollDirectionDown ) { // Сохраняем строку ввода
		CONSOLE_SCREEN_BUFFER_INFO csbiTmp;
		if ( !GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp ) ) {
			char strTmp[1000];
			wsprintf( strTmp, "Fatal error: невозможно получить информацию о буфере экрана" );//fail to get screen buffer info\n");
			CharToOem( strTmp, strTmp );
			printf( strTmp );
			exit( 1 );
		}
		srcrSource.Top = csbiTmp.dwCursorPosition.Y;//24; // TODO: не уверен
		srcrSource.Bottom = csbiTmp.dwCursorPosition.Y;//24;
		ReadConsoleOutput( hndlOutput, chiTmp, coordBuf, 
				coordBufInsertPoint, &srcrSource );
		for ( i = 0 ; i < 80 ; i++ ) {
			strPromptStringText[i] = chiTmp[i].Char.AsciiChar;
			strPromptStringAttr[i] = chiTmp[i].Attributes;
		}
		strPromptStringText[80] = 0;
		strPromptStringAttr[80] = 0;
	}
		
	for ( int j = 0 ; j < iScrollLines ; j++ ) { // Сдвигаем строки экрана вверх (вниз)
		if ( bScrollDirectionDown ) { // Scroll down
			if ( iOutputHistoryCount - iScrollDownCount + iStatusNumLines < 23 )
				return; // для обработки PageUp
			for ( i = iScrollEndLine ; i >= iScrollStartLine ; i-- ) // сдвигаем вниз
				if ( i < 24 ) {	// TODO: сделать все считывания не с экрана, а из истории экранов
					srcrSource.Top = i;
					srcrSource.Bottom = i;
					ReadConsoleOutput( hndlOutput, chiTmp, coordBuf, 
							coordBufInsertPoint, &srcrSource );
					srcrSource.Top = i + 1;
					srcrSource.Bottom = i + 1;
					WriteConsoleOutput( hndlOutput, chiTmp, coordBuf, 
							coordBufInsertPoint, &srcrSource );
				}
		} else { // Scroll up
			if ( iScrollDownCount < 3 ) { // назад к вводу команд
				SetCursorVisibility( true );
				return;
			}
			for ( i = iScrollStartLine + 1;
					i <= iScrollEndLine + 1 ; i++ ) { // сдвигаем вверх
				srcrSource.Top = i;
				srcrSource.Bottom = i;
				ReadConsoleOutput( hndlOutput, chiTmp, coordBuf, 
						coordBufInsertPoint, &srcrSource );
				srcrSource.Top = i - 1;
				srcrSource.Bottom = i - 1;
				WriteConsoleOutput( hndlOutput, chiTmp, coordBuf, 
						coordBufInsertPoint, &srcrSource );
			}
		}
		if ( bScrollDirectionDown )
			iTmp = iOutputHistoryCount - (iScrollEndLine - iScrollStartLine) -
					iScrollDownCount;
		else
			iTmp = iOutputHistoryCount - iScrollDownCount + 3;
		
		if ( !bScrollDirectionDown && iScrollDownCount == 3 ) { // возвращение к вводу командной строки
			for ( i = 0 ; i < 80 ; i++ ) {
				chiTmp[i].Char.AsciiChar = strPromptStringText[i];
				chiTmp[i].Attributes = (unsigned char)strPromptStringAttr[i];
			}
			bReturnToPrompt = true;
		} else
			for ( i = 0 ; i < 80 ; i++ ) {
				chiTmp[i].Char.AsciiChar = pOutputStringText[iTmp][i];
				chiTmp[i].Attributes = (unsigned char)pOutputStringAttr[iTmp][i];
			}

		if ( bScrollDirectionDown ) {
			srcrSource.Top = iScrollStartLine;
			srcrSource.Bottom = iScrollStartLine;
			iScrollDownCount++;
		} else {
			srcrSource.Top = iScrollEndLine + 1;
			srcrSource.Bottom = iScrollEndLine + 1;
			iScrollDownCount--;
		}
		
		WriteConsoleOutput( hndlOutput, chiTmp, coordBuf, 
				coordBufInsertPoint, &srcrSource );
	}
	if ( bReturnToPrompt )
		SetCursorVisibility( true );
}

void ProcessNewLine( int &ix, int &iy ) {
	ix = 0;
	iy++;
	if ( iy == 25 ) {
		ScrollUp(1, iStatusNumLines + 1, 24);
		iy--;
	}
	iCurrentScroll++; // для срабатывания More()
	if ( iCurrentScroll > 23 + iStatusNumLines )
		More();

	if ( bScripting && iScriptType == SCRIPT_ALL )
		ScriptChar( '\n' );
}

void Print( char *strPrint, int x, int y ) {
	bool bNextStringStartsWithSpace = false;
	bool bLongWord = false;
	int iSaveCount = x;

	CHAR_INFO chiTmp;
	COORD coordTmp1, coordTmp2;
	SMALL_RECT srcrSource;
	
	ParseOutputString( strPrint );

	for ( int i = 0; i < lstrlen(strPrint); i++ ) {
		if ( !(OutAttr[i] & ATTR_SKIP) ) {
			if ( strPrint[i] == '\n' ) { // New line
				chiTmp.Char.AsciiChar = ' ';
				chiTmp.Attributes = iniFile.ConfigColors.Background |
						iniFile.ConfigColors.Normal;
				coordTmp1.X = 1;
				coordTmp1.Y = 1;
				coordTmp2.X = 0;
				coordTmp2.Y = 0;
				srcrSource.Top = y;
				srcrSource.Bottom = y;
 				for ( int i1 = x; i1 < 80; i1++ ) { //добавляем пробелы до конца экранной строки
					srcrSource.Left = i1;
					srcrSource.Right = i1;

					WriteConsoleOutput( hndlOutput, &chiTmp, coordTmp1, 
							coordTmp2, &srcrSource );

					AttrSaveCurrent[iSaveCount] = chiTmp.Attributes;
					strSaveCurrent[iSaveCount] = ' ';
					iSaveCount++;
				}

				if ( bPromptJustTyped ) { // Для учета вывода новой строки (в ScrIn())
					COORD coordBuf, coordBufInsertPoint;

					coordBuf.X = 80;
					coordBuf.Y = 1;
					coordBufInsertPoint.X = 0;
					coordBufInsertPoint.Y = 0;
					srcrSource.Top = i;
					srcrSource.Bottom = i;
					for ( i1 = 0 ; i1 < x ; i1++ ) {
						ReadConsoleOutput( hndlOutput, &chiTmp, coordBuf, 
								coordBufInsertPoint, &srcrSource );
						AttrSaveCurrent[i1] = chiTmp.Attributes;
						strSaveCurrent[i1] = chiTmp.Char.AsciiChar;
					}
				}

				ProcessNewLine( x, y );
				strSaveCurrent[iSaveCount] = 0;
				AttrSaveCurrent[iSaveCount] = 0;
				if ( !bPromptJustTyped ) 
					SaveInOutputHistory( strSaveCurrent, AttrSaveCurrent );
				else
					bPromptJustTyped = false;
				iSaveCount = 0;
			} else { // New line
				int iNextSpacePos = FindNextSpace(strPrint, i);

				if ( iNextSpacePos == 1 && x == 79 ) // Чтобы не выводился первый пробел на следующей строке если текущая строка доходит до крайнего правого столбца (79)
					bNextStringStartsWithSpace = true;

				if ( iNextSpacePos == 1 )
					bLongWord = false;
				
				if ( x + iNextSpacePos > 80 ) { // следующее слово не влезает полностью на строку

					if ( x == 0 || x == 1 || bLongWord ) // на случай если выводимое слово превышает 80 символов
						bLongWord = true;
					else {

						chiTmp.Char.AsciiChar = ' ';
						chiTmp.Attributes = iniFile.ConfigColors.Background |
								iniFile.ConfigColors.Normal;
						coordTmp1.X = 1;
						coordTmp1.Y = 1;
						coordTmp2.X = 0;
						coordTmp2.Y = 0;
						srcrSource.Top = y;
						srcrSource.Bottom = y;
 						for ( int i1 = x; i1 < 80; i1++ ) { //добавляем пробелы до конца экранной строки
							srcrSource.Left = i1;
							srcrSource.Right = i1;

							WriteConsoleOutput( hndlOutput, &chiTmp, 
									coordTmp1, coordTmp2, &srcrSource );

							AttrSaveCurrent[iSaveCount] = chiTmp.Attributes;
							strSaveCurrent[iSaveCount] = ' ';
							iSaveCount++;
						}
						ProcessNewLine( x, y );
						strSaveCurrent[iSaveCount] = 0;
						AttrSaveCurrent[iSaveCount] = 0;
						SaveInOutputHistory( strSaveCurrent, AttrSaveCurrent );
						iSaveCount = 0;
					}
				} // следующее слово не влезает полностью на строку
				
				chiTmp.Char.AsciiChar = strPrint[i];
				if ( OutAttr[i] & ATTR_BOLD )
					chiTmp.Attributes = iniFile.ConfigColors.Background |
							iniFile.ConfigColors.Bold;
				if ( OutAttr[i] & ATTR_ITALIC )
					chiTmp.Attributes = iniFile.ConfigColors.Background | 
							iniFile.ConfigColors.Italics;
				if ( OutAttr[i] & ATTR_UNDERSCORE )
					chiTmp.Attributes = iniFile.ConfigColors.Background |
							iniFile.ConfigColors.Underscore;
				if ( OutAttr[i] & ATTR_NORMAL )
					chiTmp.Attributes = iniFile.ConfigColors.Background |
							iniFile.ConfigColors.Normal;
				coordTmp1.X = 1;
				coordTmp1.Y = 1;
				coordTmp2.X = 0;
				coordTmp2.Y = 0;
				srcrSource.Top = y;
				srcrSource.Bottom = y;
				srcrSource.Left = x;
				srcrSource.Right = x;

				WriteConsoleOutput( hndlOutput, &chiTmp, coordTmp1, 
						coordTmp2, &srcrSource );
				if ( bScripting && iScriptType == SCRIPT_ALL && 
						!bPrintingCommandHistory )
					ScriptChar( strPrint[i] );

				AttrSaveCurrent[iSaveCount] = chiTmp.Attributes;
				strSaveCurrent[iSaveCount] = chiTmp.Char.AsciiChar; // может быть  = strPrint[i] быстрее
				iSaveCount++;

//				printf( "%c", strPrint[i] );
				
				if ( bNextStringStartsWithSpace ) {
					i++;
					bNextStringStartsWithSpace = false;
				}
				
				x++;
			}
			
			if ( x == 80 ) {
				ProcessNewLine( x, y );
				strSaveCurrent[iSaveCount] = 0;
				AttrSaveCurrent[iSaveCount] = 0;
				SaveInOutputHistory( strSaveCurrent, AttrSaveCurrent );
				iSaveCount = 0;
			}
//				SetCurPos(x, y);
		} // & ATTR_SKIP

	}
	SetCurPos(x, y);
}

void print( char *strPrint, int x, int y) {
	Print( strPrint, x, y);
}

void Print( char *strPrint ) {
	print( strPrint );
}

void print( char *strPrint ) {
	CONSOLE_SCREEN_BUFFER_INFO csbiTmp;
	if ( !GetConsoleScreenBufferInfo( hndlOutput, &csbiTmp ) ) {
		char strTmp[1000];
		wsprintf( strTmp, "Fatal error: невозможно получить информацию о буфере экрана" );//fail to get screen buffer info\n");
		CharToOem( strTmp, strTmp );
		printf( strTmp );
		exit( 1 );
	}
	Print( strPrint, csbiTmp.dwCursorPosition.X, csbiTmp.dwCursorPosition.Y );
}

void SetCurPos( int x, int y ) {
	COORD coordCursorPos;

	coordCursorPos.X = x;
	coordCursorPos.Y = y;
	SetConsoleCursorPosition( hndlOutput, coordCursorPos );
}

void SetCursorVisibility( bool bOnOff ) { // установка признака видимости курсора
	CONSOLE_CURSOR_INFO cciCursor;

	GetConsoleCursorInfo( hndlOutput, &cciCursor );
	cciCursor.bVisible = bOnOff;
	SetConsoleCursorInfo( hndlOutput, &cciCursor );
}

void ScriptChar( char c ) {	// запись одного символа в файл стенограммы
	fputc( c, hndlScriptFile );
}

void StartTranscript( int iTranscriptType ) {
	iScriptType = iTranscriptType;
	char strTmp[MAX_PATH];
again:
	wsprintf( strTmp, "Введите имя файла (расширение \".scr\" будет добавлено).\n\
По умолчанию \"%s.scr\": ", strRunGameName );
	CharToOem( strTmp, strTmp );
//	wsprintf( strTmp, "Enter file name (\".scr\" will be added).\n\
//Default is \"%s.scr\": ", strTitle );
	ScrIn( strTmp );
	OemToAnsi( strInput, strTmp );
	if ( strTmp[0] == 0 )
		lstrcpy( strScriptFileName, strRunGameName );
	else
		lstrcpy( strScriptFileName, strTmp );
	lstrcat( strScriptFileName, ".scr" );
	if ( lstrlen(strScriptFileName) >= MAX_PATH ) {	// может быть ограничить до MAX_PATH?
		char strTmp[200];
		wsprintf( strTmp, "Слишком длинный путь к файлу стенограммы (максимум \
%d символов).\n", MAX_PATH );
		CharToOem( strTmp, strTmp );
		print( strTmp );
		goto again;
	}

	switch ( iTranscriptType ) {
	case SCRIPT_ALL: // выводить все
		if ( (hndlScriptFile = fopen(strScriptFileName, "r+t")) != NULL || 
				(hndlScriptFile = fopen(strScriptFileName, "w+t")) != NULL )
			fseek( hndlScriptFile, 0, SEEK_END );
		else {
			wsprintf( strTmp, "Ошибка: невозможно открыть файл \"%s\" для \
стенографирования", strScriptFileName );
			CharToOem( strTmp, strTmp );
//			wsprintf( strTmp, "Error: cannot open file \"%s\" for \
//transcripting\n", strScriptFileName );
			print( strTmp );
			goto again;
		}
		bScripting = true;
//		print( "Start of a transcript of\n" ); но нужно знать настоящее название игры
		break;
	case SCRIPT_TEXT_ONLY: // выводить только текст игры
		break;
	case SCRIPT_COMMANDS_ONLY: // выводить только команды игрока
		break;
	default:
		Error( "Фатальная ошибка в Hydrofin: попытка начать стенографирования \
неизвестного типа" );
//		Error( "Fatal error in Hydrofin: trying to start transcript of \
//unknown type" );
	}
}

void StopTranscript( FILE *hndlFile ) {
	if ( bScripting )
		fclose( hndlFile );
}

void EndTranscript( FILE *hndlFile ) {
	StopTranscript( hndlFile );
	bScripting = false;
}

void ContinueTranscript( char *strFileName ) {
	char strTmp[3000];
	if ((hndlScriptFile = fopen (strFileName, "r+t")) != NULL || 
			(hndlScriptFile = fopen (strFileName, "w+t")) != NULL) {
		fseek( hndlScriptFile, 0, SEEK_END );
	} else {
			wsprintf( strTmp, "Ошибка: невозможно открыть файл \"%s\" для \
стенографирования", strFileName );
//		wsprintf( strTmp, "Error: cannot open file \"%s\" for 
//transcripting\n",	strFileName );
		Error( strTmp );
	}
}

bool InvokeGameChestDialog( void ) {
	GameChestDlg m_dGameChestDlg;
	
	if ( m_dGameChestDlg.DoModal() == IDOK ) // нажата книпка "Запустить игру"
		return true;
	return false;
}

int Error( char *strError ) {
	char strTmp[1000]; // должно хватить

	CharToOem( strError, strTmp );
	lstrcat( strTmp, "\n" );
	print( strTmp );
	ExitGame();

	return NULL;
}

void PyErrCatching( char *str ) {
	FILE *hndlErrorLogFile;
	char strTmp[1000];

	CharToOem( "[Ошибка программирования Hydra: подробности смотрите в \
файле \"GameSession.log\"]\n", strTmp );
	print( strTmp );
	if ( ( hndlErrorLogFile = fopen("GameSession.log", "at")) != NULL ) {
		if ( !bPyErrorCatching ) { // файл открыт впервые за сессию
			time_t t;
			char strTmp1[1000];

			t = time(NULL);
			lstrcpy( strTmp1, ctime(&t) );
			lstrcpyn( strTmp, strTmp1, lstrlen( strTmp1 ) );
			wsprintf( strTmp1, "\n\n***** Сессия Hydrofin: %s *****\n", strTmp );
			CharToOem( strTmp1, strTmp );
			fwrite( strTmp, lstrlen(strTmp), 1, hndlErrorLogFile );
			bPyErrorCatching = true;
		}
		fwrite( str, lstrlen(str), 1, hndlErrorLogFile );
		fclose( hndlErrorLogFile );
	} else {
		CharToOem( "[Ошибка Hydrofin: невозможно открыть файл \
\"GameSession.log\"] для записи\n", strTmp );
		print( strTmp );
		print( "[Python output]: " );
		print( str );
	}
}

void ExitGame( void ) {

	BackToPrompt();
	EscPressed();
	print( "\n" );
	EndTranscript( hndlScriptFile );
	print( "\nEnd of session\n" );

	SaveIniFile();

	for ( int i = 0 ; i < iOutputHistoryCount ; i++ ) {
		free( pOutputStringText[i] );
		free( pOutputStringAttr[i] );
	}

	for ( i = 0 ; i < iHistoryCount ; i++ )
		free( pInputString[i] );

	for ( i = 1; i <= iniFile.GameChest.NumberOfGames; i++ )
		free( iniFile.GameChest.pGame[i - 1] );
	
	resizeConBufAndWindow( hndlOutput, scbiOldCosoleInfo.dwSize.X, 
			scbiOldCosoleInfo.dwSize.Y, false ); // false для предотвращения случая повторного освобождения памяти при ошибке востановления размера консоли

	exit( 0 );
}

void PrintXY( char x, char y, char *strSource, int color, bool bCharToOem = true ) {
	char str[255];
	CHAR_INFO chiSrcBuffer[255];
	COORD coordSrcBufferSize, coordSrcBufferCoord;
	SMALL_RECT srctDestRect;

	if ( bCharToOem )
		CharToOem( strSource, str );
	else
		lstrcpy( str, strSource );
	for ( int i = 0; i < lstrlen( str ); i++ ) {
		chiSrcBuffer[i].Char.AsciiChar = str[i];
		chiSrcBuffer[i].Attributes = color;
	}

	coordSrcBufferSize.X = lstrlen( str );
	coordSrcBufferSize.Y = 1;

	coordSrcBufferCoord.X = 0;
	coordSrcBufferCoord.Y = 0;

	srctDestRect.Top = y;
	srctDestRect.Bottom = y;
	srctDestRect.Left = x;
	srctDestRect.Right = x + lstrlen( str ) - 1;

	WriteConsoleOutput( hndlOutput, chiSrcBuffer, coordSrcBufferSize, 
			coordSrcBufferCoord, &srctDestRect );
}

///////////////////

CWinApp theApp;

using namespace std;

/////////////////// функции необходимые для интеграции Hydrofin с Hydra (Python)
static PyObject *Py_Hydrofin_ScrIn(PyObject *self, PyObject *args) {
    static char *str;
	
	if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;
	ScrIn( str );
	return Py_BuildValue("s", strInput);
}

static PyObject *Py_Hydrofin_ScrOut(PyObject *self, PyObject *args) {
    static char *str;
	
	if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;
	ScrOut( str );
	RETURN_NONE
}

static PyObject *Py_Hydrofin_StatusLineOut(PyObject *self, PyObject *args) {
	static char *str;

	if (!PyArg_ParseTuple(args, "s", &str))
		return NULL;
	StatusLineOut( 1, str );
	RETURN_NONE
}

// может нужно, а может и нет
static PyObject *Py_Hydrofin_ExitGame(PyObject *self, PyObject *args) {
//    static char *str;
	
//	if (!PyArg_ParseTuple(args, "s", &str))
//		return NULL;
	ExitGame();
	RETURN_NONE
} 

static PyObject *Py_Hydrofin_StartTranscript( PyObject *self, PyObject *args ) {
	static char *str;
	int iType;
	
	if ( !PyArg_ParseTuple( args, "i", &iType ) )
        return NULL;
	StartTranscript( iType );
	RETURN_NONE
}

static PyObject *Py_Hydrofin_PyErrCatching( PyObject *self, PyObject *args ) {
	char *str;
	
	if ( !PyArg_ParseTuple( args, "s", &str ) )
        return NULL;
	PyErrCatching( str );
	RETURN_NONE
}

static PyMethodDef Py_HydrofinMethods[] = {
	{"ScrIn",			Py_Hydrofin_ScrIn,				METH_VARARGS},
	{"ScrOut",			Py_Hydrofin_ScrOut,				METH_VARARGS},
	{"StatusLineOut",	Py_Hydrofin_StatusLineOut,		METH_VARARGS},
	{"StartTranscript",	Py_Hydrofin_StartTranscript,	METH_VARARGS},
	{"ExitGame",		Py_Hydrofin_ExitGame,			METH_VARARGS}, // может нужно, а может и нет
	{"PyErrCatching",	Py_Hydrofin_PyErrCatching,		METH_VARARGS},
	{NULL,			NULL} /* Sentinel */
};

void Py_InitHydrofin() {
	PyObject* module;
    module = Py_InitModule( "Hydrofin", Py_HydrofinMethods );
}

/////////////////

int _tmain(int argc, TCHAR *argv[], TCHAR *envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
		return nRetCode;
	}

	GetWindowsVersion();

	SetHydrofinPath( argv[0] );

	OpenIniFile();

//	HydrofinDlgHydrofinConfig();
	if ( argc < 2 ) {
		if ( InvokeGameChestDialog() )
			SetCurrentDirectory( strHydrofinPath );
		else
			exit( 0 ); //пользователь не выбрал файла игры
	} else {
		SetCurrentDirectory( strHydrofinPath );
		GetFileTitle( argv[1], strRunGameName, MAX_PATH );
//		lstrcpy( strRunGameName, argv[1] );
	}

/*
	CFileDialog m_ldFile( TRUE );
	CString sPathName, sFileName, sFilePath, sFileTitle;
	char strOpenFile[3000];
	char strTmp1[3000];
	char strTmp2[3000];

	if ( m_ldFile.DoModal() == IDOK ) {
		if ( m_ldFile.GetPathName().GetLength() > 3000 ) {
			MessageBox( NULL, "Too long file path (>3000 symbols).\n\
Terminating.", "Fatal Error", MB_OK );
			exit( 1 );
		}
		
		sPathName = m_ldFile.GetPathName();
		sFileName = m_ldFile.GetFileName();

//		for ( int i = 0 ; i < sPathName.GetLength() + sFileName.GetLength();
//				i++ )	{
//			if ( i < sPathName.GetLength() )
//				strTmp1[i] = sPathName.GetAt( i );
//			else
//				strTmp1[i] = sFileName.GetAt( i - sPathName.GetLength() );
//		}
//		strTmp1[i] = 0;

//		wsprintf( strTmp1, "%s%s", sPathName, sFileName );

//		wsprintf( strTmp1, "D:\\\\Stas\\\\PRIVATE\\\\IF\\\\RUSSIAN\\\\HYDRA\\\\HydraPublicBeta04\\\\" );
		strTmp1[ m_ldFile.GetPathName().GetLength() - 
				m_ldFile.GetFileName().GetLength() ] = 0; // обрезаем имя файла
		sFileTitle = m_ldFile.GetFileTitle();
		wsprintf( strTmp2, "%s", sFileTitle );
		lstrcat( strTmp1, strTmp2 );
		lstrcpy( strOpenFile, strTmp1 );

//		sFilePath = m_ldFile.GetFolderPath(); // приводит к ошибке Windows
//		sFileTitle = m_ldFile.GetFileTitle();// GetPathName();//  GetFileName();
//		lstrcpy( sFilePath. , sFileTitle );
//		wsprintf( strOpenFile, "%s%s", sFilePath, sFileTitle );

//		int j = 0;
//		for ( int i = 0 ; i < sFilePath.GetLength() ; i++ )	{
//			if ( sFilePath.GetAt( i ) == '\\' 
//					&& sFilePath.GetAt( i + 1 ) == '\\' );
//			else
//				strOpenFile[j++] = sGilePath.GetAt( i );
//		}
//		strOpenFile[j] = 0;
//
	} else
		exit( 0 );
*/

	Init( strRunGameName );

	Py_SetProgramName( argv[0] );
	Py_Initialize();

	Py_InitHydrofin();

	PyRun_SimpleString( "from hydrofinstart import *" );
	
	char strTmp3[MAX_PATH];
	char strTmp4[MAX_PATH * 2];
	
	int iInd = 0;
	for ( int i = 0; i < lstrlen(strRunGamePath); i++, iInd++ ) { // Замена всех "\" в пути файла на "\\"
		if ( strRunGamePath[i] == '\\' ) {
			strTmp4[iInd] = '\\';
			iInd++;
			strTmp4[iInd] = '\\';
		} else
			strTmp4[iInd] = strRunGamePath[i];
	}
	strTmp4[iInd] = 0;
	// TODO: несколько типов входных "файлов": "File", "Dir", "Zip"
	wsprintf( strTmp3, "LoadGame( '%s', '%s', 'File' )", strRunGameName,
			strTmp4 );
	PyRun_SimpleString( strTmp3 );
//	StartTranscript( SCRIPT_ALL, strRunGameName );
//	char ss[81 + 2*80] = " status line -- test";
//	ss[81] = 't'; ss[82] = 'e'; ss[83] = 's'; ss[84] = 't';	ss[85] = 0;
//	ss[161] = '0'; ss[162] = '1'; ss[163] = '2'; ss[164] = '3';	ss[165] = 0;
//	StatusLineOut( 2, ss );
	PyRun_SimpleString( "main()" );

	ExitGame();

	return nRetCode;
}


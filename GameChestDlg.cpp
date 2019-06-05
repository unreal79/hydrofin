// GameChestDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "Hydrofin.h"
#include "GameChestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GameChestDlg dialog


GameChestDlg::GameChestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GameChestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(GameChestDlg)
	m_strGameComment = _T("");
	//}}AFX_DATA_INIT
	iItemsSelectedCount = 0;
//	iSelectedItem[1] = 0;
	m_strGameComment = "Игра не выбрана";
	m_bHeaderEnabled = false;
	bCalledFromKeyDown = false;
	iLastClicked = 0;
	for ( int i = 0; i < 32000; i++ )
		iSelectedItem[i] = 0;
//	UpdateData( FALSE ); // приводит к фатальной ошибке
}

void GameChestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GameChestDlg)
	DDX_Control(pDX, IDC_REMOVEGAME, m_cRemoveGame);
	DDX_Control(pDX, IDOK, m_cRunGame);
	DDX_Control(pDX, IDC_GAMELIST, m_cGameList);
	DDX_Text(pDX, IDC_GAMECOMMENT, m_strGameComment);
	//}}AFX_DATA_MAP

	if ( !m_bHeaderEnabled ) {
		m_bHeaderEnabled = true;
		LV_COLUMN pcol;
		pcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		pcol.fmt = LVCFMT_LEFT;
		pcol.cx = iniFile.GameChest.ColumnWidth1;
		pcol.pszText = "Название";
//		pcol.iSubItem = 0;
		pcol.cchTextMax = lstrlen( pcol.pszText );
		int i = ListView_InsertColumn( ::GetDlgItem( m_hWnd, IDC_GAMELIST ), 0, &pcol );
		pcol.cx = iniFile.GameChest.ColumnWidth2;
		pcol.pszText = "Имя файла";
		ListView_InsertColumn( ::GetDlgItem( m_hWnd, IDC_GAMELIST ), 1, &pcol );
		pcol.cx = iniFile.GameChest.ColumnWidth3;
		pcol.pszText = "Тип";
		ListView_InsertColumn( ::GetDlgItem( m_hWnd, IDC_GAMELIST ), 2, &pcol );
		for ( i = 0; i < iniFile.GameChest.NumberOfGames; i++ ) { // идет распознание текста из .ini файла
			// Пример данных: 'test","c:\tmp\test.py","c:\tmp\","File' (первая и последняя кавычки отбрасываются при чтении из ini файла)
			char str[MAX_PATH * 3];
			char str1[4][MAX_PATH];
			LVITEM pItem;
			MYITEM *pIt = (MYITEM *)LocalAlloc(LPTR, sizeof(MYITEM)); 

			lstrcpy( str, iniFile.GameChest.pGame[i] );
			for ( int j = 0, iCount = 0, iQuote = 0; j < lstrlen( str ); j++ ) {
				if ( str[j] == '\"' ) {
					str1[iQuote - 1][iCount] = 0;
					if ( !( str1[iQuote - 1][0] == ',' && str1[iQuote - 1][1] == 0 ) )
						iQuote++;
					iCount = 0;
				} else {
/*					if ( iQuote == 0 ) {
						MessageBox( "Ошибка в \"Hydrofin.ini\".",
								"Ошибка в \"Hydrofin.ini\"" );
						RemoveGameFromList( i );
						j--;
						continue;
					}*/
					str1[iQuote - 1][iCount] = str[j];
					iCount++;
				}
			}
			str1[iQuote - 1][iCount] = 0;
//m_cGameList.GetItemCount()
			ListGameName[ i ] = (char *)malloc( lstrlen( str1[0] ) + 1 );
			if ( ListGameName[ i ] == NULL )
				Error( "Фатальная ошибка: Невозможно отвести помять для \
хранения информации \"Сундучка игр\"" );
			lstrcpy( ListGameName[ i ], str1[0] );
			pIt->aCols[0] = DupString( str1[0] );
			
			ListText[ i ] = (char *)malloc( lstrlen( str1[1] ) + 1 );
			if ( ListText[ i ] == NULL )
				Error( "Фатальная ошибка: Невозможно отвести помять для \
хранения информации \"Сундучка игр\"" );
			lstrcpy( ListText[ i ], str1[1] );
			pIt->aCols[1] = DupString( str1[1] );

			ListGamePath[ i ] = (char *)malloc( lstrlen( str1[2] ) + 1 );
			if ( ListGamePath[ i ] == NULL )
				Error( "Фатальная ошибка: Невозможно отвести помять для \
хранения информации \"Сундучка игр\"" );
			lstrcpy( ListGamePath[ i ], str1[2] );

			if ( !lstrcmp( str1[3], "File" )	)
				lstrcpy( str, "Файл" );
			else if ( !lstrcmp( str1[3], "Dir" )	)
				lstrcpy( str, "Директория" );
			else if ( !lstrcmp( str1[3], "Zip" )	)
				lstrcpy( str, "Архив Zip" );
			ListGameType[ i ] = (char *)malloc( lstrlen( str ) + 1 );
			if ( ListGameType[ i ] == NULL )
				Error( "Фатальная ошибка: Невозможно отвести помять для \
хранения информации \"Сундучка игр\"" );
			lstrcpy( ListGameType[ i ], str );
			pIt->aCols[2] = DupString( str );

			pItem.mask = LVIF_PARAM | LVIF_STATE | LVIF_TEXT;
			pItem.state = 0; 
			pItem.stateMask = 0;
//			pItem.iImage = 0;
			pItem.pszText = LPSTR_TEXTCALLBACK;
			pItem.iItem = m_cGameList.GetItemCount();
			pItem.iSubItem = 0;
			pItem.lParam = (LPARAM) pIt;
			m_cGameList.InsertItem( &pItem );
		}
	}
}


BEGIN_MESSAGE_MAP(GameChestDlg, CDialog)
	//{{AFX_MSG_MAP(GameChestDlg)
	ON_BN_CLICKED(IDC_ADDGAMEFILE, OnAddgamefile)
	ON_NOTIFY(NM_CLICK, IDC_GAMELIST, OnClickGamelist)
	ON_NOTIFY(NM_RETURN, IDC_GAMELIST, OnReturnGamelist)
	ON_NOTIFY(LVN_KEYDOWN, IDC_GAMELIST, OnKeydownGamelist)
	ON_NOTIFY(NM_DBLCLK, IDC_GAMELIST, OnDblclkGamelist)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_GAMELIST, OnGetdispinfoGamelist)
	ON_BN_CLICKED(IDC_REMOVEGAME, OnRemovegame)
	ON_NOTIFY(NM_RCLICK, IDC_GAMELIST, OnRclickGamelist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GAMELIST, OnItemchangedGamelist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GameChestDlg message handlers

// DupString - allocates a copy of a string. 
// lpsz - address of the null-terminated string to copy 
LPSTR GameChestDlg::DupString( LPSTR lpsz ) {
	int cb = lstrlen(lpsz) + 1;
	LPSTR lpszNew = (char *)LocalAlloc( LMEM_FIXED, cb );
	if (lpszNew != NULL)
		CopyMemory( lpszNew, lpsz, cb );
	return lpszNew;
}

void GameChestDlg::OnAddgamefile() 
{
	CFileDialog m_ldFile(TRUE);
	LVITEM pItem;
	MYITEM *pIt = (MYITEM *)LocalAlloc(LPTR, sizeof(MYITEM)); 

	m_ldFile.m_ofn.lpstrInitialDir = iniFile.GameChest.DefaultGamesDirectory;//strHydrofinPath; // TODO: изменить	на iniFile.GameChest.DefaultGamesDirectory

	CString allFilter;
	CString strFilter;

	strFilter += "Python Files (*.py)";
	strFilter += '\0';
	strFilter += "*.py";
	strFilter += '\0';
	VERIFY( allFilter.LoadString(AFX_IDS_ALLFILTER) );
	strFilter += allFilter;
	strFilter += '\0';
	strFilter += "*.*";
	strFilter += '\0';
//? m_ldFile.m_ofn.nMaxCustFilter++;
	m_ldFile.m_ofn.Flags |= OFN_EXPLORER | OFN_ENABLESIZING;
			//| OFN_ALLOWMULTISELECT;// TODO: не знаю как
	m_ldFile.m_ofn.lpstrFilter = strFilter;

	if (m_ldFile.DoModal() == IDOK) {
		char str[MAX_PATH];
//		POSITION posTmp;
//		posTmp = (POSITION) NULL;
//		lstrcpy( str, m_ldFile.GetPathName() );
//		posTmp = (POSITION)str;
//		lstrcpy( str, m_ldFile.GetNextPathName( posTmp ) );//.GetPathName() );
//		PSTR pszStart; 
//		PSTR pszEnd; 
//		int iSubItem; 
//		char g_achTemp[256];

		pItem.mask = LVIF_PARAM | LVIF_STATE | LVIF_TEXT;
		pItem.state = 0; 
		pItem.stateMask = 0;
//		pItem.iImage = 0;
		pItem.pszText = LPSTR_TEXTCALLBACK;
/*		lstrcpy( g_achTemp, "first1;second2;third3;another4;5" );
		pszEnd = strchr(g_achTemp, ';'); 
		*pszEnd = '\0'; 
		pIt->aCols[0] = DupString( g_achTemp ); 
		for (iSubItem = 1; iSubItem < 3 && pszEnd != NULL; iSubItem++) { 
			pszStart = pszEnd + 1; 
			if ((pszEnd = strchr(pszStart, ';')) != NULL) 
				*pszEnd = '\0'; 
			pIt->aCols[iSubItem] = DupString(pszStart); 
		} */

		char str1[MAX_PATH];
/*		POSITION posTmp = m_ldFile.GetStartPosition();
		lstrcpy( str, m_ldFile.GetNextPathName( posTmp ) );
		lstrcpy( str, m_ldFile.GetNextPathName( posTmp ) );
		lstrcpy( str, m_ldFile.GetNextPathName( posTmp ) );
		lstrcpy( str, m_ldFile.GetNextPathName( posTmp ) );
*/
		lstrcpy( str, m_ldFile.GetPathName() );
		for ( int j = 0; j < lstrlen(m_ldFile.GetPathName()) -
				lstrlen( m_ldFile.GetFileName() ); j++ )
			str1[j] = str[j];
		str1[j] = 0;
		ListGamePath[ m_cGameList.GetItemCount() ] = (char *)malloc( lstrlen( str1 ) + 1 );
		if ( ListGamePath[ m_cGameList.GetItemCount() ] == NULL )
			Error( "Фатальная ошибка: Невозможно отвести помять для \
хранения информации \"Сундучка игр\"" );
		lstrcpy( ListGamePath[ m_cGameList.GetItemCount() ], str1 );

		lstrcpy( str, m_ldFile.GetFileTitle() );
		ListGameName[ m_cGameList.GetItemCount() ] = (char *)malloc( lstrlen( str ) + 1 );
		if ( ListGameName[ m_cGameList.GetItemCount() ] == NULL )
			Error( "Фатальная ошибка: Невозможно отвести помять для \
хранения информации \"Сундучка игр\"" );
		lstrcpy( ListGameName[ m_cGameList.GetItemCount() ], str );
		pIt->aCols[0] = DupString( str );

		lstrcpy( str, m_ldFile.GetPathName() );
		ListText[ m_cGameList.GetItemCount() ] = (char *)malloc( lstrlen( str ) + 1 );
		if ( ListText[ m_cGameList.GetItemCount() ] == NULL )
			Error( "Фатальная ошибка: Невозможно отвести помять для \
хранения информации \"Сундучка игр\"" );
		lstrcpy( ListText[ m_cGameList.GetItemCount() ], str );
		pIt->aCols[1] = DupString( str );

		lstrcpy( str, "Файл" );
		ListGameType[ m_cGameList.GetItemCount() ] = (char *)malloc( lstrlen( str ) + 1 );
		if ( ListGameType[ m_cGameList.GetItemCount() ] == NULL )
			Error( "Фатальная ошибка: Невозможно отвести помять для \
хранения информации \"Сундучка игр\"" );
		lstrcpy( ListGameType[ m_cGameList.GetItemCount() ], str );

		pIt->aCols[2] = DupString( "Файл" );

		pItem.iItem = m_cGameList.GetItemCount();
		pItem.iSubItem = 0;
		pItem.lParam = (LPARAM) pIt;
		m_cGameList.InsertItem( &pItem );//( m_cGameList.GetItemCount(), str, 0 );
		UpdateData( TRUE );
	}
}

void GameChestDlg::OnClickGamelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int i = 1;

	if ( !bCalledFromKeyDown ) {
		i = (int)m_cGameList.GetSelectionMark() + 1;//GetFirstSelectedItemPosition();
		if ( i != 0 ) {
			int iTmp;
			POSITION posTmp = m_cGameList.GetFirstSelectedItemPosition();
			
			iItemsSelectedCount = m_cGameList.GetSelectedCount();
			for ( int j = 1; j <= iItemsSelectedCount; j++ ){
				iTmp = m_cGameList.GetNextSelectedItem( posTmp ) + 1;
				if ( iTmp > 0 )
					iSelectedItem[j] = iTmp;
			}
//			iSelectedItem[ iItemsSelectedCount ] = i;
			iLastClicked = i;
		}
	} else
		bCalledFromKeyDown = false;
//	if ( i == 0 && m_cRunGame.IsWindowEnabled() )
//		m_cRunGame.EnableWindow( FALSE );
	if ( i != 0 ) {
		m_cRemoveGame.EnableWindow( TRUE );
		m_cRunGame.EnableWindow( TRUE );
	}
	if ( iLastClicked != 0 )
		m_strGameComment = ListText[ iLastClicked - 1 ];
	UpdateData( FALSE );
	
	*pResult = 0;
}

void GameChestDlg::OnReturnGamelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	lstrcpy( strRunGame, ListText[ iLastClicked - 1 ] );//(int)m_cGameList.GetFirstSelectedItemPosition() - 1 ] );

	*pResult = 0;
}

void GameChestDlg::OnKeydownGamelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;
	int iTmp = m_cGameList.GetSelectionMark() + 1;

	//TODO: обеспечить работу с выделением нескольких объектов
	switch( (*pLVKeyDown).wVKey ) {
//	case 32: // Space
//		iSelectedItem[] = iTmp;
//		iLastClicked = iTmp;
/*	case 33: //Page Up
		if ( iTmp > 1 ) {
			iItemSelected = iTmp - 19;
			if ( iItemSelected < 1 )
				iItemSelected = 1;
		}
		break;
	case 34: //Page Down
		if ( iTmp < m_cGameList.GetItemCount() ) {
			iItemSelected = iTmp + 19;
			if ( iItemSelected > m_cGameList.GetItemCount() )
				iItemSelected = m_cGameList.GetItemCount();
		}
		break;
	case 35: //End
		if ( iTmp < m_cGameList.GetItemCount() )
			iItemSelected = m_cGameList.GetItemCount();
		break;
	case 36: //Home
		if ( iTmp < m_cGameList.GetItemCount() )
			iItemSelected = 1;
		break;
	case 38: // Arrow Up
		if ( iTmp > 1 )
			iItemSelected = iTmp - 1;
		break;
	case 40: // Arrow Down
		if ( iTmp < m_cGameList.GetItemCount() )
			iItemSelected = iTmp + 1;
		break;
*/	case 46: //Delete
		if ( iItemsSelectedCount != 0 )
			OnRemovegame();
		*pResult = 0;
		return;
	default:
		*pResult = 0;
		return;
	}
	bCalledFromKeyDown = true;
	OnClickGamelist( pNMHDR, pResult);
	
	*pResult = 0;
}

void GameChestDlg::OnCancel() 
{
	OnExit( false );
}

void GameChestDlg::OnOK() 
{
	OnExit( true );
}

void GameChestDlg::OnExit( bool bOk )
{
	int iGetItemCount = m_cGameList.GetItemCount();

	m_strGameComment = "";
	if ( bOk ) {
		char str[MAX_PATH * 3];
		char str1[10];

		for ( int i = 1; i <= iniFile.GameChest.NumberOfGames; i++ )
			free( iniFile.GameChest.pGame[i - 1] );
		// Пример данных: 'test","c:\tmp\test.py","c:\tmp\","File' (первая и последняя кавычки отбрасываются при чтении из ini файла)
		for ( i = 0; i < m_cGameList.GetItemCount(); i++ ) {
			if ( !lstrcmp( ListGameType[i], "Файл" ) )
				lstrcpy( str1, "File" );
			else if ( !lstrcmp( ListGameType[i], "Директория" ) )
				lstrcpy( str1, "Dir" );
			else if ( !lstrcmp( ListGameType[i], "Архив Zip" ) )
				lstrcpy( str1, "Zip" );
			wsprintf( str, "\"%s\",\"%s\",\"%s\",\"%s\"", ListGameName[i],
					ListText[i], ListGamePath[i], str1 );
			iniFile.GameChest.pGame[i] = (char *)malloc( lstrlen( str ) + 1 );
			if ( iniFile.GameChest.pGame[i] == NULL )
				Error( "Фатальная ошибка: невозможно отвести память для \
информации из \"Hydrofin.ini\"" );
			lstrcpy( iniFile.GameChest.pGame[i], str );
		}
		iOldiniFile_GameChest_NumberOfGames = iniFile.GameChest.NumberOfGames;
		iniFile.GameChest.NumberOfGames	= i;
		iniFile.GameChest.ColumnWidth1 = m_cGameList.GetColumnWidth( 0 );
		iniFile.GameChest.ColumnWidth2 = m_cGameList.GetColumnWidth( 1 );
		iniFile.GameChest.ColumnWidth3 = m_cGameList.GetColumnWidth( 2 );
		CDialog::OnOK();
	} else
		CDialog::OnCancel();
	lstrcpy( strRunGameName, ListGameName[ iLastClicked - 1 ] );
	for ( int i = 0; i < iGetItemCount; i++ )
		free( ListGameName[i] );
	lstrcpy( strRunGamePath, ListGamePath[ iLastClicked - 1 ] );
	for ( i = 0; i < iGetItemCount; i++ )
		free( ListGamePath[i] );
	for ( i = 0; i < iGetItemCount; i++ )
		free( ListText[i] );
}

void GameChestDlg::OnDblclkGamelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if ( (int)m_cGameList.GetFirstSelectedItemPosition() != 0 ) {
		GameChestDlg::OnClickGamelist( pNMHDR, pResult );
		GameChestDlg::OnOK();
	}

	*pResult = 0;
}

void GameChestDlg::OnRclickGamelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_cGameList.SetSelectionMark( pNMListView->iItem );	// не работает?
	GameChestDlg::OnClickGamelist( pNMHDR, pResult );	

	*pResult = 0;
}

void GameChestDlg::OnGetdispinfoGamelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// Provide the item or subitem's text, if requested. 
	if (pDispInfo->item.mask & LVIF_TEXT) { 
		MYITEM *pIt = (MYITEM *) (pDispInfo->item.lParam); 
		lstrcpy( pDispInfo->item.pszText, 
				pIt->aCols[ pDispInfo->item.iSubItem ] ); 
	} 
	*pResult = 0;
}

void GameChestDlg::RemoveGameFromList( int iEntry ) { // Удаление одного элемента из списка игр
	int i;

	if ( m_cGameList.GetItemCount() > 1 ) {
		for ( i = iEntry; i + 1 < m_cGameList.GetItemCount(); i++ ) {
			free( ListText[i] );
			ListText[i] = (char *)malloc( lstrlen( ListText[i + 1] ) + 1 );
			if ( ListText[i] == NULL )
				Error( "Фатальная ошибка: Невозможно отвести помять для \
хранения информации \"Сундучка игр\"" );
			lstrcpy( ListText[i], ListText[i + 1] );

			free( ListGameName[i] );
			ListGameName[i] = (char *)malloc( lstrlen( ListGameName[i + 1] ) + 1 );
			if ( ListGameName[i] == NULL )
				Error( "Фатальная ошибка: Невозможно отвести помять для \
хранения информации \"Сундучка игр\"" );
			lstrcpy( ListGameName[i], ListGameName[i + 1] );
			
			free( ListGamePath[i] );
			ListGamePath[i] = (char *)malloc( lstrlen( ListGamePath[i + 1] ) + 1 );
			if ( ListGamePath[i] == NULL )
				Error( "Фатальная ошибка: Невозможно отвести помять для \
хранения информации \"Сундучка игр\"" );
			lstrcpy( ListGamePath[i], ListGamePath[i + 1] );

			free( ListGameType[i] );
			ListGameType[i] = (char *)malloc( lstrlen( ListGameType[i + 1] ) + 1 );
			if ( ListGameType[i] == NULL )
				Error( "Фатальная ошибка: Невозможно отвести помять для \
хранения информации \"Сундучка игр\"" );
			lstrcpy( ListGameType[i], ListGameType[i + 1] );
		}
	} else
		i = 0;
	free( ListText[i] );
	free( ListGameName[i] );
	free( ListGamePath[i] );
	free( ListGameType[i] );
}

void GameChestDlg::OnRemovegame() 
{
	int iTmp;
	POSITION posTmp = m_cGameList.GetFirstSelectedItemPosition();

	int List[32000];

	iItemsSelectedCount = m_cGameList.GetSelectedCount();

	int iSelCount = iItemsSelectedCount;
	for ( int j = 1; j <= iItemsSelectedCount; j++ )
		List[j] = iSelectedItem[ j ] + 1;

	for ( j = 1; j <= iSelCount; j++ ) {
		iTmp = List[ j ] - j;//m_cGameList.GetNextSelectedItem( posTmp ) + 1;
		if ( iTmp > 0 ) {
			GameChestDlg::RemoveGameFromList( iTmp - 1 );
			m_cGameList.DeleteItem( iTmp - 1 );
			iSelectedItem[ j ] = 0;
		}
	}
	iItemsSelectedCount = 0;
//	iLastClicked = 0;

	m_cRemoveGame.EnableWindow( FALSE );
	m_cRunGame.EnableWindow( FALSE );
	m_strGameComment = "Игра не выбрана";
	UpdateData( FALSE );
}

void GameChestDlg::OnItemchangedGamelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
//	MessageBox( "OnItemchangedGamelist" );
	POSITION posTmp = m_cGameList.GetFirstSelectedItemPosition();
	int iTmp;

	iLastClicked = m_cGameList.GetSelectionMark() + 1;
	iItemsSelectedCount = m_cGameList.GetSelectedCount();
	for ( int i = 1; i <= iItemsSelectedCount; i++ ){
		iTmp = m_cGameList.GetNextSelectedItem( posTmp ) + 1;
//		iTmp = m_cGameList.GetSelectionMark() + 1;
		if ( iTmp > 0 )
			iSelectedItem[i] = iTmp;
	}

	bCalledFromKeyDown = true;
	OnClickGamelist( pNMHDR, pResult);
	*pResult = 0;
}

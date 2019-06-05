// HydrofinDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "Hydrofin.h"
#include "HydrofinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HydrofinDlg dialog


HydrofinDlg::HydrofinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(HydrofinDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(HydrofinDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void HydrofinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(HydrofinDlg)
	DDX_Control(pDX, IDC_CONFIGTAB, m_cConfigTab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(HydrofinDlg, CDialog)
	//{{AFX_MSG_MAP(HydrofinDlg)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HydrofinDlg message handlers

void HydrofinDlg::OnContextMenu(CWnd* pWnd, CPoint point) {
	CMenu *m_lMenu;
	CPoint m_pPoint;

	m_pPoint = point;
//	ClientToScreen( &m_pPoint );
	m_lMenu = GetMenu();
	m_lMenu = m_lMenu->GetSubMenu( 0 );
	m_lMenu->TrackPopupMenu( TPM_CENTERALIGN + TPM_LEFTBUTTON, m_pPoint.x, 
			m_pPoint.y, NULL );
}

void HydrofinDlg::HydrofinConfig() {

	//TabCtrl_GetItem(HWND hwnd, int iItem, TC_ITEM FAR* pitem);
	//TabCtrl_DeleteAllItems( HydrofinDlg::m_cConfigTab );
}


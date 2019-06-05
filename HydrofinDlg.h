#if !defined(AFX_HYDROFINDLG_H__AB811AF3_6060_4D5C_9CD5_CAA992EBC9A5__INCLUDED_)
#define AFX_HYDROFINDLG_H__AB811AF3_6060_4D5C_9CD5_CAA992EBC9A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HydrofinDlg.h : header file
//

#include "resource.h"
#include "GameChestDlg.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// HydrofinDlg dialog

class HydrofinDlg : public CDialog
{
// Construction
public:
//	GameChestDlg m_GameChestDlg;
	void HydrofinConfig( void );
	HydrofinDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(HydrofinDlg)
	enum { IDD = IDD_CONFIG };
	CTabCtrl	m_cConfigTab;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HydrofinDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(HydrofinDlg)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HYDROFINDLG_H__AB811AF3_6060_4D5C_9CD5_CAA992EBC9A5__INCLUDED_)

// StopDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Frac.h"
#include "RTWIN.hpp"
#include "StopDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern HWND hDlgCurrent;

/////////////////////////////////////////////////////////////////////////////
// CStopDialog dialog


CStopDialog::CStopDialog(RTVALS *rt, CWnd* pParent /*=NULL*/)
	: m_rt1(rt), m_bDidOk(false), CDialog()
{
    Create( CStopDialog::IDD, pParent );
	//{{AFX_DATA_INIT(CStopDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CStopDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStopDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStopDialog, CDialog)
	//{{AFX_MSG_MAP(CStopDialog)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStopDialog message handlers

void CStopDialog::OnOK() 
{
	CDialog::OnOK();
    m_bDidOk = true;
}

bool CStopDialog::DidOk()
{
    return m_bDidOk;
}

void CStopDialog::DoStop()
{
    m_bDidOk = true;
}

void CStopDialog::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
    if ( m_rt1->stopset )
    {
        RECT r;
        GetWindowRect( &r );
        m_rt1->StopPos.x = r.left;
        m_rt1->StopPos.y = r.top;
    }
}

void CStopDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    CDialog::OnActivate(nState, pWndOther, bMinimized);

    if (nState == WA_INACTIVE)  // becoming inactive
    {
        hDlgCurrent = NULL;
    }
    else                        // becoming active
    {
        hDlgCurrent = GetSafeHwnd();
    }
}

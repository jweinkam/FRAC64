#if !defined(AFX_STOPDIALOG_H__51B526CE_8696_4366_AFE2_6E7E52C825BC__INCLUDED_)
#define AFX_STOPDIALOG_H__51B526CE_8696_4366_AFE2_6E7E52C825BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StopDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStopDialog dialog

class CStopDialog : public CDialog
{
// Construction
public:
	CStopDialog(RTVALS *rt, CWnd* pParent = NULL);   // standard constructor

    bool DidOk();
    void ClearDidOk() { m_bDidOk = false; }
    void DoStop();
    
// Dialog Data
	//{{AFX_DATA(CStopDialog)
	enum { IDD = IDD_STOP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStopDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStopDialog)
	virtual void OnOK();
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    bool m_bDidOk;
    RTVALS *m_rt1;
public:
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STOPDIALOG_H__51B526CE_8696_4366_AFE2_6E7E52C825BC__INCLUDED_)

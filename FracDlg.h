// FracDlg.h : header file
//

#if !defined(AFX_FRACDLG_H__5F0BFBC8_55C7_4D1F_83A0_B8605F9E94CF__INCLUDED_)
#define AFX_FRACDLG_H__5F0BFBC8_55C7_4D1F_83A0_B8605F9E94CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rtwin.hpp"

/////////////////////////////////////////////////////////////////////////////
// CFracDlg dialog

class CFracDlg : public CDialog
{
// Construction
public:
	CFracDlg(rtvals *rt1, CWnd* pParent = NULL);	// standard constructor

    void WindowSizeChanged();
    void ParameterChanged();
    bool IsSeries() { return m_bIsSeries; }
    bool IsStartRunning() { return m_bIsStartRunning; }
    void ClearStartRunning() { m_bIsStartRunning = false; }
// Dialog Data
	//{{AFX_DATA(CFracDlg)
	enum { IDD = IDD_FRAC_DIALOG };
	CEdit	m_Width;
	CEdit	m_Height;
	CString	m_Xc;
	CString	m_Yc;
	BOOL	m_SimpleScan;
	CString	m_Zoom;
	CString	m_Jy;
	CString	m_Jx;
	BOOL	m_DrawPosition;
	BOOL	m_RestartScan;
	long	m_Count;
	BOOL	m_BlankScreen;
	double	m_Angle;
	BOOL	m_fillRegion;
    int     m_DrawMandle;
	BOOL	m_bUseWindowSize;
	CString	m_DynamicMax;
	CString	m_RenderTime;
	CString	m_JZoom;
	long	m_lBits;
    long    m_lThreads;
    double  m_dOrder;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFracDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFracDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnChangeXc();
	afx_msg void OnChangeYc();
	afx_msg void OnChangeScale();
	afx_msg void OnChangeAngle();
	afx_msg void OnChangeJx();
	afx_msg void OnChangeJy();
	afx_msg void OnChangeCount();
	afx_msg void OnSimplescan();
	afx_msg void OnBlank();
	afx_msg void OnRegion();
	afx_msg void OnDrawposition();
	afx_msg void OnMandle();
	afx_msg void OnJulia();
	afx_msg void OnRender();
	afx_msg void OnRenderseries();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnColorsettings();
	afx_msg void OnQuit();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnUseWindowSize();
	afx_msg void OnChangeJscale();
	afx_msg void OnChangeBits();
    afx_msg void OnBnClickedRestartscan();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    rtvals *m_rt1;
private:
    bool m_bIsStartRunning;
    bool m_bIsSeries;
public:
    afx_msg void OnEnChangeThreads();
    afx_msg void OnEnChangeOrder();
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRACDLG_H__5F0BFBC8_55C7_4D1F_83A0_B8605F9E94CF__INCLUDED_)


// FRAC64NonTemplate2017Dlg.h : header file
//

#pragma once


// CFRAC64NonTemplate2017Dlg dialog
class CFRAC64NonTemplate2017Dlg : public CDialog
{
// Construction
public:
	CFRAC64NonTemplate2017Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FRAC64NONTEMPLATE2017_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};

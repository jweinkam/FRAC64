#if !defined(AFX_COLORSETTINGS_H__276506AC_E00E_4A19_8AD6_301EDD773F57__INCLUDED_)
#define AFX_COLORSETTINGS_H__276506AC_E00E_4A19_8AD6_301EDD773F57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorSettings dialog

class CColorSettings : public CDialog
{
// Construction
public:
	CColorSettings(RTVALS *rt, CWnd* pParent = NULL);   // standard constructor

    void InitComboBox();
// Dialog Data
	//{{AFX_DATA(CColorSettings)
	enum { IDD = IDD_COLORSETTINGS };
	CComboBox	m_Positions;
	int		m_Blue;
	double	m_Cycle;
	int		m_Green;
	int		m_CountScaling;
	int		m_NumberColors;
	int		m_Red;
	int		m_Method;
	double	m_Sat;
	double	m_Val;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorSettings)
	afx_msg void OnChangeNumbercolors();
	afx_msg void OnChangeRed();
	afx_msg void OnChangeGreen();
	afx_msg void OnChangeBlue();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnSelchangePosition();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
    RTVALS *m_rt;
    RTVALS m_rtcopy;
    int m_CurrentPosition;
    BOOL m_bInited;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSETTINGS_H__276506AC_E00E_4A19_8AD6_301EDD773F57__INCLUDED_)

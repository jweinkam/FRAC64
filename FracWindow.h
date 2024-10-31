#if !defined(AFX_FRACWINDOW_H__9A6F0382_BBEA_449B_AF63_6DF4016C51BC__INCLUDED_)
#define AFX_FRACWINDOW_H__9A6F0382_BBEA_449B_AF63_6DF4016C51BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FracWindow.h : header file
//
#include "RTWIN.hpp"

POINTL GetPoint(RTVALS *rt, CLargeFixedInt &x, CLargeFixedInt &y,
    CLargeFixedInt &pixelAmountHx, CLargeFixedInt &pixelAmountHy,
    CLargeFixedInt &pixelAmountVx, CLargeFixedInt &pixelAmountVy,
    long maxx, long maxy);

/////////////////////////////////////////////////////////////////////////////
// CFracWindow frame

class CFracWindow : public CFrameWnd
{
	DECLARE_DYNCREATE(CFracWindow)
	CFracWindow();           // protected constructor used by dynamic creation

    void SetRTVALS( RTVALS *rt ) { m_rt = rt; }
protected:

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFracWindow)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFracWindow();

	// Generated message map functions
	//{{AFX_MSG(CFracWindow)
    afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg LRESULT OnDrawPixels(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateTitle(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
    RTVALS *m_rt;
public:
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRACWINDOW_H__9A6F0382_BBEA_449B_AF63_6DF4016C51BC__INCLUDED_)

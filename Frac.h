 // Frac.h : main header file for the FRAC application
//

#if !defined(AFX_FRAC_H__825E9AD6_6BB7_4291_8464_512E318794AB__INCLUDED_)
#define AFX_FRAC_H__825E9AD6_6BB7_4291_8464_512E318794AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFracApp:
// See Frac.cpp for the implementation of this class
//

class CFracApp : public CWinApp
{
public:
	CFracApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFracApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFracApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAC_H__825E9AD6_6BB7_4291_8464_512E318794AB__INCLUDED_)


// FRAC64NonTemplate2017.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CFRAC64NonTemplate2017App:
// See FRAC64NonTemplate2017.cpp for the implementation of this class
//

class CFRAC64NonTemplate2017App : public CWinApp
{
public:
	CFRAC64NonTemplate2017App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CFRAC64NonTemplate2017App theApp;

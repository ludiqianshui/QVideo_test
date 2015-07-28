// AVCompare.h : main header file for the AVCompare DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CAVCompareApp
// See AVCompare.cpp for the implementation of this class
//

class CAVCompareApp : public CWinApp
{
public:
	CAVCompareApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

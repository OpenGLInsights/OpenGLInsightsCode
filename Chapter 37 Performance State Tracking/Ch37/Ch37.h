
// Ch37.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CCh37App:
// See Ch37.cpp for the implementation of this class
//

class CCh37App : public CWinAppEx
{
public:
	CCh37App();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CCh37App theApp;
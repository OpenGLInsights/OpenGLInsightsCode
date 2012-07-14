
// Ch38NV.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CCh38NVApp:
// See Ch38NV.cpp for the implementation of this class
//

class CCh38NVApp : public CWinAppEx
{
public:
	CCh38NVApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CCh38NVApp theApp;
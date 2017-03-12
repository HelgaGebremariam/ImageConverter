
// ImageConvertion.h : main header file for the ImageConvertion application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
// CImageConvertionApp:
// See ImageConvertion.cpp for the implementation of this class
//

class CImageConvertionApp : public CWinApp
{
public:
	CImageConvertionApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	DECLARE_MESSAGE_MAP()
};

extern CImageConvertionApp theApp;

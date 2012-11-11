// JPEGTOBMP.h : main header file for the JPEGTOBMP application
//

#if !defined(AFX_JPEGTOBMP_H__A073B07A_9938_4149_BAE9_A5DFDD25C359__INCLUDED_)
#define AFX_JPEGTOBMP_H__A073B07A_9938_4149_BAE9_A5DFDD25C359__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CJPEGTOBMPApp:
// See JPEGTOBMP.cpp for the implementation of this class
//

class CJPEGTOBMPApp : public CWinApp
{
public:
	CJPEGTOBMPApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJPEGTOBMPApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CJPEGTOBMPApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPEGTOBMP_H__A073B07A_9938_4149_BAE9_A5DFDD25C359__INCLUDED_)

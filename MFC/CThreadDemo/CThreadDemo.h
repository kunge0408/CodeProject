// CThreadDemo.h : main header file for the CTHREADDEMO application
//

#if !defined(AFX_CTHREADDEMO_H__14E87E56_7F9F_4BBA_83BF_68B17D67B229__INCLUDED_)
#define AFX_CTHREADDEMO_H__14E87E56_7F9F_4BBA_83BF_68B17D67B229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCThreadDemoApp:
// See CThreadDemo.cpp for the implementation of this class
//

class CCThreadDemoApp : public CWinApp
{
public:
	CCThreadDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCThreadDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCThreadDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTHREADDEMO_H__14E87E56_7F9F_4BBA_83BF_68B17D67B229__INCLUDED_)

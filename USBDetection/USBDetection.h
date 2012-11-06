// USBDetection.h : main header file for the USBDETECTION application
//

#if !defined(AFX_USBDETECTION_H__290C187A_95F0_499D_A2D8_EA5C68F213BC__INCLUDED_)
#define AFX_USBDETECTION_H__290C187A_95F0_499D_A2D8_EA5C68F213BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CUSBDetectionApp:
// See USBDetection.cpp for the implementation of this class
//

class CUSBDetectionApp : public CWinApp
{
public:
	CUSBDetectionApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBDetectionApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CUSBDetectionApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBDETECTION_H__290C187A_95F0_499D_A2D8_EA5C68F213BC__INCLUDED_)

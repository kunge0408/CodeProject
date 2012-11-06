// USBDetectionDlg.h : header file
//

#if !defined(AFX_USBDETECTIONDLG_H__AD03C5AA_A8B5_49AB_A3D5_EA8BCEAAF91F__INCLUDED_)
#define AFX_USBDETECTIONDLG_H__AD03C5AA_A8B5_49AB_A3D5_EA8BCEAAF91F__INCLUDED_

#include "PCHost.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////

#include "PCHost.h"
#include "HCDevice.h"
#include "Port.h"
/////////////////////////////////////////////////////
//#include "Extent.h"
/////////////////////////////////////////////////////

//////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CUSBDetectionDlg dialog

struct node{
int i;
HTREEITEM TreeItem;
HANDLE handle;
PUSB_NODE_INFORMATION hubInfo;
PUSB_NODE_CONNECTION_INFORMATION connectionInfo;
PCHAR strShow;
};

#define MAX_TREE_NODE 100

class CUSBDetectionDlg : public CDialog
{
// Construction
public:
	BOOL InsertPort(Port* pPort,HTREEITEM hHub,int numPort);
	CUSBDetectionDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUSBDetectionDlg)
	enum { IDD = IDD_USBDETECTION_DIALOG };
	CListCtrl	m_ListUp;
	CListCtrl	m_ListDown;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBDetectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	Port* pPort;
	HCDevice* pHCDevice;
	PCHost* pMyPC;
	CTreeCtrl	m_Tree;
	CImageList m_ImageList_Tree;
	CImageList m_ImageList_ListUp;
	CImageList m_ImageList_ListDown;
	HICON m_hIcon;

	node Node[MAX_TREE_NODE];
	int nodeNum;

	void InitialTree();
	void InitialListUp();
	void InitialListDown();
	// Generated message map functions
	//{{AFX_MSG(CUSBDetectionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCancle();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRetry();
	afx_msg void OnDblclkListup(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBDETECTIONDLG_H__AD03C5AA_A8B5_49AB_A3D5_EA8BCEAAF91F__INCLUDED_)

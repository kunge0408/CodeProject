// JPEGTOBMPDlg.h : header file
//

#if !defined(AFX_JPEGTOBMPDLG_H__3CBD781B_E0FD_4141_B0E3_40FBE39A5CC2__INCLUDED_)
#define AFX_JPEGTOBMPDLG_H__3CBD781B_E0FD_4141_B0E3_40FBE39A5CC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CJPEGTOBMPDlg dialog

class CJPEGTOBMPDlg : public CDialog
{
// Construction
public:
	CJPEGTOBMPDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CJPEGTOBMPDlg)
	enum { IDD = IDD_JPEGTOBMP_DIALOG };
	CEdit	m_JpgToBmp;
	CEdit	m_BmpFileName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJPEGTOBMPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CJPEGTOBMPDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnJpegToBmp();
	afx_msg void OnBmpToJpeg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPEGTOBMPDLG_H__3CBD781B_E0FD_4141_B0E3_40FBE39A5CC2__INCLUDED_)

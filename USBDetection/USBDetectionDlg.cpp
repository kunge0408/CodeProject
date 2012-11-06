// USBDetectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "USBDetection.h"
#include "USBDetectionDlg.h"
#include "Extent.h"
#include "vndrlist.h"
#pragma comment (lib,"hid.lib")
#pragma comment (lib,"setupapi.lib")
#pragma comment (lib,"cfgmgr32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BUFSIZE 1024
CHAR szComputerName[BUFSIZE]={0};
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBDetectionDlg dialog

CUSBDetectionDlg::CUSBDetectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBDetectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUSBDetectionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//Initial tree
	m_ImageList_Tree.Create(16, 16, 0, 16, 16);

	m_ImageList_Tree.Add(AfxGetApp()->LoadIcon(IDI_COMPUTER));
	m_ImageList_Tree.Add(AfxGetApp()->LoadIcon(IDI_HUB));
	m_ImageList_Tree.Add(AfxGetApp()->LoadIcon(IDI_CONNECTED));
	m_ImageList_Tree.Add(AfxGetApp()->LoadIcon(IDI_UNCONNECTED));



}

void CUSBDetectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUSBDetectionDlg)
	DDX_Control(pDX, IDC_LISTUP, m_ListUp);
	DDX_Control(pDX, IDC_LISTDOWN, m_ListDown);
	DDX_Control(pDX, IDC_TREE, m_Tree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUSBDetectionDlg, CDialog)
	//{{AFX_MSG_MAP(CUSBDetectionDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CANCLE, OnCancle)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_BN_CLICKED(IDC_RETRY, OnRetry)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTUP, OnDblclkListup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBDetectionDlg message handlers

BOOL CUSBDetectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	//Initial ListUp
	m_ListUp.ModifyStyle(LVS_TYPEMASK, LVS_REPORT, LVS_LIST);
	m_ListUp.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListUp.SetTextColor(RGB(0, 51, 0));
	m_ListUp.SetBkColor(RGB(240, 240, 240));

	m_ListUp.InsertColumn(0, _T("Co. Info"), LVCFMT_LEFT, 135);
	m_ListUp.InsertColumn(1, _T("Device Name"), LVCFMT_LEFT, 155);
	m_ListUp.InsertColumn(2, _T("Vender ID"), LVCFMT_CENTER, 120);
	m_ListUp.InsertColumn(3, _T("Product ID"), LVCFMT_CENTER, 120);

	//Initial ListDown list

	m_ListDown.SetImageList(&m_ImageList_ListDown, LVSIL_SMALL);
	m_ListDown.ModifyStyle(LVS_TYPEMASK, LVS_REPORT, LVS_LIST);
	m_ListDown.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListDown.SetTextColor(RGB(0, 51, 0));
	m_ListDown.SetBkColor(RGB(240, 240, 240));

	m_ListDown.InsertColumn(0, _T(""), LVCFMT_LEFT, 140);
	
	m_ListDown.InsertColumn(1, _T(""), LVCFMT_LEFT, 390);

	InitialTree();
// 	InitialListUp();
// 	InitialListDown();

	return TRUE;  // return TRUE  unless you set the focus to a control
}
BOOL CUSBDetectionDlg::InsertPort(Port *pPort,HTREEITEM hHub,int numPort)
{
	CString str;
	PUSB_NODE_CONNECTION_INFORMATION conInfo = pPort->GetConnectionInfo();
	if(conInfo->DeviceIsHub)
	{
		
		HTREEITEM hPort = m_Tree.InsertItem(DriverNameToDeviceDesc(pPort->GetDriverKeyName()), 2, 2, hHub);//DriverNameToDeviceDesc(pPort->GetDriverKeyName())
		Node[nodeNum].i = 2;			//i = 3 -> Port
		Node[nodeNum].TreeItem = hPort;
		Node[nodeNum].handle = pPort->GetParentPortHandle();
		Node[nodeNum].hubInfo =pPort->GetHubInfo();
		Node[nodeNum].connectionInfo =  NULL;
		Node[nodeNum].strShow = pPort->GetHubName();
		nodeNum++;
		for(int numPort = 0; numPort < pPort->GetPortNum(); numPort++)
		{
			
			Port* pPort1 = new Port(pPort->GetPortHandle(), numPort + 1);		
			PUSB_NODE_CONNECTION_INFORMATION conInfo = pPort1->GetConnectionInfo();
			if(conInfo->ConnectionStatus)
				InsertPort(pPort1,hPort,numPort + 1);
			else
			{
				str.Format("[Port%d]NoDeviceConnected", numPort + 1);
				HTREEITEM hPort1 = m_Tree.InsertItem(str, 3, 3, hPort);				
				Node[nodeNum].i = 3;			//i = 3 -> Port
				Node[nodeNum].TreeItem = hPort1;
				Node[nodeNum].handle = pPort1->GetParentPortHandle();
				Node[nodeNum].hubInfo = NULL;
				Node[nodeNum].connectionInfo = NULL;
				Node[nodeNum].strShow = _T("No Device Connected!");
				nodeNum++;
			}
			delete[] pPort1;
		}
	}
	else
	{
		//PCHAR aa=;
		PCHAR aa=DriverNameToDeviceDesc(pPort->GetDriverKeyName()); 
		HTREEITEM hPort = m_Tree.InsertItem(aa, 2, 2, hHub);//DriverNameToDeviceDesc(pPort->GetDriverKeyName())
		Node[nodeNum].i = 3;			//i = 3 -> Port
		Node[nodeNum].TreeItem = hPort;
		Node[nodeNum].handle = pPort->GetParentPortHandle();
		Node[nodeNum].hubInfo = NULL;
		Node[nodeNum].connectionInfo = pPort->GetConnectionInfo();
		CString str;
		str.Format("%s",DriverNameToDeviceDesc(pPort->GetDriverKeyName()));
		Node[nodeNum].strShow = DriverNameToDeviceDesc(pPort->GetDriverKeyName());//pPort->GetDriverKeyName();
		nodeNum++;						
	}
	return 1;
	
}
void CUSBDetectionDlg::InitialTree()
{
	//Initial Tree
	m_Tree.DeleteAllItems();

	m_Tree.SetImageList(&m_ImageList_Tree, TVSIL_NORMAL);
	m_Tree.SetTextColor(RGB(0, 51, 0));
	m_Tree.SetBkColor(RGB(240, 240, 240));

	PCHAR lpszComputerName = new CHAR[BUFSIZE];      // pointer to system information string 
	DWORD cchBuff = BUFSIZE;    // size of computer or user name 

	if(!GetComputerName(lpszComputerName, &cchBuff))
		exit(GetLastError());

	strcpy(szComputerName,lpszComputerName);
	nodeNum = 0;
	HTREEITEM hComputer;
	hComputer = m_Tree.InsertItem(lpszComputerName, 0, 0, TVI_ROOT, TVI_LAST);
	Node[nodeNum].i = 0;				//i = 0 -> Computer
	Node[nodeNum].TreeItem = hComputer;
	Node[nodeNum].handle = NULL;
	Node[nodeNum].hubInfo = NULL;
	Node[nodeNum].connectionInfo = NULL;
	Node[nodeNum].strShow = szComputerName;

	nodeNum++;
	pMyPC = new PCHost;

	for(int numHCD = 0; numHCD < pMyPC->GetHCDeviceNum(); numHCD++)
	{
		pHCDevice = new HCDevice(pMyPC->GetDeviceHandle(numHCD));
		HTREEITEM hHostController = m_Tree.InsertItem(pHCDevice->GetHCDDriverKeyName(), 2, 2, hComputer);

		Node[nodeNum].i = 1;	//i = 1 -> HCD
		Node[nodeNum].TreeItem = hHostController;
		Node[nodeNum].handle = pHCDevice;
		Node[nodeNum].hubInfo = NULL;
		Node[nodeNum].connectionInfo = NULL;
		Node[nodeNum].strShow = pHCDevice->GetHCDDriverKeyName();

		nodeNum++;

//		MessageBox(DriverNameToDeviceDesc(pHCDevice->GetHCDDriverKeyName()));

		PCHAR rootHubName = pHCDevice->GetRootHubName();
		if(rootHubName)
		{
			HTREEITEM hHub = m_Tree.InsertItem("RootHub", 2, 2, hHostController);
			
			Node[nodeNum].i = 2;			//i = 2 -> Hub
			Node[nodeNum].TreeItem = hHub;
			Node[nodeNum].handle = pHCDevice->GetHCDeviceHandle();
			Node[nodeNum].hubInfo = pHCDevice->GetHubInfo();
			Node[nodeNum].connectionInfo = NULL;
			Node[nodeNum].strShow = pHCDevice->GetRootHubName();

			nodeNum++;

			for(int numPort = 0; numPort < pHCDevice->GetPortNum(); numPort++)
			{
				pPort = new Port(pHCDevice->GetRootHubHandle(), numPort + 1);
				
				PUSB_NODE_CONNECTION_INFORMATION conInfo = pPort->GetConnectionInfo();
				
				if(conInfo->ConnectionStatus)
				{
//					str.Format("[Port%d]DeviceConnected", numPort + 1);
// 					HTREEITEM hPort = m_Tree.InsertItem(DriverNameToDeviceDesc(pPort->GetDriverKeyName()), 2, 2, hHub);//DriverNameToDeviceDesc(pPort->GetDriverKeyName())
// 					Node[nodeNum].i = 3;			//i = 3 -> Port
// 					Node[nodeNum].TreeItem = hPort;
// 					Node[nodeNum].handle = pHCDevice->GetRootHubHandle();
// 					Node[nodeNum].hubInfo = NULL;
// 					Node[nodeNum].connectionInfo = pPort->GetConnectionInfo();
// 					Node[nodeNum].strShow = pPort->GetDriverKeyName();
// 					nodeNum++;
					InsertPort(pPort,hHub,numPort + 1);
	
				}
				else
				{
					CString str;
					str.Format("[Port%d]NoDeviceConnected", numPort + 1);
					HTREEITEM hPort = m_Tree.InsertItem(str, 3, 3, hHub);
					Node[nodeNum].i = 3;			//i = 3 -> Port
					Node[nodeNum].TreeItem = hPort;
					Node[nodeNum].handle = pHCDevice->GetRootHubHandle();
					Node[nodeNum].hubInfo = NULL;
					Node[nodeNum].connectionInfo = NULL;
					Node[nodeNum].strShow = _T("No Device Connected!");
					nodeNum++;
				}
				m_Tree.Expand(hHub, TVE_EXPAND);
				delete pPort;
			}
			m_Tree.Expand(hHostController, TVE_EXPAND);
		}
		delete pHCDevice;
	}

	m_Tree.Expand(hComputer, TVE_EXPAND);
	delete[] lpszComputerName;
	delete pMyPC;
}

void CUSBDetectionDlg::InitialListUp()
{
	int j = 0;
	CString str;

	m_ListUp.DeleteAllItems();

	for(int i = 0; i < (int)m_Tree.GetCount(); i++)
	{
		if(Node[i].i == 3 && Node[i].connectionInfo != NULL)
		{
			USHORT IDVendor = Node[i].connectionInfo->DeviceDescriptor.idVendor;
			USHORT IDProduct = Node[i].connectionInfo->DeviceDescriptor.idProduct;

			for(int n = 0; n < NUMOFCO; n++)
			{
				if(IDVendor == USBVendorIDs[n].usVendorID)
				{
					str.Format(USBVendorIDs[n].szVendor);
					break;
				}

				if(n == NUMOFCO - 1)			// unknown vid
				{
					str.Format("Unknown");
				}
			}

			m_ListUp.InsertItem(j, str, j);

//			MessageBox(Node[i].pDeviceName);
			m_ListUp.SetItemText(j, 1, DriverNameToDeviceDesc(Node[i].strShow));

			str.Format("0x %04X", IDVendor);
			m_ListUp.SetItemText(j, 2, str);

			str.Format("0x %04X", IDProduct);
			m_ListUp.SetItemText(j, 3, str);

			j++;
		}
	}
}

void CUSBDetectionDlg::InitialListDown()
{
	m_ListDown.DeleteAllItems();

//	m_ListDown.InsertItem(0, Node[0].strShow, 1);
}

void CUSBDetectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUSBDetectionDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUSBDetectionDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CUSBDetectionDlg::OnCancle() 
{
	// TODO: Add your control notification handler code here
	PostQuitMessage(WM_QUIT);
}

void CUSBDetectionDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
//	TVITEM item = pNMTreeView->itemNew; 

	HTREEITEM m_SelectItem; 
    CString str;
	int numNode = m_Tree.GetCount();

	m_ListDown.DeleteAllItems();

	m_SelectItem = m_Tree.GetSelectedItem();
	
	for(int i = 0; i < numNode; i ++)
	{
		if(Node[i].TreeItem == m_SelectItem)
		{
			if(Node[i].i == 0)
			{
				m_ListDown.InsertItem(0, Node[i].strShow, 0);
				break;
			}
			else if(Node[i].i == 1)
			{
				m_ListDown.InsertItem(0, _T("Host Controller: "), 0);
				m_ListDown.SetItemText(0, 1, Node[i].strShow);
				break;
			}
			else if(Node[i].i == 2)
			{
				m_ListDown.InsertItem(0, _T("Root Hub: "), 0);
				m_ListDown.SetItemText(0, 1, Node[i].strShow);

				m_ListDown.InsertItem(1, _T("HubIsBusPowered: "), 1);
				if(Node[i].hubInfo->u.HubInformation.HubIsBusPowered)
				{
					str.Format("Hub is Bus Powered");
				}
				else
				{
					str.Format("Self Power");
				}
				m_ListDown.SetItemText(1, 1, str);

				m_ListDown.InsertItem(2, _T("Number of Ports: "), 2);
				str.Format("%d", Node[i].hubInfo->u.HubInformation.HubDescriptor.bNumberOfPorts);
				m_ListDown.SetItemText(2, 1, str);

				m_ListDown.InsertItem(3, _T("bDescriptorType: "), 3);
				str.Format("0x %04X", Node[i].hubInfo->u.HubInformation.HubDescriptor.bDescriptorType);
				m_ListDown.SetItemText(3, 1, str);

				m_ListDown.InsertItem(4, _T("wHubCharacteristics: "), 4);
				str.Format("0x %04X", Node[i].hubInfo->u.HubInformation.HubDescriptor.wHubCharacteristics);
				m_ListDown.SetItemText(4, 1, str);

				m_ListDown.InsertItem(5, _T("bPowerOnToPowerGood: "), 5);
				str.Format("0x %04X", Node[i].hubInfo->u.HubInformation.HubDescriptor.bPowerOnToPowerGood);
				m_ListDown.SetItemText(5, 1, str);

				m_ListDown.InsertItem(6, _T("bHubControlCurrent: "), 6);
				str.Format("0x %04X", Node[i].hubInfo->u.HubInformation.HubDescriptor.bHubControlCurrent);
				m_ListDown.SetItemText(6, 1, str);
				
				break;
			}
			else
			{
				if(Node[i].connectionInfo != NULL)
				{
					m_ListDown.InsertItem(0, _T("DeviceDriverKeyName"), 0);
					m_ListDown.SetItemText(0, 1, Node[i].strShow);

					m_ListDown.InsertItem(1, _T("bLength:"), 1);
					str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bLength);
					m_ListDown.SetItemText(1, 1, str);

					m_ListDown.InsertItem(2, _T("bcdUSB:"), 2);
					str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bcdUSB);
					m_ListDown.SetItemText(2, 1, str);

					m_ListDown.InsertItem(3, _T("bDeviceClass:"), 3);
					str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bDeviceClass);
					m_ListDown.SetItemText(3, 1, str);

					m_ListDown.InsertItem(4, _T("bDeviceSubClass:"), 4);
					str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bDeviceSubClass);
					m_ListDown.SetItemText(4, 1, str);

					m_ListDown.InsertItem(5, _T("bDeviceProtocol:"), 5);
					str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bDeviceProtocol);
					m_ListDown.SetItemText(5, 1, str);

					m_ListDown.InsertItem(6, _T("bMaxPacketSize0:"), 6);
					str.Format("0x %04X	( %d )", Node[i].connectionInfo->DeviceDescriptor.bMaxPacketSize0, 
						Node[i].connectionInfo->DeviceDescriptor.bMaxPacketSize0);
					m_ListDown.SetItemText(6, 1, str);

					m_ListDown.InsertItem(7, _T("bLength:"), 7);
					str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bLength);
					m_ListDown.SetItemText(7, 1, str);

					m_ListDown.InsertItem(8, _T("idVendor:"), 8);
					str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.idVendor);
					m_ListDown.SetItemText(8, 1, str);

					m_ListDown.InsertItem(9, _T("idProduct:"), 9);
					str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.idProduct);
					m_ListDown.SetItemText(9, 1, str);

					m_ListDown.InsertItem(10, _T("bcdDevice:"), 10);
					str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bcdDevice);
					m_ListDown.SetItemText(10, 1, str);

					m_ListDown.InsertItem(11, _T("iManufacturer:"), 11);
					str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.iManufacturer);
					m_ListDown.SetItemText(11, 1, str);

					m_ListDown.InsertItem(12, _T("iSerialNumber:"), 12);
					str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.iSerialNumber);
					m_ListDown.SetItemText(12, 1, str);

					m_ListDown.InsertItem(13, _T("bNumConfigurations:"), 13);
					str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bNumConfigurations);
					m_ListDown.SetItemText(13, 1, str);

					m_ListDown.InsertItem(14, _T("ConnectionStatus:"), 14);
					str.Format("0x %04X	( Connected )", Node[i].connectionInfo->ConnectionStatus);
					m_ListDown.SetItemText(14, 1, str);

					m_ListDown.InsertItem(15, _T("High Speed:"), 15);
					if(Node[i].connectionInfo->LowSpeed)
					{
						m_ListDown.SetItemText(15, 1, _T("No"));
					}
					else
					{
						m_ListDown.SetItemText(15, 1, _T("Yes"));
					}
				}
				else
				{
					m_ListDown.InsertItem(0, Node[i].strShow, 0);
				}

				break;
			}
		}
	}

	*pResult = 0;
}

void CUSBDetectionDlg::OnRetry() 
{
	// TODO: Add your control notification handler code here

	InitialTree();
	InitialListUp();
	InitialListDown();	
}

void CUSBDetectionDlg::OnDblclkListup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW*   pNMListView   =   (NM_LISTVIEW*)pNMHDR;

	int numNode = m_Tree.GetCount();
	CString str;

	int j = 0;
	for(int i = 0; i < numNode; i++)
	{
		if(Node[i].i == 3 && Node[i].connectionInfo != NULL)
		{
			if(j != pNMListView->iItem)
			{
				j++;
			}
			else
			{
				m_ListDown.DeleteAllItems();

				m_ListDown.InsertItem(0, _T("DeviceDriverKeyName"), 0);
				m_ListDown.SetItemText(0, 1, Node[i].strShow);

				m_ListDown.InsertItem(1, _T("bLength:"), 1);
				str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bLength);
				m_ListDown.SetItemText(1, 1, str);

				m_ListDown.InsertItem(2, _T("bcdUSB:"), 2);
				str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bcdUSB);
				m_ListDown.SetItemText(2, 1, str);

				m_ListDown.InsertItem(3, _T("bDeviceClass:"), 3);
				str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bDeviceClass);
				m_ListDown.SetItemText(3, 1, str);

				m_ListDown.InsertItem(4, _T("bDeviceSubClass:"), 4);	
				str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bDeviceSubClass);
				m_ListDown.SetItemText(4, 1, str);

				m_ListDown.InsertItem(5, _T("bDeviceProtocol:"), 5);
				str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bDeviceProtocol);
				m_ListDown.SetItemText(5, 1, str);

				m_ListDown.InsertItem(6, _T("bMaxPacketSize0:"), 6);
				str.Format("0x %04X	( %d )", Node[i].connectionInfo->DeviceDescriptor.bMaxPacketSize0, 
				Node[i].connectionInfo->DeviceDescriptor.bMaxPacketSize0);
				m_ListDown.SetItemText(6, 1, str);
 
				m_ListDown.InsertItem(7, _T("bLength:"), 7);
				str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bLength);
				m_ListDown.SetItemText(7, 1, str);

				m_ListDown.InsertItem(8, _T("idVendor:"), 8);
				str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.idVendor);
				m_ListDown.SetItemText(8, 1, str);

				m_ListDown.InsertItem(9, _T("idProduct:"), 9);
				str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.idProduct);
				m_ListDown.SetItemText(9, 1, str);

				m_ListDown.InsertItem(10, _T("bcdDevice:"), 10);
				str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bcdDevice);
				m_ListDown.SetItemText(10, 1, str);

				m_ListDown.InsertItem(11, _T("iManufacturer:"), 11);
				str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.iManufacturer);
				m_ListDown.SetItemText(11, 1, str);

				m_ListDown.InsertItem(12, _T("iSerialNumber:"), 12);
				str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.iSerialNumber);
				m_ListDown.SetItemText(12, 1, str);

				m_ListDown.InsertItem(13, _T("bNumConfigurations:"), 13);
				str.Format("0x %04X", Node[i].connectionInfo->DeviceDescriptor.bNumConfigurations);
				m_ListDown.SetItemText(13, 1, str);

				m_ListDown.InsertItem(14, _T("ConnectionStatus:"), 14);
				str.Format("0x %04X	( Connected )", Node[i].connectionInfo->ConnectionStatus);
				m_ListDown.SetItemText(14, 1, str);

				m_ListDown.InsertItem(15, _T("High Speed:"), 15);
				if(Node[i].connectionInfo->LowSpeed)
				{
					m_ListDown.SetItemText(15, 1, _T("No"));
				}
				else
				{
					m_ListDown.SetItemText(15, 1, _T("Yes"));
				}

				break;
			}
		}
	}
	
	*pResult = 0;
}



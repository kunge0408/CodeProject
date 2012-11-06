// Port.h: interface for the Port class.
//
//////////////////////////////////////////////////////////////////////
/***************************************************************************

Interface:
int GetPortIndex()	: return the index of this port
HANDLE GetPortHandle() :	reutrn the handle of the device which owns this port
PUSB_NODE_CONNECTION_INFORMATION GetConnectionInfo() : return the connection 
	information of this port;

***************************************************************************/

#if !defined(AFX_PORT_H__86DA2E3A_0925_4304_AAEB_0EA1B2046CCB__INCLUDED_)
#define AFX_PORT_H__86DA2E3A_0925_4304_AAEB_0EA1B2046CCB__INCLUDED_

#include "usbioctl.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "usbioctl.h"
#include "devioctl.h"
#include "Extent.h"

class Port  
{
public:
	Port(HANDLE handle, int index);
	PUSB_NODE_CONNECTION_INFORMATION GetConnectionInfo();
	int GetPortIndex();
	PCHAR GetDriverKeyName();
	HANDLE GetParentPortHandle();	//Retreive the root hub handle of this port;
	PCHAR GetHubName();
	PUSB_NODE_INFORMATION GetHubInfo();
	PCHAR GetHubDeviceName();
	HANDLE GetPortHandle();
	int  GetPortNum();
	Port();
	virtual ~Port();

private:
	void InitConnectionInfo();
	PUSB_NODE_CONNECTION_INFORMATION ConnectionInfo;
	int iPortIndex;

	HANDLE hRootHubHandle;
	BOOL bIsHub;
	HANDLE  hHubHandle;
	PUSB_NODE_INFORMATION  HubInfo;
	int iPortNum;
};

#endif // !defined(AFX_PORT_H__86DA2E3A_0925_4304_AAEB_0EA1B2046CCB__INCLUDED_)

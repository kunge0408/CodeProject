// HCDevice.h: interface for the HCDevice class.
//
//////////////////////////////////////////////////////////////////////
/******************************************************************************

Interface:

void SetHandle(HANDLE handle) : Initialize the device
int GetPortNum() : return the number of ports of this device

*******************************************************************************/
#if !defined(AFX_HCDEVICE_H__5C5AAE37_4078_42AF_8CB6_6A03C94A6BEB__INCLUDED_)
#define AFX_HCDEVICE_H__5C5AAE37_4078_42AF_8CB6_6A03C94A6BEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "usbioctl.h"
#include "devioctl.h"

#include "Extent.h"

class HCDevice  
{
public:

	PCHAR GetRootHubName();
	PCHAR GetDeviceName();
	PCHAR GetHCDDriverKeyName();
	void SetHandle(HANDLE handle);
	int GetPortNum();
	HANDLE GetHCDeviceHandle();
	HANDLE GetRootHubHandle();
	PUSB_NODE_INFORMATION GetHubInfo();
	HCDevice(HANDLE hHCD);
	HCDevice();
	virtual ~HCDevice();

private:
	PUSB_NODE_INFORMATION HubInfo;
	int iPortNum;
	HANDLE hHCDevice;
	HANDLE hRootHub;
};

#endif // !defined(AFX_HCDEVICE_H__5C5AAE37_4078_42AF_8CB6_6A03C94A6BEB__INCLUDED_)

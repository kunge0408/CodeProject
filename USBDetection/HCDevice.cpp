// HCDevice.cpp: implementation of the HCDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HCDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HCDevice::HCDevice()
{
	hHCDevice = NULL;
	hRootHub = NULL;
	HubInfo = NULL;
	iPortNum = 0;
}

HCDevice::~HCDevice()
{

}

HCDevice::HCDevice(HANDLE hHCD)
{
	hHCDevice = hHCD;
	hRootHub = NULL;
	iPortNum = 0;
	HubInfo = GetHubInfo();
}

HANDLE HCDevice::GetHCDeviceHandle()
{
	return hHCDevice;
}

HANDLE HCDevice::GetRootHubHandle()
{
	return hRootHub;
}

int HCDevice::GetPortNum()
{
	return iPortNum;
}

void HCDevice::SetHandle(HANDLE hHCD)
{
	hHCDevice = hHCD;
	iPortNum = 0;
	HubInfo = GetHubInfo();
}

PUSB_NODE_INFORMATION HCDevice::GetHubInfo()
{
	ULONG nBytes;
	
	HubInfo = (PUSB_NODE_INFORMATION)malloc(sizeof(USB_NODE_INFORMATION));

	hRootHub = CreateFile(GetDeviceName(),
                            GENERIC_WRITE,
                            FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL);

	BOOL success = DeviceIoControl(hRootHub,
		                           IOCTL_USB_GET_NODE_INFORMATION,
								   HubInfo,
								   sizeof(USB_NODE_INFORMATION),
								   HubInfo,
								   sizeof(USB_NODE_INFORMATION),
								   &nBytes,
								   NULL);
	if(!success)
		return NULL;
	
	iPortNum = HubInfo->u.HubInformation.HubDescriptor.bNumberOfPorts;
	
	return HubInfo;
}

PCHAR HCDevice::GetRootHubName()
{
	BOOL                success;
    ULONG               nBytes;
    USB_ROOT_HUB_NAME   rootHubName;
    PUSB_ROOT_HUB_NAME  rootHubNameW;
    PCHAR               rootHubNameA;

    rootHubNameW = NULL;
    rootHubNameA = NULL;
	
	success = DeviceIoControl(hHCDevice,
                              IOCTL_USB_GET_ROOT_HUB_NAME,
                              0,
                              0,
                              &rootHubName,
                              sizeof(rootHubName),
                              &nBytes,
                              NULL);

    if (!success)
    {
        goto GetRootHubNameError;
    }

    nBytes = rootHubName.ActualLength;

    rootHubNameW =(PUSB_ROOT_HUB_NAME) malloc(nBytes);

    if (rootHubNameW == NULL)
    {

        goto GetRootHubNameError;
    }

    success = DeviceIoControl(hHCDevice,
                              IOCTL_USB_GET_ROOT_HUB_NAME,
                              NULL,
                              0,
                              rootHubNameW,
                              nBytes,
                              &nBytes,
                              NULL);

    if (!success)
    {
       goto GetRootHubNameError;
    }

    rootHubNameA = WideStrToMultiStr(rootHubNameW->RootHubName);
    free(rootHubNameW);

    return rootHubNameA;


GetRootHubNameError:
    if (rootHubNameW != NULL)
    {
        free(rootHubNameW);
        rootHubNameW = NULL;
    }

    return NULL;
}

PCHAR HCDevice::GetDeviceName()
{
	PCHAR rootHubName;
	PCHAR deviceName;

	rootHubName = GetRootHubName();
	
	deviceName = (PCHAR)malloc(strlen(rootHubName) + sizeof("\\\\.\\"));
	strcpy(deviceName, "\\\\.\\");
	strcpy(deviceName + sizeof("\\\\.\\") - 1, rootHubName);

	return deviceName;
}

PCHAR HCDevice::GetHCDDriverKeyName()
{
    BOOL                    success;
    ULONG                   nBytes;
    USB_HCD_DRIVERKEY_NAME  driverKeyName;
    PUSB_HCD_DRIVERKEY_NAME driverKeyNameW;
    PCHAR                   driverKeyNameA;

    driverKeyNameW = NULL;
    driverKeyNameA = NULL;

    // Get the length of the name of the driver key of the HCD
    //
    success = DeviceIoControl(hHCDevice,
                              IOCTL_GET_HCD_DRIVERKEY_NAME,
                              &driverKeyName,
                              sizeof(driverKeyName),
                              &driverKeyName,
                              sizeof(driverKeyName),
                              &nBytes,
                              NULL);

    if (!success)
    {
        goto GetHCDDriverKeyNameError;
    }

    // Allocate space to hold the driver key name
    //
    nBytes = driverKeyName.ActualLength;

    if (nBytes <= sizeof(driverKeyName))
    {
        goto GetHCDDriverKeyNameError;
    }

    driverKeyNameW = (PUSB_HCD_DRIVERKEY_NAME) malloc(nBytes);;

    if (driverKeyNameW == NULL)
    {
        goto GetHCDDriverKeyNameError;
    }

    // Get the name of the driver key of the device attached to
    // the specified port.
    //
    success = DeviceIoControl(hHCDevice,
                              IOCTL_GET_HCD_DRIVERKEY_NAME,
                              driverKeyNameW,
                              nBytes,
                              driverKeyNameW,
                              nBytes,
                              &nBytes,
                              NULL);

    if (!success)
    {
        goto GetHCDDriverKeyNameError;
    }

    // Convert the driver key name
    //
    driverKeyNameA = WideStrToMultiStr(driverKeyNameW->DriverKeyName);

    // All done, free the uncoverted driver key name and return the
    // converted driver key name
    //
    free(driverKeyNameW);

    return driverKeyNameA;


GetHCDDriverKeyNameError:
    // There was an error, free anything that was allocated
    //
    if (driverKeyNameW != NULL)
    {
        free(driverKeyNameW);
        driverKeyNameW = NULL;
    }

    return NULL;
}
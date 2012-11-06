// Port.cpp: implementation of the Port class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Port.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Port::Port()
{
	hRootHubHandle = NULL;
	iPortIndex = 0;
}

Port::~Port()
{

}

HANDLE Port::GetParentPortHandle()
{
	return hRootHubHandle;
}
HANDLE Port::GetPortHandle()
{
	return hHubHandle;
}
int Port::GetPortIndex()
{
	return iPortIndex;
}
int Port::GetPortNum()
{
	return iPortNum;
}

PUSB_NODE_CONNECTION_INFORMATION Port::GetConnectionInfo()
{
	return ConnectionInfo;
}

Port::Port(HANDLE handle, int index)
{
	hRootHubHandle = handle;
	iPortIndex = index;
	InitConnectionInfo();
}
PUSB_NODE_INFORMATION Port::GetHubInfo()
{
	ULONG nBytes;
	
	HubInfo = (PUSB_NODE_INFORMATION)malloc(sizeof(USB_NODE_INFORMATION));

	hHubHandle = CreateFile(GetHubDeviceName(),
                            GENERIC_WRITE,
                            FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL);

	BOOL success = DeviceIoControl(hHubHandle,
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
void Port::InitConnectionInfo()
{
	BOOL success;

	ULONG nBytes;
	nBytes = sizeof(USB_NODE_CONNECTION_INFORMATION) + 
		sizeof(USB_PIPE_INFO) * 30;

	ConnectionInfo = (PUSB_NODE_CONNECTION_INFORMATION)malloc(nBytes);
	ConnectionInfo->ConnectionIndex = iPortIndex;
	success = DeviceIoControl(hRootHubHandle,
                              IOCTL_USB_GET_NODE_CONNECTION_INFORMATION,
							  ConnectionInfo,
                              nBytes,
                              ConnectionInfo,
                              nBytes,
                              &nBytes,
                              NULL);
	if(ConnectionInfo->DeviceIsHub)
	{
		bIsHub = TRUE;
		GetHubInfo();
	}

	if (!success)
	{
		exit(1);
	}
}

PCHAR Port::GetDriverKeyName()
{
	BOOL                                success;
    ULONG                               nBytes;
    USB_NODE_CONNECTION_DRIVERKEY_NAME  driverKeyName;
    PUSB_NODE_CONNECTION_DRIVERKEY_NAME driverKeyNameW;
    PCHAR                               driverKeyNameA;

    driverKeyNameW = NULL;
    driverKeyNameA = NULL;

    driverKeyName.ConnectionIndex = GetPortIndex();

    success = DeviceIoControl(GetParentPortHandle(),
                              IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME,
                              &driverKeyName,
                              sizeof(driverKeyName),
                              &driverKeyName,
                              sizeof(driverKeyName),
                              &nBytes,
                              NULL);

    if (!success)
    {
        goto GetDriverKeyNameError;
    }

    nBytes = driverKeyName.ActualLength;

    if (nBytes <= sizeof(driverKeyName))
    {
        goto GetDriverKeyNameError;
    }

    driverKeyNameW = (PUSB_NODE_CONNECTION_DRIVERKEY_NAME)malloc(nBytes);

    if (driverKeyNameW == NULL)
    {
        goto GetDriverKeyNameError;
    }

    driverKeyNameW->ConnectionIndex = GetPortIndex();

    success = DeviceIoControl(GetParentPortHandle(),
                              IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME,
                              driverKeyNameW,
                              nBytes,
                              driverKeyNameW,
                              nBytes,
                              &nBytes,
                              NULL);

    if (!success)
    {
        goto GetDriverKeyNameError;
    }
    driverKeyNameA = WideStrToMultiStr(driverKeyNameW->DriverKeyName);
    free(driverKeyNameW);

    return driverKeyNameA;


GetDriverKeyNameError:
    if (driverKeyNameW != NULL)
    {
        free(driverKeyNameW);
        driverKeyNameW = NULL;
    }

    return NULL;
}
PCHAR Port::GetHubName()
{

	BOOL                success;
    ULONG               nBytes;
    USB_NODE_CONNECTION_NAME   HubName;
    PUSB_NODE_CONNECTION_NAME  HubNameW;
    PCHAR               HubNameA;
    HubNameW = NULL;
    HubNameA = NULL;
	nBytes = sizeof(USB_NODE_CONNECTION_NAME);
	HubName.ConnectionIndex = 	ConnectionInfo->ConnectionIndex;
	success = DeviceIoControl(GetParentPortHandle(),
		IOCTL_USB_GET_NODE_CONNECTION_NAME,
		&HubName,
		nBytes,
		&HubName,
		nBytes,
		&nBytes,
		NULL);
	
    if (!success)
    {
        goto GetHubNameError;
    }
	
    nBytes = HubName.ActualLength;
	
    HubNameW =(PUSB_NODE_CONNECTION_NAME) malloc(nBytes);	
    if (HubNameW == NULL)
    {
		
        goto GetHubNameError;
    }
	HubNameW->ConnectionIndex = ConnectionInfo->ConnectionIndex;
    success = DeviceIoControl(GetParentPortHandle(),
		IOCTL_USB_GET_NODE_CONNECTION_NAME,
		HubNameW,
		nBytes,
		HubNameW,
		nBytes,
		&nBytes,
		NULL);
	
    if (!success)
    {
		goto GetHubNameError;
    }
    HubNameA = WideStrToMultiStr(HubNameW->NodeName);
    free(HubNameW);
    return HubNameA;
	
	
GetHubNameError:
    if (HubNameW != NULL)
    {
        free(HubNameW);
        HubNameW = NULL;
    }	
    return NULL;
}
PCHAR Port::GetHubDeviceName()
{
	PCHAR HubName;
	PCHAR deviceName;
	
	HubName = GetHubName();
	
	deviceName = (PCHAR)malloc(strlen(HubName) + sizeof("\\\\.\\"));
	strcpy(deviceName, "\\\\.\\");
	strcpy(deviceName + sizeof("\\\\.\\") - 1, HubName);	
	return deviceName;
}
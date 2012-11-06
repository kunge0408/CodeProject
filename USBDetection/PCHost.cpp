// PCHost.cpp: implementation of the PCHost class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PCHost.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PCHost::PCHost()
{
	iHCDNum = 0;
	hHCDeviceHandle = new HANDLE[NUM_HCS_TO_CHECK];

	GetHCDeviceInfo();
}

PCHost::~PCHost()
{
	for(int i = 0; i < GetHCDeviceNum(); i++)
	{
		CloseHandle(hHCDeviceHandle[i]);
	}

	delete [] hHCDeviceHandle;
}

int PCHost::GetHCDeviceNum()
{
	return iHCDNum;
}

HANDLE PCHost::GetDeviceHandle(int num)
{
	if(num >= 0 && num < iHCDNum)
		return hHCDeviceHandle[num];
	else
		return NULL;
}

void PCHost::GetHCDeviceInfo()
{
	char        HCName[16];
	PCHAR* HCDRootHubName = new PCHAR [NUM_HCS_TO_CHECK];

	CString str;

	for(int i = 0; i < NUM_HCS_TO_CHECK; i++)
	{
		wsprintf(HCName, "\\\\.\\HCD%d", i);

        hHCDeviceHandle[i] = CreateFile(HCName,
                            GENERIC_WRITE,
                            FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL);

		if(hHCDeviceHandle[i] == INVALID_HANDLE_VALUE)
			break;
		else
			iHCDNum++;
	}
	delete[] HCDRootHubName;
}
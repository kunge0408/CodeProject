// PCHost.h: interface for the PCHost class.
//
//////////////////////////////////////////////////////////////////////
/***************************************************************************
Interface:
int GetHCDeviceNum() : return the Host Control Device of the object
HANDLE GetDeviceHandle(int num) : return the handle of the numth HCD, (num 
		should be postive) 

/**************************************************************************/
#if !defined(AFX_PCHOST_H__EFC0B855_ADDE_4F45_BE02_6B9D7DF6F409__INCLUDED_)
#define AFX_PCHOST_H__EFC0B855_ADDE_4F45_BE02_6B9D7DF6F409__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "usbioctl.h"
#include "devioctl.h"

#define NUM_HCS_TO_CHECK 10

class PCHost  
{
public:

	HANDLE GetDeviceHandle(int num);
	int GetHCDeviceNum();
	PCHost();
	virtual ~PCHost();

private:
	void GetHCDeviceInfo();
	HANDLE* hHCDeviceHandle;
	int iHCDNum;
protected:
};

#endif // !defined(AFX_PCHOST_H__EFC0B855_ADDE_4F45_BE02_6B9D7DF6F409__INCLUDED_)

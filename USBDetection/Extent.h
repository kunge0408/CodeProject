#include "stdafx.h"

#include <windows.h>
#include <basetyps.h>
#include <winioctl.h>
//#include "cfgmgr32.h"
#include "usbioctl.h"

#include <string.h>

//CHAR		buf[512];

PCHAR WideStrToMultiStr(PWCHAR WideStr);

PCHAR DriverNameToDeviceDesc(PCHAR DriverName);

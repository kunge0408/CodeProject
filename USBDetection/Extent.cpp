#include "stdafx.h"
#include "Extent.h"
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include "cfgmgr32.h"
 

//*****************************************************************************
//
// PCHAR WideStrToMultiStr (PWCHAR WideStr)
//
//*****************************************************************************

	
PCHAR WideStrToMultiStr (PWCHAR WideStr)
{
    ULONG nBytes;
    PCHAR MultiStr;

    // Get the length of the converted string
    //
    nBytes = WideCharToMultiByte(
                 CP_ACP,
                 0,
                 WideStr,
                 -1,
                 NULL,
                 0,
                 NULL,
                 NULL);

    if (nBytes == 0)
    {
        return NULL;
    }

    // Allocate space to hold the converted string
    //
    MultiStr = (PCHAR) malloc(nBytes);

    if (MultiStr == NULL)
    {
        return NULL;
    }

    // Convert the string
    //
    nBytes = WideCharToMultiByte(
                 CP_ACP,
                 0,
                 WideStr,
                 -1,
                 MultiStr,
                 nBytes,
                 NULL,
                 NULL);

    if (nBytes == 0)
    {
        free(MultiStr);
        return NULL;
    }

    return MultiStr;
}

//*****************************************************************************
//
// PCHAR DriverNameToDeviceDesc (PCHAR DriverName)
//
//*****************************************************************************

PCHAR DriverNameToDeviceDesc (PCHAR DriverName)
{
	HDEVINFO hDevInfo = NULL;
    SP_DEVINFO_DATA DeviceInfoData;
    DWORD i;
	LPTSTR buffer = NULL;
	
    // Create a HDEVINFO with all present devices.
    hDevInfo = SetupDiGetClassDevs(NULL,
        0, // Enumerator
        0,
        DIGCF_PRESENT | DIGCF_ALLCLASSES);
    
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        // Insert error handling here.
        return NULL;
    }
    
    // Enumerate through all devices in Set.
    
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	
    for (i=0; SetupDiEnumDeviceInfo(hDevInfo,i,&DeviceInfoData); i++)
    {
        DWORD DataT;
        DWORD buffersize = 0;
        
        // 
        // Call function with null to begin with, 
        // then use the returned buffer size 
        // to Alloc the buffer. Keep calling until
        // success or an unknown failure.
        //
		
        while (!SetupDiGetDeviceRegistryProperty(
            hDevInfo,
            &DeviceInfoData,
            SPDRP_DRIVER,
            &DataT,
            (PBYTE)buffer,
            buffersize,
            &buffersize))
        {
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                // Change the buffer size.
                if (buffer)
				{
					LocalFree(buffer);
					buffer = NULL;
				}
				buffer = (char *)LocalAlloc(LPTR,buffersize);
            }
			else
			{
				// Insert error handling here.
                break;
			}
		}
		
		if(strcmp(DriverName, buffer) == 0)
		{

// 			while (!SetupDiGetDeviceInstanceId(
// 				hDevInfo,
// 				&DeviceInfoData ,
// 				buffer,
// 				buffersize, 
// 				&buffersize    /* optional */
// 				) )
// 			{
// 				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
// 				{
// 					// Change the buffer size.
// 					if (buffer)
// 					{
// 						LocalFree(buffer);
// 						buffer = NULL;
// 					}
// 					buffer = (char *)LocalAlloc(LPTR,buffersize);
// 				}
// 				else
// 				{
// 					// Insert error handling here.
// 					break;
// 				}
// 			}

			while(!SetupDiGetDeviceRegistryProperty(
				hDevInfo,
				&DeviceInfoData,
				SPDRP_FRIENDLYNAME,
				&DataT,
				(PBYTE)buffer,
				buffersize,
				&buffersize))
			{
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					if (buffer)
					{
						LocalFree(buffer);
						buffer = NULL;
					}
					buffer = (char *)LocalAlloc(LPTR,buffersize);
				}
				else
				{
					break;
				}
			}

			break;
		}
    }
    //  Cleanup
//	MessageBox(0, buffer, 0, 0);

	SetupDiDestroyDeviceInfoList(hDevInfo);
//  
// 	CString str;
// 	WCHAR b[1024];
// 	StringFromGUID2(DeviceInfoData.ClassGuid,b,1024);
// 	str.Format("%s%s",b,buffer);
// 	AfxMessageBox(str);
	return buffer;
}
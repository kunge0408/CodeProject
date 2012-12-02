# Microsoft Developer Studio Project File - Name="stm32v350vc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=stm32v350vc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stm32v350vc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stm32v350vc.mak" CFG="stm32v350vc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stm32v350vc - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "stm32v350vc - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stm32v350vc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "stm32v350vc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "stm32v350vc - Win32 Release"
# Name "stm32v350vc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "CMSIS"

# PROP Default_Filter ""
# Begin Group "startup"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\CMSIS\startup\startup_stm32f10x_cl.s
# End Source File
# Begin Source File

SOURCE=..\..\CMSIS\startup\startup_stm32f10x_hd.s
# End Source File
# Begin Source File

SOURCE=..\..\CMSIS\startup\startup_stm32f10x_hd_vl.s
# End Source File
# Begin Source File

SOURCE=..\..\CMSIS\startup\startup_stm32f10x_ld.s
# End Source File
# Begin Source File

SOURCE=..\..\CMSIS\startup\startup_stm32f10x_ld_vl.s
# End Source File
# Begin Source File

SOURCE=..\..\CMSIS\startup\startup_stm32f10x_md.s
# End Source File
# Begin Source File

SOURCE=..\..\CMSIS\startup\startup_stm32f10x_md_vl.s
# End Source File
# Begin Source File

SOURCE=..\..\CMSIS\startup\startup_stm32f10x_xl.s
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\CMSIS\core_cm3.c
# End Source File
# Begin Source File

SOURCE=..\..\CMSIS\system_stm32f10x.c
# End Source File
# End Group
# Begin Group "FWlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\FWlib\src\misc.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_adc.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_bkp.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_can.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_cec.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_crc.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_dac.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_dbgmcu.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_dma.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_exti.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_flash.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_fsmc.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_gpio.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_i2c.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_iwdg.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_pwr.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_rcc.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_rtc.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_sdio.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_spi.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_tim.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_usart.c
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\src\stm32f10x_wwdg.c
# End Source File
# End Group
# Begin Group "USER"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\USER\main.c
# End Source File
# Begin Source File

SOURCE=..\..\USER\stm32f10x_it.c
# End Source File
# End Group
# Begin Group "APP"

# PROP Default_Filter ""
# Begin Group "Uart"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\APP\Uart\uart.c
# End Source File
# End Group
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\CMSIS\core_cm3.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\misc.h
# End Source File
# Begin Source File

SOURCE=..\..\CMSIS\stm32f10x.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_adc.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_bkp.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_can.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_cec.h
# End Source File
# Begin Source File

SOURCE=..\..\USER\stm32f10x_conf.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_crc.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_dac.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_dbgmcu.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_dma.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_exti.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_flash.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_fsmc.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_gpio.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_i2c.h
# End Source File
# Begin Source File

SOURCE=..\..\USER\stm32f10x_it.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_iwdg.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_pwr.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_rcc.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_rtc.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_sdio.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_spi.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_tim.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_usart.h
# End Source File
# Begin Source File

SOURCE=..\..\FWlib\inc\stm32f10x_wwdg.h
# End Source File
# Begin Source File

SOURCE=..\..\CMSIS\system_stm32f10x.h
# End Source File
# Begin Source File

SOURCE=..\..\APP\Uart\inc\uart.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

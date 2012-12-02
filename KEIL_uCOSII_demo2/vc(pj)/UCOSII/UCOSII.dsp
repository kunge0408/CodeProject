# Microsoft Developer Studio Project File - Name="UCOSII" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=UCOSII - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UCOSII.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UCOSII.mak" CFG="UCOSII - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UCOSII - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "UCOSII - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UCOSII - Win32 Release"

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
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "UCOSII - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "UCOSII - Win32 Release"
# Name "UCOSII - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "CommonFiles"

# PROP Default_Filter ""
# Begin Group "uCOS-II"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\os_cfg_r.h"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\os_core.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCOS-II_BSP\Os_cpu_c.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\os_dbg_r.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\os_flag.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\os_mbox.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\os_mem.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\os_mutex.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\os_q.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\os_sem.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\os_task.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\os_time.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCOS-II_BSP\os_timer4_c.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\os_tmr.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\ucos_ii.c"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCoS-II_SRC\ucos_ii.h"
# End Source File
# End Group
# Begin Group "FL2440_BSP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\CommonFiles\FL2440_BSP\2440BSP_SRC\2440_init.s
# End Source File
# End Group
# End Group
# Begin Group "Drivers"

# PROP Default_Filter ""
# Begin Group "LED_test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Drivers\LED_test\LED_test_SRC\LED_test.c
# End Source File
# End Group
# Begin Group "LCD_test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Drivers\LCD_test\LCD_test_SRC\lcd.c
# End Source File
# Begin Source File

SOURCE=..\..\Drivers\LCD_test\LCD_test_SRC\xyx_320_240.c
# End Source File
# End Group
# Begin Group "Printf_test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Drivers\Printf_test\Printf_test_SRC\Printf.c
# End Source File
# End Group
# End Group
# Begin Group "App_Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\App_Main\App_SRC\App_Main.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CommonFiles\FL2440_BSP\2440BSP_INC\2440addr.h
# End Source File
# Begin Source File

SOURCE=..\..\App_Main\App_INC\App.h
# End Source File
# Begin Source File

SOURCE=..\..\App_Main\App_INC\App_cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\FL2440_BSP\2440BSP_INC\def.h
# End Source File
# Begin Source File

SOURCE=..\..\App_Main\App_INC\includes.h
# End Source File
# Begin Source File

SOURCE=..\..\Drivers\LCD_test\LCD_test_INC\lcd.h
# End Source File
# Begin Source File

SOURCE=..\..\Drivers\LED_test\LED_test_INC\LED_test.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\FL2440_BSP\2440BSP_INC\Mytype.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\FL2440_BSP\2440BSP_INC\Option.h
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCOS-II_BSP\os_cfg.h"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCOS-II_BSP\OS_CPU.H"
# End Source File
# Begin Source File

SOURCE="..\..\CommonFiles\uCOS-II\uCOS-II_BSP\os_timer4_c.h"
# End Source File
# Begin Source File

SOURCE=..\..\Drivers\Printf_test\Printf_test_INC\Printf.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 2002         SEGGER Microcontroller Systeme GmbH        *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

**** emWin/GSC Grafical user interface for embedded applications ****
emWin is protected by international copyright laws. Knowledge of the
source code may not be used to write a similar product. This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : WM_Video.c
Purpose     :
----------------------------------------------------------------------
*/

#include <stddef.h>
#include <stdio.h>

#include "WM.h"
#include "GUI.h"
#include "FRAMEWIN.h"
#include "BUTTON.h"

#include "GUIDEMO.h"

#define countof(Obj) (sizeof(Obj)/sizeof(Obj[0]))



static void _cbFrameWinTest(WM_MESSAGE* pMsg);

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define MAX_WINDOWS   5

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static WM_HWIN _ahWin[MAX_WINDOWS];
static char    _IsCompletelyVis;

static char    flag_exit=0;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _CreateWindow
*/
static void _CreateWindow(void) {
  unsigned i;
  for (i = 0; i < MAX_WINDOWS; i++) {
    if (_ahWin[i] == 0) {
      WM_HWIN hWin;
      char ac[32];
      sprintf(ac, "Test window %d", i + 1);
      hWin = FRAMEWIN_CreateEx(5 + 10 * i, 135 + 10 * i, 120, 60, 0, WM_CF_SHOW, FRAMEWIN_CF_MOVEABLE, 0, ac, _cbFrameWinTest);
      FRAMEWIN_SetClientColor(hWin, GUI_INVALID_COLOR);
      _ahWin[i] = hWin;
      break;
    }
  }
}

/*********************************************************************
*
*       _DeleteWindow
*/
static void _DeleteWindow(void) {
  unsigned i;
  for (i = 0; i < MAX_WINDOWS; i++) {
    if (WM_IsVisible(_ahWin[i])) {
      WM_DeleteWindow(_ahWin[i]);
      _ahWin[i] = 0;
      break;
    }
  }
}

static void _DeleteAllWindow(void) {
  unsigned i;
  for (i = 0; i < MAX_WINDOWS; i++) {	    
      WM_DeleteWindow(_ahWin[i]);
      _ahWin[i] = 0;         
  }
}



/*********************************************************************
*
*       _ShowWindow
*/
static void _ShowWindow(void) {
  unsigned i;
  for (i = 0; i < MAX_WINDOWS; i++) {
    if (_ahWin[i] != 0) {
      if (WM_IsVisible(_ahWin[i]) == 0) {
        WM_ShowWindow(_ahWin[i]);
        break;
      }
    }
  }
}

/*********************************************************************
*
*       _HideWindow
*/
static void _HideWindow(void) {
  unsigned i;
  for (i = 0; i < MAX_WINDOWS; i++) {
    if (WM_IsVisible(_ahWin[i])) {
      WM_HideWindow(_ahWin[i]);
      break;
    }
  }
}

/*********************************************************************
*
*       Static code, callbacks
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbBkWin
*/
static void _cbBkWin(WM_MESSAGE* pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();
    break;
  case WM_NOTIFY_PARENT:
    if (pMsg->Data.v == WM_NOTIFICATION_RELEASED) {
      int Id = WM_GetId(pMsg->hWinSrc);
      switch (Id) {
      case GUI_ID_BUTTON0:  /* Create window */
        _CreateWindow();
        break;
      case GUI_ID_BUTTON1:  /* Delete window */
        _DeleteWindow();
        break;
      case GUI_ID_BUTTON2:  /* Show window   */
        _ShowWindow();
        break;
      case GUI_ID_BUTTON3:  /* Hide window   */
        _HideWindow();
        break;	
			case GUI_ID_BUTTON4:  /* Hide window   */				  
        flag_exit=1;
        break;

      }
    }
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _cbFrameWinVideo
*/
static void _cbFrameWinVideo(WM_MESSAGE* pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    if (_IsCompletelyVis) {
      GUI_SetBkColor(GUI_DARKGREEN);
      GUI_Clear();
      GUI_SetColor(GUI_WHITE);
      GUI_DispStringAt("Completely visible", 5, 5);
    } else {
      GUI_SetBkColor(GUI_GRAY);
      GUI_Clear();
      GUI_SetColor(GUI_WHITE);
      GUI_DispStringAt("Not completely visible", 5, 5);
    }
    break;
  case WM_NOTIFY_VIS_CHANGED:
    {
      WM_HWIN hWin;
      int IsCompletelyVis;
      hWin = WM_GetClientWindow(pMsg->hWin);
      IsCompletelyVis = WM_IsCompletelyVisible(hWin);
      if (_IsCompletelyVis != IsCompletelyVis) {
        _IsCompletelyVis = IsCompletelyVis;
        WM_InvalidateWindow(hWin);    /* Only required if content changes if partially hidden */
      }
    }
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _cbFrameWinTest
*/
static void _cbFrameWinTest(WM_MESSAGE* pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(GUI_DARKRED);
    GUI_Clear();
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
void GUIDEMO_VmVideo(void) {

  int i;

  FRAMEWIN_Handle hWinVideo;
  BUTTON_Handle hBut[5];


  //GUI_Init();
  WM_SetCallback(WM_HBKWIN, _cbBkWin);
  /* Create buttons */
  hBut[0] = BUTTON_CreateEx(240,  5, 75, 18, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON0);
  BUTTON_SetText(hBut[0], "Create win");
  hBut[1] = BUTTON_CreateEx(240, 25, 75, 18, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON1);
  BUTTON_SetText(hBut[1], "Delete win");
  hBut[2] = BUTTON_CreateEx(240, 45, 75, 18, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON2);
  BUTTON_SetText(hBut[2], "Show win");
  hBut[3] = BUTTON_CreateEx(240, 65, 75, 18, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON3);
  BUTTON_SetText(hBut[3], "Hide win");

	hBut[4] = BUTTON_CreateEx(240, 85, 75, 18, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON4);
  BUTTON_SetText(hBut[4], "Exit");

  /* Create framewin video */
  hWinVideo = FRAMEWIN_CreateEx(5, 5, 170, 120, 0, WM_CF_SHOW, FRAMEWIN_CF_MOVEABLE, 0, "Video window", _cbFrameWinVideo);
  FRAMEWIN_SetClientColor(hWinVideo, GUI_INVALID_COLOR);
  /* Create test windows */
  _CreateWindow();
  _CreateWindow();
  _CreateWindow();
  do {
    GUI_Delay(1000);
  }while(!flag_exit);

  GUIDEMO_Wait(); 

	for (i = 0; i < countof(hBut); i++)	//É¾³ý°´Å¥
	{
		BUTTON_Delete(hBut[i]);
	}

	_DeleteAllWindow(); //É¾³ý´°Ìå 
	
  WM_DeleteWindow(hWinVideo);   //É¾³ý´°Ìå 	
  WM_SetCallback(WM_HBKWIN, 0); //É¾³ý»Øµ÷º¯Êý
  
  GUI_Clear();   


}

/**************************** end of file ***************************/

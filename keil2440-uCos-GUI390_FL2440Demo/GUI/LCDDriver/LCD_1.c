/*
*********************************************************************************************************
*   											 uC/GUI
*   					 Universal graphic software for embedded applications
*
*   					(c) Copyright 2002, Micrium Inc., Weston, FL
*   					(c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*   		   µC/GUI is protected by international copyright laws. Knowledge of the
*   		   source code may not be used to write a similar product. This file may
*   		   only be used in accordance with a license and should not be redistributed
*   		   in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File		: LCD_1.C
Purpose 	: Glue code for second display in a multi-controller
			  environment
---------------------------END-OF-HEADER------------------------------
*/
#include <stddef.h> 		  /* needed for definition of NULL */
#include "LCD_Private.H"	  /* private modul definitions & config */
#include "GUI_Private.H"
#include "GUIDebug.h"
#include "LCD_0.h"  		  /* Defines for first display */
#include "LCDConf.h"
#include "LCD_ConfDefaults.h"
#define   COLOR LCD_COLORINDEX


#ifndef LCD_DISPLAY_INDEX
  #define LCD_DISPLAY_INDEX 0
#endif


#include "LCD_LTV350QV_F05.h"


#define PHYS2LOG(x, y) x, y

#define GETPIXEL(x, y)     GetPixelIndex(PHYS2LOG(x, y))

#ifndef LCD_ENABLE_REG_ACCESS
  #define LCD_ENABLE_REG_ACCESS()
#endif

#ifndef LCD_ENABLE_MEM_ACCESS
  #define LCD_ENABLE_MEM_ACCESS()
#endif


#define  LCDSIM_SetPixelIndex(x, y, c, LCD_DISPLAY_INDEX)   SetPixel(x, y, c) 
#define  LCDSIM_GetPixelIndex(x,y, LCD_DISPLAY_INDEX)       GetPixelIndex(x,y)


static void XorPixel(int x, int y);


/*
*********************************************************
*   													*
*   	Internal set pixel routines 					*
*   													*
*********************************************************
*/

static void SetPixel(int x, int y, LCD_PIXELINDEX c)
{
   PutPixel(x,y,c);
}




unsigned int GetPixelIndex(int x, int y)
{
	return GetPixelVal(x,y);
}
/*
		*********************************************************
		*   													*
		*   	LCD_SetColorIndex   							*
		*   	LCD_SetBkColorIndex 							*
		*   													*
		*********************************************************
*/

/*
static int GetColorIndex(int i)  // i is 0 or 1
{
	return  (GUI_Context.DrawMode & LCD_DRAWMODE_REV) ? i - 1 : i;
}

*/

/*
*********************************************************
*   													*
*   	LCD_Init : Init the display 					*
*   													*
*********************************************************
*/
int LCD_L0_Init(void)
{
	
	LCD_L0_ReInit();
	LCD_Off();

	return 0;    /* Init successfull ! */
}
/*
*********************************************************
*   													*
*   	LCD_L0_ReInit : Re-Init the display 			*
*   													*
*********************************************************

ReInit contains all of the code that can be re-executed at any point without
changing or even disturbing what can be seen on the LCD.
Note that it is also used as a subroutine by LCD_Init().

*/

void LCD_L0_ReInit(void)
{
    Lcd_Tft_LTV350QV_F05_Init();    
			  
}
void LCD_L0_Off(void)
{
	Lcd_EnvidOnOff(0);
}
void LCD_L0_On(void)
{
	Lcd_EnvidOnOff(1);
}



void LCD_L0_DrawPixel(int x, int y) {
  if (GUI_Context.DrawMode & LCD_DRAWMODE_XOR) {
    XorPixel(x, y);
  } else {
    SetPixel(x, y, LCD_COLORINDEX);
  }
}


static void XorPixel(int x, int y)
{
	LCD_PIXELINDEX Index = GetPixelIndex(x, y);
	SetPixel(x, y, LCD_NUM_COLORS - 1 - Index);
}

/*
*********************************************************
*   													*
*   	LCD_L0_XorPixel 								*
*   													*
*********************************************************

Purpose:  This routine is called by emWin. It writes 1 pixel into the
		  display.

*/

void LCD_L0_XorPixel(int x, int y)
{
	XorPixel(x, y);
}

void LCD_L0_SetPixelIndex(int x, int y, int ColorIndex)
{
	SetPixel(x, y, ColorIndex);
}
/*
*********************************************************
*   													*
*   	   LCD_L0_DrawHLine optimized   				*
*   													*
*   	   16 bit bus, Using BITBLT 					*
*   													*
*********************************************************
*/
/*
 //LCD_1.C Primary CODE
 
void LCD_L0_DrawHLine(int x0, int y, int x1)
{
  
	if (GUI_Context.DrawMode & LCD_DRAWMODE_XOR)
	{
		while (x0 <= x1)
		{
			XorPixel(x0, y);
			x0++;
		}
	}
	else
	{
		while (x0 <= x1)
		{
			SetPixel(x0, y, COLOR);
			x0++;
		}
	}
	
	
	//Glib_Line(x0,y,x1,y,COLOR);
}*/

/*********************************************************************
*
*       LCD_L0_DrawHLine
*/
void LCD_L0_DrawHLine(int x0, int y,  int x1) {
  if (GUI_Context.DrawMode & LCD_DRAWMODE_XOR) {
    for (;x0 <= x1; x0++) {
      XorPixel(x0, y);
    }
  } else {
    for (;x0 <= x1; x0++) {
      SetPixel(x0, y, LCD_COLORINDEX);
    }
  }
}


/*
*********************************************************
*   													*
*   	   LCD_L0_DrawVLine optimized   				*
*   													*
*   	   16 bit bus, using BITBLT 					*
*   													*
*********************************************************
*/

void LCD_L0_DrawVLine(int x, int y0, int y1)
{
	if (GUI_Context.DrawMode & LCD_DRAWMODE_XOR)
	{
		while (y0 <= y1)
		{
			XorPixel(x, y0);
			y0++;
		}
	}
	else
	{
		while (y0 <= y1)
		{
			SetPixel(x, y0, COLOR);
			y0++;
		}
	}
}

/*
*********************************************************
*   													*
*   	   LCD_FillRect 								*
*   													*
*********************************************************
*/

void LCD_L0_FillRect(int x0, int y0, int x1, int y1)
{
#if !LCD_SWAP_XY
	for (; y0 <= y1; y0++)
	{
		LCD_L0_DrawHLine(x0, y0, x1);
	}
#else
	for (; x0 <= x1; x0++)
	{
		LCD_L0_DrawVLine(x0, y0, y1);
	}
#endif
}


unsigned int LCD_L0_GetPixelIndex(int x, int y)
{
    return GETPIXEL(x,y);	
}



/*********************************************************************
*
*       _DrawBitLine1BPP
*/
static void _DrawBitLine1BPP(int x, int y, U8 const*p, int Diff, int xsize, const LCD_PIXELINDEX*pTrans) {
  LCD_PIXELINDEX Index0, Index1;   
  
  Index0 = *(pTrans + 0);
  Index1 = *(pTrans + 1);
  x+=Diff;
  switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS|LCD_DRAWMODE_XOR)) {
  case 0:    /* Write mode */
    do {
      LCDSIM_SetPixelIndex(x++,y, (*p & (0x80>>Diff)) ? Index1 : Index0, LCD_DISPLAY_INDEX);
			if (++Diff==8) {
        Diff=0;
				p++;
			}
		} while (--xsize);
    break;
  case LCD_DRAWMODE_TRANS:
    do {
  		if (*p & (0x80>>Diff))
        LCDSIM_SetPixelIndex(x,y, Index1, LCD_DISPLAY_INDEX);
      x++;
			if (++Diff==8) {
        Diff=0;
				p++;
			}
		} while (--xsize);
    break;
  case LCD_DRAWMODE_XOR | LCD_DRAWMODE_TRANS:
  case LCD_DRAWMODE_XOR:
    do {
  		if (*p & (0x80>>Diff)) {
        int Pixel = LCDSIM_GetPixelIndex(x,y, LCD_DISPLAY_INDEX);
        LCDSIM_SetPixelIndex(x,y, LCD_NUM_COLORS-1-Pixel, LCD_DISPLAY_INDEX);
      }
      x++;
			if (++Diff==8) {
        Diff=0;
				p++;
			}
		} while (--xsize);
    break;
	}
}




/*********************************************************************
*
*       _DrawBitLine2BPP
*/
#if (LCD_MAX_LOG_COLORS > 2)
static void _DrawBitLine2BPP(int x, int y, U8 const * p, int Diff, int xsize, const LCD_PIXELINDEX * pTrans) {
  LCD_PIXELINDEX Pixels = *p;
  int CurrentPixel = Diff;
  x += Diff;
  switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
    case 0:
      if (pTrans) {
        do {
          int Shift = (3 - CurrentPixel) << 1;
          int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
          LCD_PIXELINDEX PixelIndex = *(pTrans + Index);
          SetPixel(x++, y, PixelIndex);
          if (++CurrentPixel == 4) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      } else {
        do {
          int Shift = (3 - CurrentPixel) << 1;
          int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
          SetPixel(x++, y, Index);
          if (++CurrentPixel == 4) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      }
      break;
    case LCD_DRAWMODE_TRANS:
      if (pTrans) {
        do {
          int Shift = (3 - CurrentPixel) << 1;
          int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
          if (Index) {
            LCD_PIXELINDEX PixelIndex = *(pTrans + Index);
            SetPixel(x, y, PixelIndex);
          }
          x++;
          if (++CurrentPixel == 4) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      } else {
        do {
          int Shift = (3 - CurrentPixel) << 1;
          int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
          if (Index) {
            SetPixel(x, y, Index);
          }
          x++;
          if (++CurrentPixel == 4) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      }
      break;
  }
}
#endif

/*********************************************************************
*
*       _DrawBitLine4BPP
*/
#if (LCD_MAX_LOG_COLORS > 4)
static void _DrawBitLine4BPP(int x, int y, U8 const * p, int Diff, int xsize, const LCD_PIXELINDEX * pTrans) {
  LCD_PIXELINDEX Pixels = *p;
  int CurrentPixel = Diff;
  x += Diff;
  switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
    case 0:
      if (pTrans) {
        do {
          int Shift = (1 - CurrentPixel) << 2;
          int Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
          LCD_PIXELINDEX PixelIndex = *(pTrans + Index);
          SetPixel(x++, y, PixelIndex);
          if (++CurrentPixel == 2) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      } else {
        do {
          int Shift = (1 - CurrentPixel) << 2;
          int Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
          SetPixel(x++, y, Index);
          if (++CurrentPixel == 2) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      }
      break;
    case LCD_DRAWMODE_TRANS:
      if (pTrans) {
        do {
          int Shift = (1 - CurrentPixel) << 2;
          int Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
          if (Index) {
            LCD_PIXELINDEX PixelIndex = *(pTrans + Index);
            SetPixel(x, y, PixelIndex);
          }
          x++;
          if (++CurrentPixel == 2) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      } else {
        do {
          int Shift = (1 - CurrentPixel) << 2;
          int Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
          if (Index) {
            SetPixel(x, y, Index);
          }
          x++;
          if (++CurrentPixel == 2) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      }
      break;
  }
}
#endif



void LCD_L0_SetLUTEntry(U8 Pos, LCD_COLOR color) {
  int i;
	
  #if LCD_BITSPERPIXEL
    U16 aColorSep[3];
    for (i=0; i<3; i++) {
      aColorSep[i] = color &0xff;
      color>>=8;
    }
  #endif

  LCD_ENABLE_MEM_ACCESS();
}



U8 LCD_ControlCache(U8 command)
{
	return 1;
}



/*********************************************************************
*
*       _DrawBitLine8BPP
*/
#if (LCD_MAX_LOG_COLORS > 16)
static void _DrawBitLine8BPP(int x, int y, U8 const*p, int xsize, const LCD_PIXELINDEX*pTrans) {
  LCD_PIXELINDEX pixel;
  if ((GUI_Context.DrawMode & LCD_DRAWMODE_TRANS)==0) {
    if (pTrans) {
      for (;xsize > 0; xsize--,x++,p++) {
        pixel = *p;
        SetPixel(x, y, *(pTrans+pixel));
      }
    } else {
      for (;xsize > 0; xsize--,x++,p++) {
        SetPixel(x, y, *p);
      }
    }
  } else {   /* Handle transparent bitmap */
    if (pTrans) {
      for (; xsize > 0; xsize--, x++, p++) {
        pixel = *p;
        if (pixel) {
          SetPixel(x+0, y, *(pTrans+pixel));
        }
      }
    } else {
      for (; xsize > 0; xsize--, x++, p++) {
        pixel = *p;
        if (pixel) {
          SetPixel(x+0, y, pixel);
        }
      }
    }
  }
}
#endif



/*********************************************************************
*
*       _DrawBitLine16BPP
*********************************************************************/

#if (LCD_BITSPERPIXEL > 8)

static void _DrawBitLine16BPP(int x, int y, U16 const*p, int xsize, const LCD_PIXELINDEX*pTrans) {
  LCD_PIXELINDEX pixel;
  if ((GUI_Context.DrawMode & LCD_DRAWMODE_TRANS)==0) {
    if (pTrans) {
      for (;xsize > 0; xsize--,x++,p++) {
        pixel = *p;
        SetPixel(x, y, *(pTrans+pixel));
      }
    } else {
      for (;xsize > 0; xsize--,x++,p++) {
        SetPixel(x, y, *p);
      }
    }
  } else {   // Handle transparent bitmap
    if (pTrans) {
      for (; xsize > 0; xsize--, x++, p++) {
        pixel = *p;
        if (pixel) {
          SetPixel(x+0, y, *(pTrans+pixel));
        }
      }
    } else {
      for (; xsize > 0; xsize--, x++, p++) {
        pixel = *p;
        if (pixel) {
          SetPixel(x+0, y, pixel);
        }
      }
    }
  }
}


#endif	 




/*********************************************************************
*
*       LCD_L0_DrawBitmap
*********************************************************************/

void LCD_L0_DrawBitmap(int x0,
					   int y0,
					   int xsize,
					   int ysize,
					   int BitsPerPixel,
					   int BytesPerLine,
					   const U8 *pData,
					   int Diff,
					   const LCD_PIXELINDEX *pTrans)
{
	int i;
	 for (i=0; i<ysize; i++) {
    switch (BitsPerPixel) {
    case 1:
      _DrawBitLine1BPP(x0, i+y0, pData, Diff, xsize, pTrans);
      break;
    #if (LCD_MAX_LOG_COLORS > 2)
      case 2:
        _DrawBitLine2BPP(x0, i+y0, pData, Diff, xsize, pTrans);
        break;
    #endif
    #if (LCD_MAX_LOG_COLORS > 4)
      case 4:
        _DrawBitLine4BPP(x0, i+y0, pData, Diff, xsize, pTrans);
        break;
    #endif
    #if (LCD_MAX_LOG_COLORS > 16)
      case 8:
        _DrawBitLine8BPP(x0, i+y0, pData, xsize, pTrans);
        break;
    #endif
    #if (LCD_BITSPERPIXEL > 8)
      case 16:
        _DrawBitLine16BPP(x0, i+y0, (const U16 *)pData, xsize, pTrans);
        break;
    #endif
    }
    pData += BytesPerLine;
  }
}


/********************************************************
*   													*
*   	LCD_L0_SetOrg   								*
*   													*
*********************************************************
														
Purpose:		Sets the original position of the virtual display.
				Has no function at this point with the PC-driver.
*/

int OrgX, OrgY;
void LCD_L0_SetOrg(int x, int y)
{
	OrgX = x;
	OrgY = y;
}



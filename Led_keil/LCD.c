#include "stdarg.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "ctype.h"
#include "addr.h"


//TFT 240320
#define LCD_XSIZE_TFT_240320 	(320)	
#define LCD_YSIZE_TFT_240320 	(240)

#define SCR_XSIZE_TFT_240320 	(320)
#define SCR_YSIZE_TFT_240320 	(240)
extern unsigned char xyx_320_240[];	//宽240，高320
extern unsigned char bagua[];
/*------------------------------------------------------------/
函数名称：	LCDmain
功能描述：	LCD主程序，入口程序
传    参：	无
返 回 值：	无
-------------------------------------------------------------*/
void LCDmain(void)
{
 
	 Uart_Init();
	 Uart_Printf("\nTest 320*240 TFT LCD!\n");
	 Lcd_Port_Init();
	 Lcd_Init();
	 Lcd_PowerEnable(0, 1);
	 Lcd_EnvidOnOff(1);
	 Lcd_ClearScr(0xf00f);

	 //Lcd_ClearScr( (0x00<<11) | (0x00<<5) | (0x00) );  
	 Paint_Bmp(0, 0, 320, 240, bagua);
	 Delay(100);
	// Glib_Rectangle(50,50,150,150,0x11111);
	// Delay(100);
	// Glib_FilledRectangle(50,50,150,150,0x11111);
	  Uart_Printf("\nLCD Test BMP\n");
	 Uart_Printf("\nLCD Test OVER\n"); 
	  while(1);
     }
 
/*------------------------------------------------------------/
函数名称：	Lcd_PowerEnable
功能描述：	LCD电源管理
传    参：	int invpwren,int pwren
返 回 值：	无
-------------------------------------------------------------*/ 
static void Lcd_PowerEnable(int invpwren,int pwren)
{
    //GPG4 is setted as LCD_PWREN
    rGPGUP=rGPGUP&(~(1<<4))|(1<<4); 			// Pull-up disable
    rGPGCON=rGPGCON&(~(3<<8))|(3<<8); 			//GPG4=LCD_PWREN
    //Enable LCD POWER ENABLE Function
    rLCDCON5=rLCDCON5&(~(1<<3))|(pwren<<3);   	// PWREN
    rLCDCON5=rLCDCON5&(~(1<<5))|(invpwren<<5);	// INVPWREN
}

/*------------------------------------------------------------/
函数名称：	Lcd_Port_Init
功能描述：	LCD端口初始化
传    参：	无
返 回 值：	无
-------------------------------------------------------------*/ 
void Lcd_Port_Init(void)
{
	 rGPCUP=0x0;
	 rGPCCON=0xaaaa56a9;
	 rGPDUP=0x0;
	 rGPDCON=0xaaaaaaaa;
}

/*------------------------------------------------------------/
函数名称：	Lcd_Init
功能描述：	LCD控制器初始化
传    参：	无
返 回 值：	无
-------------------------------------------------------------*/ 
void Lcd_Init(void)
{ 
	rGPCUP=0xffffffff; 	// Disable Pull-up register
    rGPCCON=0xaaaa56a9; //Initialize VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND 

    rGPDUP=0xffffffff; 	// Disable Pull-up register
    rGPDCON=0xaaaaaaaa; //Initialize VD[15:8]

	rLCDCON1=(CLKVAL_TFT_320240<<8)|(MVAL_USED<<7)|(3<<5)|(12<<1)|0;
    // TFT LCD panel,12bpp TFT,ENVID=off
	rLCDCON2=(VBPD_320240<<24)|(LINEVAL_TFT_320240<<14)|(VFPD_320240<<6)|(VSPW_320240);
	rLCDCON3=(HBPD_320240<<19)|(HOZVAL_TFT_320240<<8)|(HFPD_320240);
	rLCDCON4=(MVAL<<8)|(HSPW_320240);
	rLCDCON5=(1<<11)|(1<<9)|(1<<8)|(1<<3)|(BSWP<<1)|(HWSWP);
	//rLCDCON5=(1<<11)|(0<<9)|(0<<8)|(0<<6)|(BSWP<<1)|(HWSWP);	
	//FRM5:6:5,HSYNC and VSYNC are inverted

	rLCDSADDR1=(((U32)LCD_BUFFER>>22)<<21)|M5D((U32)LCD_BUFFER>>1);
	rLCDSADDR2=M5D( ((U32)LCD_BUFFER+(SCR_XSIZE_TFT_320240*LCD_YSIZE_TFT_320240*2))>>1 );
	rLCDSADDR3=(((SCR_XSIZE_TFT_320240-LCD_XSIZE_TFT_320240)/1)<<11)|(LCD_XSIZE_TFT_320240/1);
	rLCDINTMSK|=(3); 		// MASK LCD Sub Interrupt
    //rTCONSEL|=((1<<4)|1); // Disable LCC3600, LPC3600
	rTPAL=0; 				// Disable Temp Palette
}

/*------------------------------------------------------------/
函数名称：	Lcd_EnvidOnOff
功能描述：	LCD开关函数
传    参：	int onoff
返 回 值：	无
-------------------------------------------------------------*/ 
void Lcd_EnvidOnOff(int onoff)
{
	if(onoff==1)
		rLCDCON1|=1;
	else 
		rLCDCON1=rLCDCON1&0x3fffe;
}

/*------------------------------------------------------------/
函数名称：	Lcd_ClearScr
功能描述：	LCD清屏函数
传    参：	U32 c
返 回 值：	无
-------------------------------------------------------------*/ 
void Lcd_ClearScr(U32 c)
{
	void Pixel(U32 x,U32 y, U32 c );
	unsigned int x,y;

	for(y=0;y<SCR_YSIZE_TFT_320240;y++)
	{
		for(x=0;x<SCR_XSIZE_TFT_320240;x++)
	    {
	    //LCD_BUFFER[y][x]=c;
		 Pixel( x,y,c );
		}
	}
}

void Pixel(U32 x,U32 y, U32 c )
{
	if ( (x < SCR_XSIZE_TFT_320240) && (y < SCR_YSIZE_TFT_320240) )
	LCD_BUFFER[y][x] = c;
}

/*------------------------------------------------------------/
函数名称：	Glib_Line
功能描述：	LCD图形显示函数
传    参：	int x1,int y1,int x2,int y2,int color
返 回 值：	无
-------------------------------------------------------------*/ 
static void Glib_Line(int x1,int y1,int x2,int y2,int color)
{
	int dx,dy,e;
	dx=x2-x1; 
	dy=y2-y1;
    
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					Pixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					Pixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					Pixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					Pixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					Pixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					Pixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					Pixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					Pixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
}

/*------------------------------------------------------------/
函数名称：	Glib_Rectangle
功能描述：	在LCD屏幕上画一个矩形
传    参：	int x1,int y1,int x2,int y2,int color
返 回 值：	无
-------------------------------------------------------------*/ 
static void Glib_Rectangle(int x1,int y1,int x2,int y2,int color)
{
    Glib_Line(x1,y1,x2,y1,color);
    Glib_Line(x2,y1,x2,y2,color);
    Glib_Line(x1,y2,x2,y2,color);
    Glib_Line(x1,y1,x1,y2,color);
}

/*------------------------------------------------------------/
函数名称：	Glib_FilledRectangle
功能描述：	在LCD屏幕上用颜色填充一个矩形
传    参：	int x1,int y1,int x2,int y2,int color
返 回 值：	无
-------------------------------------------------------------*/ 
static void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color)
{
    int i;

    for(i=y1;i<=y2;i++)
	Glib_Line(x1,i,x2,i,color);
}

/**************************************************************
在LCD屏幕上指定坐标点画一个指定大小的图片
**************************************************************/
void Paint_Bmp(int x0,int y0,int h,int l,unsigned char bmp[])
{
	int x,y;
	U32 c;
	int p = 0;
	
    for( y = y0 ; y < l ; y++ )
    {
    	for( x = x0 ; x < h ; x++ )
    	{
    		c = bmp[p+1] | (bmp[p]<<8) ;

			if ( ( (x0+x) < SCR_XSIZE_TFT_240320) && ( (y0+y) < SCR_YSIZE_TFT_240320) )
				LCD_BUFFER[y0+y][x0+x] = c ;
			
    		p = p + 2 ;
    	}
    }
}

 
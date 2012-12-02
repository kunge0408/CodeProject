#define rUFCON0 (*(volatile unsigned *)0x50000008)
#define rULCON0 (*(volatile unsigned *)0x50000000)
#define rUCON0 (*(volatile unsigned *)0x50000004)
#define rUBRDIV0 (*(volatile unsigned *)0x50000028)
#define rUTRSTAT0 (*(volatile unsigned *)0x50000010)
#define rUTXH0 (*(volatile unsigned *)0x50000020)
#define rURXH0 (*(volatile unsigned *)0x50000024)
#define rUMCON0 (*(volatile unsigned *)0x5000000c)

#define rGPCCON    (*(volatile unsigned *)0x56000020)
#define rGPCUP     (*(volatile unsigned *)0x56000028)
#define rGPDCON    (*(volatile unsigned *)0x56000030)
#define rGPDUP     (*(volatile unsigned *)0x56000038)

#define rLCDCON1    (*(volatile unsigned *)0x4d000000)	
#define rLCDCON2    (*(volatile unsigned *)0x4d000004)	
#define rLCDCON3    (*(volatile unsigned *)0x4d000008)	
#define rLCDCON4    (*(volatile unsigned *)0x4d00000c)	
#define rLCDCON5    (*(volatile unsigned *)0x4d000010)	
#define rLCDSADDR1  (*(volatile unsigned *)0x4d000014)	
#define rLCDSADDR2  (*(volatile unsigned *)0x4d000018)
#define rLCDSADDR3  (*(volatile unsigned *)0x4d00001c)
#define rLCDINTMSK  (*(volatile unsigned *)0x4d00005c)
#define rTPAL       (*(volatile unsigned *)0x4d000050)

#define rGPGCON    (*(volatile unsigned *)0x56000060)	//Port G control
#define rGPGDAT    (*(volatile unsigned *)0x56000064)	//Port G data
#define rGPGUP     (*(volatile unsigned *)0x56000068)	//Pull-up control G

#define U32 unsigned int
#define U16 unsigned short
#define U8  unsigned char

#define HOZVAL_TFT_320240	(LCD_XSIZE_TFT_320240-1)//分辨率
#define LINEVAL_TFT_320240	(LCD_YSIZE_TFT_320240-1)
#define LCD_XSIZE_TFT_320240 	(320)
#define LCD_YSIZE_TFT_320240 	(240)
#define SCR_XSIZE_TFT_320240 	(320)
#define SCR_YSIZE_TFT_320240 	(240)
#define M5D(n) ((n) & 0x1fffff)

#define MVAL		(13)
#define MVAL_USED 	(0)		//0=each frame   1=rate by MVAL
#define INVVDEN		(1)		//0=normal       1=inverted
#define BSWP		(0)		//Byte swap control
#define HWSWP		(1)		//Half word swap control
#define VBPD_320240		(3)		//垂直同步信号的后肩
#define VFPD_320240		(5)		//垂直同步信号的前肩
#define VSPW_320240		(15)		//垂直同步信号的脉宽

#define HBPD_320240		(58)		//水平同步信号的后肩
#define HFPD_320240		(15)		//水平同步信号的前肩
#define HSPW_320240		(8)		//水平同步信号的脉宽

#define CLKVAL_TFT_320240	(10) //	




void Uart_Init(void);
void Uart_SendByte(int data);
void Uart_SendString(char *pt);
void Uart_Printf(char *fmt,...);

static void Lcd_PowerEnable(int invpwren,int pwren); 
void Lcd_Port_Init(void);
void Lcd_Init(void);
void Lcd_EnvidOnOff(int onoff);
void Lcd_ClearScr(U32 c);
void Pixel(U32 x,U32 y, U32 c );
void Delay(unsigned int x);

static void Glib_Line(int x1,int y1,int x2,int y2,int color);
static void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color);
static void Glib_Rectangle(int x1,int y1,int x2,int y2,int color);

volatile static unsigned short LCD_BUFFER[240][320];
 void LCDmain(void);
void Paint_Bmp(int x0,int y0,int h,int l,unsigned char bmp[]);

/**************************************************************
The initial and control for 240¡Á320 16Bpp TFT LCD----LtS350Q1
**************************************************************/

void Lcd_Tft_LTV350QV_F05_Init(void);
void Lcd_LTV350QV_F05_Init(void);

void Lcd_Tft_LTV350QV_F05_Test( void ) ;
void Glib_Line(U32 x1,U32 y1,U32 x2,U32 y2,U16 color);
void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color);
void PutPixel(U32 x,U32 y, U32 c );
void Glib_Circle(U32 x0, U32 y0, U32 r, U16 color);
void LCD2440_SetPixel(U32 x,U32 y,U32 c);
U32 LCD2440_GetPixeIndexl(U32 x,U32 y,U32 c);
void Glib_Line(U32 x1,U32 y1,U32 x2,U32 y2,U16 color);
void Lcd_EnvidOnOff(int onoff);

U32 GetPixelVal(U32 x,U32 y);

void Lcd_PutASCII(unsigned int x,unsigned int y,unsigned char ch,unsigned int c,unsigned int bk_c,unsigned int st);
void Lcd_printf(unsigned int x,unsigned int y,unsigned int c,unsigned int bk_c,unsigned int st,char *fmt,...);
void Lcd_PutHZ(unsigned int x,unsigned int y,unsigned short int QW,unsigned int c,unsigned int bk_c,unsigned int st);





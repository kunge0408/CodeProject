/*
*********************************************************************************************************
* .
*********************************************************************************************************
*/
#define   Global_Def 
#include  "Includes.H"



// 开/关 LCD 及背光.
#define  LCD_ON()         Lcd_EnvidOnOff(1);
#define  LCD_OFF()        Lcd_EnvidOnOff(0)



extern void Lcd_EnvidOnOff(int onoff);


/*
********************************************************************************************************
* 函数: void s3c2410_LCD_Init (void).
* 描述: 初始化 LCD 控制器.
********************************************************************************************************
*/
void s3c2440_LCD_Init (void)
{
    /*LCD_OFF();
    rLCDCON1 = ((LC1_CLKVAL<<8)+(LC1_MMODE<<7)+(LC1_PNRMODE<<5)+(LC1_BPPMODE<<1)+(0<<0));
    rLCDCON2 = ((LC2_VBPD<<24)+(LC2_LINEVAL<<14)+(LC2_VFPD<<6)+(LC2_VSPW<<0));
    rLCDCON3 = ((LC3_HBPD<<19)+(LC3_HOZVAL<<8)+(LC3_HFPD<<0));
    rLCDCON4 = ((LC4_MVAL<<8)+(LC4_HSPW<<0));

    rLCDCON5 =  (LC5_BPP24BL<<12)+(LC5_FRM565<<11)+ \
                (LC5_INVVCLK<<10)+(LC5_INVVLINE<<9)+(LC5_INVVFRAME<<8)+(LC5_INVVD<<7)+ \
                (LC5_INVVDEN<<6)+(LC5_INVPWREN<<5)+(LC5_INVLEND<<4)+(LC5_PWREN<<3)+ \
                (LC5_ENLEND<<2)+(LC5_BSWP<<1)+(LC5_HWSWP<<0);

    rLCDSADDR1 = ((LCDBANK<<21)+(LCDBASEU<<0));
    rLCDSADDR2 = (LCDBASEL<<0);
    rLCDSADDR3 = ((OFFSIZE<<11)+(PAGEWIDTH<<0));

    rLCDINTMSK|= (3);
    rLPCSEL    = 0;
    rTPAL      = 0;

    LCD_ON();
   */ 
   
	
}

/*
********************************************************************************************************
* End.
********************************************************************************************************
*/

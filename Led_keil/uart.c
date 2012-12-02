//***************************[ UART ]******************************
#include "addr.h"
#include <stdarg.h>
#include <stdio.h>
void Uart_Init(void)
{
    int i;
   rULCON0&=0xffffff00;
   rULCON0|=0x03;
   rUCON0=0x0805;
   rUBRDIV0=0x1A;
   rUFCON0 = 0x0;   //UART channel 0 FIFO control register, FIFO disable
   rUMCON0 = 0x0;   //UART chaneel 0 MODEM control register, AFC disable
   
    
    for(i=0;i<100;i++);
}


void Uart_SendByte(int data)
{
	if(data=='\n')
    {
         while(!(rUTRSTAT0 & 0x2));
           // Delay(1);                 //because the slow response of hyper_terminal 
         rUTXH0='\r';          //'\r'表示回车
    }
    while(!(rUTRSTAT0 & 0x2));   //Wait until THR is empty.
      //  Delay(1);
   rUTXH0=data;
    
         
}               

//====================================================================
void Uart_SendString(char *pt)
{
    while(*pt)
        Uart_SendByte(*pt++);
}
void Delay(unsigned int x)        
{ 
	unsigned int i,j,k; 
	for(i=0;i<=x;i++) 
		for(j=0;j<0xff;j++)
			for(k=0;k<0xff;k++);
}
//=====================================================================
//If you don't use vsprintf(), the code size is reduced very much.
void Uart_Printf(char *fmt,...)
{
    va_list ap;
    char string[256];

    va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	Uart_SendString(string);
	va_end(ap);
} 








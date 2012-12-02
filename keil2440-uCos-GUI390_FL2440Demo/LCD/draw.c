
#include "draw.h"


#include    <stdlib.h>
#include    <math.h> 

#include "LCD_LTV350QV_F05.h"

#define   GUI_CopyColor(color1,color2)      (*color1=color2)



/* 定义前景色及背景色变量，用于ASCII码、汉字、窗口、单色位图显示 */
TCOLOR  disp_color=RED;
TCOLOR	back_color=GREEN;



/* 定义十进制(0-7)==>十六进制位转换表，由于显示点数据是由左到右，所以十六进制位顺序是倒的 */
U8 const  DCB2HEX_TAB[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

/* 定义16标准色 */
U16  const  COLORS_TAB[16] = 
{
   BLACK,NAVY,DGREEN,DCYAN,MAROON,PURPLE, OLIVE, LGRAY,	
   DGRAY,BLUE,GREEN, CYAN, RED,   MAGENTA,YELLOW,WHITE		  					   
}; 					     
							     	
							     

/****************************************************************************
* 名称：DelayNS()
* 功能：长软件延时
* 入口参数：dly		延时参数，值越大，延时越久
* 出口参数：无
****************************************************************************/
void  DelayNS(U32 dly)
{  
   U32  i;

   for(; dly>0; dly--) 
      for(i=0; i<2000; i++);
}                              


/****************************************************************************
* 名称：random()
* 功能：取随机数。
* 入口参数：seed   随机数范围(0 -- seed-1)
* 出口参数：返回值即是取得的随机数
****************************************************************************/
U16  random(U16 seed)
{  
   U16 temp;    
   
   temp = rand();
   temp = temp % seed;     
   
   return(temp);
}

/****************************************************************************
* 名称：CLineDemo()
* 功能：彩色线演示程序。以中心点为准，不断的画各种随机角度、随机长度、随机颜色画直线。
* 入口参数：无
* 出口参数：无
****************************************************************************/
void  CLineDemo(void)
{  
   U32  x0, y0;
   U32  x1, y1;
   U16  color;   
   U32  no=0;  
   
   for(no=0; no<500; no++)
   {  /* 取第一个点坐标 */
      x0 = random(320);
      y0 = random(240);
      color = random(15)+1;     // 不使用黑色(背景色)
      
      /* 计算出中心对称点 */
      x1 = 2*CENTER_X - x0;
      y1 = 2*CENTER_Y - y0;
      
      Glib_Line(x0, y0, x1, y1, COLORS_TAB[color]);
      DelayNS(200);
   }   

}


/****************************************************************************
* 名称：CLineDemo()
* 功能：彩色矩形演示程序。取得随机起点及长、宽，然后画填充矩形
* 入口参数：无
* 出口参数：无
****************************************************************************/
void  CRectangleDemo(void)
{  U32  x0, y0;
   U32  x1, y1;
   U16  color;
   U32  no;

   for(no=0; no<300; no++)
   {  /* 取第一个点坐标 */
      x0 = random(320);
      y0 = random(240);
      color = random(15)+1;     // 不使用黑色(背景色)
      
      /* 取第二点坐标 */
      x1 = x0 + random(150);							// 长控制在150像素以内
      y1 = y0 + random(100);							// 宽控制在100像素以内
      
      Glib_FilledRectangle(x0, y0, x1, y1, COLORS_TAB[color]);
      DelayNS(100);
   }
}


/****************************************************************************
* 名称：CCircleDemo()
* 功能：彩色画填充圆演示程序。
* 入口参数：无
* 出口参数：无
****************************************************************************/
void  CCircleDemo(void)
{  
   U32  x0, y0;
   U8   radius;
   U16  color;
   U32  no;
  
   for(no=0; no<300; no++)
   {  /* 取第一个点坐标 */
      x0 = random(320);
      y0 = random(240);
      color = random(16);
      radius = random(50);							
      
      Glib_Circle(x0, y0, radius, COLORS_TAB[color]);
      DelayNS(100);
   }

}




/****************************************************************************
* 名称：GUI_LoadLine()
* 功能：输出单色图形的一行数据。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的数据。
*           no      要显示此行的点个数
* 出口参数：返回值为1时表示操作成功，为0时表示操作失败。
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
U8  GUI_LoadLine(U32 x, U32 y, U8 *dat, U32 no)
{  
   U8   bit_dat=0;
   U8   i;
   TCOLOR  bakc;

   /* 参数过滤 */
   if(x>=GUI_LCM_XMAX) return(0);
   if(y>=GUI_LCM_YMAX) return(0);
   
   for(i=0; i<no; i++)
   {  /* 判断是否要读取点阵数据 */
      if( (i%8)==0 ) bit_dat = *dat++;
     
      /* 设置相应的点为color或为back_color */
      if( (bit_dat&DCB2HEX_TAB[i&0x07])==0 ) 
         GUI_CopyColor(&bakc, back_color); 
      else  
        GUI_CopyColor(&bakc, disp_color);
      
      PutPixel(x, y, bakc);           
     
      if( (++x)>=GUI_LCM_XMAX ) 
        return(0);
   }
   
   return(1);
}




/****************************************************************************
* 名称：GUI_PutHZ()
* 功能：显示汉字。
* 入口参数： x		指定显示位置，x坐标
*           y		指定显示位置，y坐标
*           dat		要输出显示的汉字点阵数据。
*           hno     要显示此行的点个数
*           lno     要显示此列的点个数
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_PutHZ(U32 x, U32 y, U8 *dat, U8 hno, U8 lno)
{  
   U8  i;

   for(i=0; i<lno; i++)
   {  GUI_LoadLine(x, y, dat, hno);						// 输出一行数据
      y++;												// 显示下一行
      dat += (hno>>3);									// 计算下一行的数据
      if( (hno&0x07)!=0 ) dat++;
   }
}


/****************************************************************************
* 名称：GUI_SetColor()
* 功能：设置显示色及背景色。用于ASCII字符显示及汉字显示。
* 入口参数：color1	显示色的值
*          color2	背景色的值
* 出口参数：无
* 说明：
****************************************************************************/
void  GUI_SetColor2(TCOLOR color1, TCOLOR color2)
{  GUI_CopyColor(&disp_color, color1);
   GUI_CopyColor(&back_color, color2);  
}

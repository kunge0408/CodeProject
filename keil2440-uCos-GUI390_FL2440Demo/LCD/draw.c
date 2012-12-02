
#include "draw.h"


#include    <stdlib.h>
#include    <math.h> 

#include "LCD_LTV350QV_F05.h"

#define   GUI_CopyColor(color1,color2)      (*color1=color2)



/* ����ǰ��ɫ������ɫ����������ASCII�롢���֡����ڡ���ɫλͼ��ʾ */
TCOLOR  disp_color=RED;
TCOLOR	back_color=GREEN;



/* ����ʮ����(0-7)==>ʮ������λת������������ʾ�������������ң�����ʮ������λ˳���ǵ��� */
U8 const  DCB2HEX_TAB[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

/* ����16��׼ɫ */
U16  const  COLORS_TAB[16] = 
{
   BLACK,NAVY,DGREEN,DCYAN,MAROON,PURPLE, OLIVE, LGRAY,	
   DGRAY,BLUE,GREEN, CYAN, RED,   MAGENTA,YELLOW,WHITE		  					   
}; 					     
							     	
							     

/****************************************************************************
* ���ƣ�DelayNS()
* ���ܣ���������ʱ
* ��ڲ�����dly		��ʱ������ֵԽ����ʱԽ��
* ���ڲ�������
****************************************************************************/
void  DelayNS(U32 dly)
{  
   U32  i;

   for(; dly>0; dly--) 
      for(i=0; i<2000; i++);
}                              


/****************************************************************************
* ���ƣ�random()
* ���ܣ�ȡ�������
* ��ڲ�����seed   �������Χ(0 -- seed-1)
* ���ڲ���������ֵ����ȡ�õ������
****************************************************************************/
U16  random(U16 seed)
{  
   U16 temp;    
   
   temp = rand();
   temp = temp % seed;     
   
   return(temp);
}

/****************************************************************************
* ���ƣ�CLineDemo()
* ���ܣ���ɫ����ʾ���������ĵ�Ϊ׼�����ϵĻ���������Ƕȡ�������ȡ������ɫ��ֱ�ߡ�
* ��ڲ�������
* ���ڲ�������
****************************************************************************/
void  CLineDemo(void)
{  
   U32  x0, y0;
   U32  x1, y1;
   U16  color;   
   U32  no=0;  
   
   for(no=0; no<500; no++)
   {  /* ȡ��һ�������� */
      x0 = random(320);
      y0 = random(240);
      color = random(15)+1;     // ��ʹ�ú�ɫ(����ɫ)
      
      /* ��������ĶԳƵ� */
      x1 = 2*CENTER_X - x0;
      y1 = 2*CENTER_Y - y0;
      
      Glib_Line(x0, y0, x1, y1, COLORS_TAB[color]);
      DelayNS(200);
   }   

}


/****************************************************************************
* ���ƣ�CLineDemo()
* ���ܣ���ɫ������ʾ����ȡ�������㼰��������Ȼ��������
* ��ڲ�������
* ���ڲ�������
****************************************************************************/
void  CRectangleDemo(void)
{  U32  x0, y0;
   U32  x1, y1;
   U16  color;
   U32  no;

   for(no=0; no<300; no++)
   {  /* ȡ��һ�������� */
      x0 = random(320);
      y0 = random(240);
      color = random(15)+1;     // ��ʹ�ú�ɫ(����ɫ)
      
      /* ȡ�ڶ������� */
      x1 = x0 + random(150);							// ��������150��������
      y1 = y0 + random(100);							// ��������100��������
      
      Glib_FilledRectangle(x0, y0, x1, y1, COLORS_TAB[color]);
      DelayNS(100);
   }
}


/****************************************************************************
* ���ƣ�CCircleDemo()
* ���ܣ���ɫ�����Բ��ʾ����
* ��ڲ�������
* ���ڲ�������
****************************************************************************/
void  CCircleDemo(void)
{  
   U32  x0, y0;
   U8   radius;
   U16  color;
   U32  no;
  
   for(no=0; no<300; no++)
   {  /* ȡ��һ�������� */
      x0 = random(320);
      y0 = random(240);
      color = random(16);
      radius = random(50);							
      
      Glib_Circle(x0, y0, radius, COLORS_TAB[color]);
      DelayNS(100);
   }

}




/****************************************************************************
* ���ƣ�GUI_LoadLine()
* ���ܣ������ɫͼ�ε�һ�����ݡ�
* ��ڲ����� x		ָ����ʾλ�ã�x����
*           y		ָ����ʾλ�ã�y����
*           dat		Ҫ�����ʾ�����ݡ�
*           no      Ҫ��ʾ���еĵ����
* ���ڲ���������ֵΪ1ʱ��ʾ�����ɹ���Ϊ0ʱ��ʾ����ʧ�ܡ�
* ˵��������ʧ��ԭ����ָ����ַ������Ч��Χ��
****************************************************************************/
U8  GUI_LoadLine(U32 x, U32 y, U8 *dat, U32 no)
{  
   U8   bit_dat=0;
   U8   i;
   TCOLOR  bakc;

   /* �������� */
   if(x>=GUI_LCM_XMAX) return(0);
   if(y>=GUI_LCM_YMAX) return(0);
   
   for(i=0; i<no; i++)
   {  /* �ж��Ƿ�Ҫ��ȡ�������� */
      if( (i%8)==0 ) bit_dat = *dat++;
     
      /* ������Ӧ�ĵ�Ϊcolor��Ϊback_color */
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
* ���ƣ�GUI_PutHZ()
* ���ܣ���ʾ���֡�
* ��ڲ����� x		ָ����ʾλ�ã�x����
*           y		ָ����ʾλ�ã�y����
*           dat		Ҫ�����ʾ�ĺ��ֵ������ݡ�
*           hno     Ҫ��ʾ���еĵ����
*           lno     Ҫ��ʾ���еĵ����
* ���ڲ�������
* ˵��������ʧ��ԭ����ָ����ַ������Ч��Χ��
****************************************************************************/
void  GUI_PutHZ(U32 x, U32 y, U8 *dat, U8 hno, U8 lno)
{  
   U8  i;

   for(i=0; i<lno; i++)
   {  GUI_LoadLine(x, y, dat, hno);						// ���һ������
      y++;												// ��ʾ��һ��
      dat += (hno>>3);									// ������һ�е�����
      if( (hno&0x07)!=0 ) dat++;
   }
}


/****************************************************************************
* ���ƣ�GUI_SetColor()
* ���ܣ�������ʾɫ������ɫ������ASCII�ַ���ʾ��������ʾ��
* ��ڲ�����color1	��ʾɫ��ֵ
*          color2	����ɫ��ֵ
* ���ڲ�������
* ˵����
****************************************************************************/
void  GUI_SetColor2(TCOLOR color1, TCOLOR color2)
{  GUI_CopyColor(&disp_color, color1);
   GUI_CopyColor(&back_color, color2);  
}
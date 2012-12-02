
#include "def1.h"

// 定义颜色数据类型(可以是数据结构)
#define  TCOLOR				U16
   
// 定义LCM像素数宏                 
#define  GUI_LCM_XMAX		320							/* 定义液晶x轴的点数 */
#define  GUI_LCM_YMAX		240							/* 定义液晶y轴的点数 */

// 设置颜色宏定义
#define   BLACK		0x0000      /* 黑色：    0,   0,   0 */
#define   NAVY		0x000F      /* 深蓝色：  0,   0, 128 */
#define   DGREEN	0x03E0		/* 深绿色：  0, 128,   0 */
#define   DCYAN		0x03EF		/* 深青色：  0, 128, 128 */
#define   MAROON	0x7800		/* 深红色：128,   0,   0 */
#define   PURPLE	0x780F		/* 紫色：  128,   0, 128 */
#define   OLIVE		0x7BE0 	    /* 橄榄绿：128, 128,   0 */
#define   LGRAY		0xC618	    /* 灰白色：192, 192, 192 */
#define   DGRAY		0x7BEF		/* 深灰色：128, 128, 128 */
#define   BLUE		0x001F		/* 蓝色：    0,   0, 255 */
#define   GREEN		0x07E0		/* 绿色：    0, 255,   0 */
#define   CYAN	    0x07FF 		/* 青色：    0, 255, 255 */
#define   RED		0xF800		/* 红色：  255,   0,   0 */
#define   MAGENTA	0xF81F		/* 品红：  255,   0, 255 */
#define   YELLOW	0xFFE0		/* 黄色：  255, 255, 0   */
#define   WHITE		0xFFFF      /* 白色：  255, 255, 255 */

#define  CENTER_X	160
#define  CENTER_Y	120




void  GUI_PutHZ(U32 x, U32 y, U8 *dat, U8 hno, U8 lno);
void  GUI_SetColor2(U16 color1, U16 color2);
void  CCircleDemo(void);

U16  random(U16 seed);


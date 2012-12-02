/*******************************************************************
文件名称：LED_test.h
功能说明：测试系统移植效果程序头文件
编写：张锡冰 阿呆的游乐园 河北大学
时间：2010/8/17
修改记录：无
其他说明：版权所有，盗版请注明出处
*******************************************************************/


#ifndef	__LED_TEST_H__
#define	__LED_TEST_H__

#include "2440addr.h"
#include "def.h"

void LED_IO_init(void);			//初始化LED IO

void LED_negate(U8 LedNum);		//根据获得的参数取反相应LED
void Delay(int x);
void LED_flow(void);
#endif

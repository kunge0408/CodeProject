/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：main.c
 * 描述    ：用3.5.0版本建的工程模板。         
 * 实验平台：野火STM32开发板
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/
#include "stm32f10x.h"
#include "uart.h"

char *str = "Hello world\n"; 
/* 
 * 函数名：main
 * 描述  : 主函数
 * 输入  ：无
 * 输出  : 无
 */
int main(void)
{
	  USART_Configuration();
  	  while(1)
	  {
		 USART2_Puts(str);
		// Delay();
	  };
	  // add your code here ^_^。
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/



 /*******************************************************************
 文件名称：App_cfg.h
 功能说明：应用程序配置头文件
 		   阿呆有一个想法就是将应用程序写成可配置性的，个应用模块相对
		   独立，然后通过宏开启或关闭，这样写程序时不用的代码可以直接
		   通过宏关闭，而非一点一点去删除，至少驱动要弄成可配置的，尝
		   试之。
 编写：张锡冰 阿呆的游乐园 河北大学
 	   于保定飞凌嵌入式
 时间：2010/8/15
 其他说明：We now assume the presence of a file called APP_CFG.H 
 		   which is declared in your application. The purpose of this
		   file is to assign task priorities, stack sizes and other 
		   configuration information for your application.(官方说明)
 *******************************************************************/

#ifndef __APP_CFG_H__
#define __APP_CFG_H__

#define LED_TEST_EN		1u		//使能LED_TEST_EN将包含测试驱动文件


#define TASK_STK_SIZE		256

#define OS_TASK_TMR_PRIO	OS_LOWEST_PRIO - 10u

#endif

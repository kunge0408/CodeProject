 /*******************************************************************
 文件名称：App.h
 功能说明：应用程序配置头文件
 编写：张锡冰 阿呆的游乐园 河北大学
 	   于保定飞凌嵌入式
 时间：2010/8/16
 其他说明：包含所有用户程序的头文件，阿呆尝试将驱动文件写成可配置型的
 		   尝试之
 *******************************************************************/

#ifndef APP_H
#define APP_H

#if	LED_TEST_EN >0u	 			//如果应用程序配置LED测试驱动，则包含相关代码到工程
	#include "LED_test.h"
//	#include "lcd.h"
#endif


OS_STK	TaskStartStk[TASK_STK_SIZE];		//声明起始任务堆栈
OS_STK	TaskTestLedStk[4][TASK_STK_SIZE];	//声明LED测试任务堆栈

INT8U	LedNo[4];							//LED参数数组，用于起始任务将LED编号传递给子任务

void AppInit(void);							//用户应用程序初始化

void TaskStart(void *pdata);				//起始任务

void TaskTestLed(void *pdata);				//测试LED任务

#endif


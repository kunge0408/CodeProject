#ifndef APP_CFG_H
#define APP_CFG_H



#define	 OS_TASK_TMR_PRIO       27


//Task Stk lengh

#define	MainTaskStkLengh	  512   	// Define the MainTask stack length 
#define	TouchTaskStkLengh   512     // Define the TouchTask stack length 
#define	Task1StkLengh		    512   	// Define the Task1 stack length 
#define	LCDfreshStkLengh	  512   	// Define the Task_LCDfresh stack length 
#define	TaskUartStkLengh	  512    // Define the TaskUart stack length 



//Task function
void  MainTask(void *pdata);   		//MainTask task
void 	TouchTask(void *pdata);			// TouchTask 
void 	Task1(void *pdata);			    // Task1 
void 	Task_LCDfresh(void *pdata);	// Task_LCDfresh
void  TaskUart(void *pdata);     //Task Uart


//Task Prio
#define NormalTaskPrio       0

#define MainTaskPrio 	  NormalTaskPrio

#define TaskUartPrio  	NormalTaskPrio+1
#define Task1Prio  		  NormalTaskPrio+2

#define TouchTaskPrio 	NormalTaskPrio+3
#define LCDfreshPrio    NormalTaskPrio+4



#endif


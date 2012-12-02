#ifndef APP_CFG_H
#define APP_CFG_H



//#define	 OS_TASK_TMR_PRIO       27


//Task Stk lengh

#define	MainTaskStkLengh	512   	// Define the MainTask stack length 
#define	Task0StkLengh		  512     // Define the Task0 stack length 
#define	Task1StkLengh		  512   	// Define the Task1 stack length 
#define	Task2StkLengh		  512   	// Define the Task2 stack length 
#define	TaskUartStkLengh	512    // Define the TaskUart stack length 



//Task function
void    MainTask(void *pdata);   		//MainTask task
void 	  Task0(void *pdata);			    // Task0 
void 	  Task1(void *pdata);			    // Task1 
void 	  Task2(void *pdata);			    // Task2
void    TaskUart(void *pdata);          //Task Uart


//Task Prio
#define NormalTaskPrio       0
#define MainTaskPrio 	NormalTaskPrio
#define Task0Prio 		NormalTaskPrio+1
#define Task1Prio  		NormalTaskPrio+2
#define Task2Prio  		NormalTaskPrio+3
#define TaskUartPrio  	NormalTaskPrio+4


#endif



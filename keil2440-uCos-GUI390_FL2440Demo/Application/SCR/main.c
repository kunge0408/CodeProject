
#include "config.h"


//=======================================
#include "app_cfg.h"
#include "Printf.h"
//=========================================
#include "DIALOG.h"
#include "GUIDEMO.H" 
#include "GUI_X.H"	
#include "string.h"




#define  TASK_STK_SIZE      256                /* Size of each task's stacks (# of WORDs) */
#define  NO_TASKS           5                  /* Number of identical tasks */


OS_STK  MainTaskStk[MainTaskStkLengh];				// Define the MainTask stack 
OS_STK	TouchTaskStk [TouchTaskStkLengh];     // Define the TouchTask stack 
OS_STK	Task1Stk [Task1StkLengh];             // Define the Task1 stack 
OS_STK	LCDfreshStk [LCDfreshStkLengh];       // Define LCDfresh stack 

int rYear, rMonth,rDay,rDayOfWeek,rHour,rMinute,rSecond;

OS_STK TaskStk[NO_TASKS][TASK_STK_SIZE];      /* Tasks stacks */

char   TaskData[NO_TASKS];                    /* Parameters to pass to each task */



const unsigned char ledtab[]={0xf7,0xef,0xdf,0xbf,0x7f,0xff};

volatile int xdata=-1, ydata=-1;

int flag_touch=0;




OS_EVENT *TouchMbox; 

OS_EVENT *RandomSem;


U8 err;

extern  int _KeyMsgCnt;

void Rtc_Init(void);

void Task_LCD(void *p);

void Touch_Screen_Init(void);
void Task (void *data);


extern void CalibrateTask(void* pdata);//触摸屏校准任务入口		
   



/**********************************************************
**   注意：在ucos-ii内部中断处理函数不要使用_irq限定符
**
***********************************************************/

void  Timer4intdone(void) 
{	    
    static  int tmr4_cnt=0;
    
    static int led_cnt=0;  
  
    rSRCPND = BIT_TIMER4;                                     
    rINTPND = BIT_TIMER4;	    
    
    
    tmr4_cnt++;
    
    if(tmr4_cnt>=100) 
    {
      tmr4_cnt=0;
      
      led_cnt++;
      
      if(led_cnt>5)  
			   led_cnt=0;  

			rGPFDAT=ledtab[led_cnt]; 	 
				     
    } 
		  
    
    
}

static void init_autorun_timer(int sec)
{
	U32 val = (PCLK>>4)/1000-1; //定时1ms	
	
	
	pISR_TIMER4 = (U32)Timer4intdone;
	
	rTCFG0 &= ~(0xff<<8);
	rTCFG0 |= 3<<8;			//prescaler = 3+1
	rTCFG1 &= ~(0xf<<16);
	rTCFG1 |= 1<<16;		//mux = 1/4

	rTCNTB4 = val;
	rTCON &= ~(0xf<<20);
	rTCON |= 7<<20;			//interval, inv-off, update TCNTB4&TCMPB4, start timer 4
	rTCON &= ~(2<<20);		//clear manual update bit
	EnableIrq(BIT_TIMER4);
}	



extern void Button_DemoTask(void); 


int main(void)
{	 		
		TargetInit();		
		Lcd_Tft_LTV350QV_F05_Init();	  
			
	  OSInit ();    
	  OSTimeSet(0);	     	
	   
	  OSTaskCreate (MainTask,(void *)0, &MainTaskStk[MainTaskStkLengh - 1], MainTaskPrio);									
		OSStart ();				
		return 0;
}	


void MainTask(void *pdata) //Main Task 
{	 
    INT16U i;	    
    U16 RunCnt=0;	   
   
   #if OS_CRITICAL_METHOD == 3    //Allocate storage for CPU status register 
    OS_CPU_SR  cpu_sr;
   #endif
    OS_ENTER_CRITICAL();   
  		
	  Timer0Init();//initial timer0 for ucos time tick
	  init_autorun_timer(10);		
	  ISRInit();   //initial interrupt prio or enable or disable	
	
	  OS_EXIT_CRITICAL();		
	
	  Touch_Screen_Init();	
			
    GUI_Init();    
   
    GUI_SetColor(GUI_RED);
    GUI_SetFont(&GUI_FontComic24B_1);
    GUI_DispStringAt("uC/GUI Starting!" , 50, 20 ) ;   
    
    GUI_SetColor(GUI_GREEN);
    GUI_SetFont(&GUI_FontComic24B_1);
    GUI_DispStringAt("uC/GUI Starting!" , 50, 70 ) ;     
    
    GUI_SetColor(GUI_WHITE);     
      
    for(i=0;i<50;i+=5)
    {
      GUI_DrawHLine(100+i,20,300); 
    }
    
    GUI_SetColor(GUI_YELLOW);
    
    GUI_DrawVLine(150,150,230);
    
    GUI_FillRect(160,150,300,230);
           
    GUI_SetColor(GUI_RED); 
    
    GUI_DrawCircle(100,200,30);
    
    OSTimeDly(3*OS_TICKS_PER_SEC);
    
    GUI_SetBkColor(GUI_BLUE); 
    
    GUI_Clear(); 
    
    srand(1245);	 
		 
	
		TouchMbox=OSMboxCreate(0);
		
		RandomSem = OSSemCreate(1);    // Random number semaphore    
		
		OSPrintfInit();//use task to print massage to Uart 
		
		OSStatInit();
		
		OSTaskCreate (TouchTask,(void *)0, &TouchTaskStk[TouchTaskStkLengh - 1], TouchTaskPrio);	
		OSTaskCreate (Task1,(void *)0, &Task1Stk[Task1StkLengh - 1], Task1Prio);	
		OSTaskCreate (Task_LCDfresh,(void *)0, &LCDfreshStk[LCDfreshStkLengh - 1], 26);	
	
	
    for (i = 0; i < NO_TASKS; i++)   //Create NO_TASKS identical tasks
     {                                  
        TaskData[i] = '0' + i;      //Each task will display its own letter
                
        OSTaskCreate(Task, (void *)&TaskData[i], (void *)&TaskStk[i][TASK_STK_SIZE - 1], i + 10);
     }		
		 
	while(1)
	 {  			
		    OSPrintf("\nEnter Main Task RunCnt=%d\n",RunCnt);
				  
				RunCnt++;					
				  
				GUIDEMO_main();						
				  
				OSTimeDly(OS_TICKS_PER_SEC/10);
	 	 
	 }

}



void Task (void *data)
{
    U8 err;

    while(1)
    {
         OSSemPend(RandomSem, 0, &err);              
        
	       OSPrintf("Task %d running !\n",*(char *)data); 
	         
	       OSSemPost(RandomSem);
           
         OSTimeDly(OS_TICKS_PER_SEC*2); 
    }
}  



void TouchTask	(void *pdata)
{	  
	
	 pdata=pdata;
	
	
	 while (1)
	  {	 	    
	     
	     GUI_TOUCH_Exec(); 	
	    
       OSTimeDly(OS_TICKS_PER_SEC/100);       

	   }
}

void Task1	(void *pdata) //task for test
{ 	
	U16 TestCnt=0;

	static U16 Version;		

	Version=OSVersion();//ucos version 	
	
			
	while (1)
	{
	
	   TestCnt++;

     OSPrintf("********************************\n");
     OSPrintf("Enter Task1 and runing Cnt=%d\n",TestCnt);	     
     OSPrintf("uC/OS Version:V%4.2f\n",(FP32)Version*0.01);
     OSPrintf("********************************\n"); 		 
		 	
     OSTimeDly(OS_TICKS_PER_SEC);				 

	}
}



#define RGB(r,g,b)   (unsigned int)( (r << 16) + (g << 8) + b )
#define FROM_BCD(n)		((((n) >> 4) * 10) + ((n) & 0xf))
#define TO_BCD(n)		((((n) / 10) << 4) | ((n) % 10))

void Task_LCDfresh(void *pdata)
{
           
    OSPrintf("Task LCD Running...\r\n"); 
    
    Rtc_Init();		

    while(1)
    {	 
       GUI_Exec();       //完成屏幕刷新
       
       GUI_X_ExecIdle(); //空闲任务	
	
    }
}

//************************[ Rtc_Init ]*********************************
static void Rtc_Init(void)
{
	 int wYear, wMonth,wDay,wDayOfWeek,wHour,wMinute,wSecond;

    wYear = 2008;
    wMonth = 9;
    wDay = 5;
    wDayOfWeek = 5;
    wHour= 9;
    wMinute = 41;
    wSecond = 30;
	
	  rRTCCON = 1 ;		//RTC read and write enable

	  rBCDYEAR = (unsigned char)TO_BCD(wYear%100);//年
    rBCDMON  = (unsigned char)TO_BCD(wMonth);		//月
    rBCDDAY	 = (unsigned char)TO_BCD(wDay);			//日	
	  rBCDDATE = wDayOfWeek+1;					        	//星期
	  rBCDHOUR = (unsigned char)TO_BCD(wHour);		//小时
	  rBCDMIN  = (unsigned char)TO_BCD(wMinute);	//分
	  rBCDSEC  = (unsigned char)TO_BCD(wSecond);	//秒
	
	  rRTCCON &= ~1 ;		//RTC read and write disable
}




void  ADC_ISR(void)  
{  	
	
	rINTSUBMSK|=(BIT_SUB_ADC|BIT_SUB_TC);	// Mask sub interrupt (ADC and TC) 
	                                     // TC(Touch screen Control) Interrupt	
 
	if(rADCDAT0 & 0x8000)//抬起
	{
		Uart_Printf("\nStylus Up!!\n");
		rADCTSC&=0xff;	// Set stylus down interrupt
		
		xdata = -1;        
		ydata = -1; //抬起触笔时，xdata,ydata 要值成不大于0的数        
       
	}
	else 
	{
		Uart_Printf("\nStylus Down!!\n");
		// <Auto X-Position and Y-Position Read>
		rADCTSC=(0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(0);
		// Stylus Down,Don't care,Don't care,Don't care,Don't care,XP pullup Dis,Auto,No operation

		rADCCON|=0x1;	// Start Auto conversion

		while(rADCCON & 0x1);		//check if Enable_start is low
		while(!(0x8000&rADCCON));	// Check ECFLG

		xdata=(rADCDAT1&0x3ff);
    ydata=(rADCDAT0&0x3ff);	     
	    
	   
		rADCTSC=(1<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);
		// Stylus Up,Don't care,Don't care,Don't care,Don't care,XP pullup En,Normal,Waiting mode
		
	}		
	
	OSMboxPost(TouchMbox,(void *) 1);//向处理触摸进程发消息
	
	ClearPending(BIT_ADC);	
	ClearSubPending(BIT_SUB_TC);
	 
	
  EnableSubIrq(BIT_SUB_TC);
  EnableIrq(BIT_ADC);    
    
}





void Touch_Screen_Init(void)
{

	rADCDLY=50000; //Normal conversion mode delay about (1/3.6864M)*50000=13.56ms
	              // Enable Prescaler=39,Prescaler A/D converter freq. = 50 MHz/(39+1) = 1.25MHz
	              //Conversion time = 1/(1.25MHz / 5cycles) = 1/250 kHz = 4 us
	              
	   //AIN0,Normal,Disable read start,No operation	          
	             
	rADCCON = (1<<14)|(39<<6)|(0<<3)|(0<<2)|(0<<1)|(0);
	
	   //YM=GND,YP=AIN5,XM=Hi-z,XP=AIN7,XP pullup En,Normal ADC,Waiting for interrupt mode	
	rADCTSC = (0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);	
	
	
	Uart_Printf("OK2440 ADC touch screen\n");
	
	pISR_ADC = (unsigned)ADC_ISR;	
	
	
	ClearSubPending(BIT_SUB_TC);
	ClearSubPending(BIT_SUB_ADC);
	ClearPending(BIT_ADC);
	EnableSubIrq(BIT_SUB_TC);
	EnableIrq(BIT_ADC);
		
}




/**********************����ʵ��*********************************/
// ��˾���� ����������Ƕ��ʽ�������޹�˾
// ��    �� ���������Ʒ�����
// ��    Ȩ ����������Ƕ��ʽ�������޹�˾
// ��    ַ ��www.witech.com.cn
/***************************************************************/

/*
	��ʵ��ӿ�˵��

	GPB5  ------ LED0
	GPB6  ------ LED1
	GPB8  ------ LED2
	GPB10 ------ LED3
*/

/*------------------------��ַ����---------------------------*/
#include "addr.h"
#define GPBCON (*(volatile unsigned *)0x56000010) 
#define GPBDAT (*(volatile unsigned *)0x56000014) 
#define GPBUP (*(volatile unsigned *)0x56000018)

#define uchar unsigned char
#define uint unsigned int

/*-----------------------����ȫ�ֱ���------------------------*/


/*-----------------------��������----------------------------*/
//void Delay(int x);

/*------------------------------------------------------------/
�������ƣ�	Delay
����������	��ʱ����
��    �Σ�	int x
�� �� ֵ��	��
-------------------------------------------------------------*/
/*
void Delay(int x)  
{
 	int k, j;
 	while(x)
 	{
 		for (k=0;k<=0xff;k++)
 			for(j=0;j<=0xff;j++);
 			
 		x--;
 	}
}
*/
/*-------------------------------------------------------------
�������ƣ�	ledMain
����������	��ڳ���
			��ʼ���󣬽�����������ѭ��
��    �Σ�	��
�� �� ֵ��	int 0
-------------------------------------------------------------*/
int ledMain(void)
{	
	GPBCON = 0x1dd7fc;	// GPB5,GPB6,GPB8,GPB10����Ϊ���
	GPBDAT = ((1<<5)|(1<<6)|(1<<8)|(1<<10));	//ʹLEDȫ��
	GPBDAT&=0xffe;		//�رշ�����
	GPBUP = 0x00; 
	LCDmain();
	while (1)			// ��ѭ��
	{
		
		GPBDAT = ~(1<<5);	//LED0��
		Delay(500);
		GPBDAT = ~(1<<6);	//LED1��
		Delay(500);
		GPBDAT = ~(1<<8);	//LED2��
		Delay(500);
		GPBDAT = ~(1<<10);	//LED3��
		Delay(500);
	}

//	return 0;  
}
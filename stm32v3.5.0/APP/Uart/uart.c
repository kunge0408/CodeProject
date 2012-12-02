#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "inc/uart.h"
//////////////////////////////////////////////////////////////////////////
//G var
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
USART_ClockInitTypeDef USART_ClockInitStructure;

//////////////////////////////////////////////////////////////////////////

void USART_Configuration(void)
{
	//使能串口2 PA AFIO总线
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	//PA3 USART2_Rx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//PA2 USART2_Tx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
	USART_ClockInit(USART2,&USART_ClockInitStructure);
	USART_Init(USART2,&USART_InitStructure);

	//Enable the USARTx
	USART_Cmd(USART2,ENABLE);	
}

void USART2_Putc(unsigned char c)
{
	USART_SendData(USART2,c);
	while (USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
}

void USART2_Puts(char *str)
{
	while (*str)
	{
		USART_SendData(USART2,*str++);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
	}
}

void Delay()
{
	int i;
	i=500;
	while(i--);
}


/**************************************************************
 NAME: option.h
 DESC: To measuure the USB download speed, the WDT is used.
       To measure up to large time, The WDT interrupt is used.
 HISTORY:
 Feb.20.2002:Shin, On Pil: Programming start
 Mar.25.2002:purnnamu: S3C2400X profile.c is ported for S3C2440X.
 **************************************************************/
 
#ifndef __OPTION_H__
#define __OPTION_H__


#define MEGA	(1000000)

#define FIN 	(12000000)	


#define FCLK	(400000000)

// Main clock
#if FIN==12000000
	#if (FCLK==200000000)
	#define HCLK (FCLK/2)
	#define PCLK (HCLK/2)
	#elif (FCLK==304800000) || (FCLK==271500000) || (FCLK==240000000)
	#define HCLK (FCLK/3)
	#define PCLK (HCLK/2)
	#elif (FCLK==360000000) || (FCLK==380000000) || (FCLK==400000000)
	#define HCLK (FCLK/4)
	#define PCLK (HCLK/2)
	#elif (FCLK==340000000) || (FCLK==350000000) || (FCLK==300000000) || (FCLK==320000000) || (FCLK==330000000)
	#define HCLK (FCLK/4)
	#define PCLK (HCLK/1)
	#endif
#else	//FIN=16.9344MHz
	#if FCLK==266716800
	#define HCLK (FCLK/2)
	#define PCLK (HCLK/2)
	#elif FCLK==296352000
	#define HCLK (FCLK/3)
	#define PCLK (HCLK/2)
	#elif FCLK==399651840
	#define HCLK (FCLK/3)
	#define PCLK (HCLK/2)
	#endif
#endif

// USB clock
#define UCLK 48000000

/*
#ifdef GLOBAL_CLK
unsigned int FCLK;
unsigned int HCLK;
unsigned int PCLK;
#else
extern unsigned int FCLK;
extern unsigned int HCLK;
extern unsigned int PCLK;
#endif
*/
// BUSWIDTH : 16,32
#define BUSWIDTH    (32)

//64MB
// 0x30000000 ~ 0x30ffffff : Download Area (16MB) Cacheable
// 0x31000000 ~ 0x33feffff : Non-Cacheable Area
// 0x33ff0000 ~ 0x33ff47ff : Heap & RW Area
// 0x33ff4800 ~ 0x33ff7fff : FIQ ~ User Stack Area
// 0x33ff8000 ~ 0x33fffeff : Not Useed Area
// 0x33ffff00 ~ 0x33ffffff : Exception & ISR Vector Table

#define _RAM_STARTADDRESS 		0x30000000
#define _ISR_STARTADDRESS 		0x33ffff00     
#define _MMUTT_STARTADDRESS		0x33ff8000
#define _STACK_BASEADDRESS		0x33ff8000
#define HEAPEND		  			0x33ff0000
#define _NONCACHE_STARTADDRESS	0x31000000


//USB Device Options
#define USBDMA		1	
#define USBDMA_DEMAND 	0	//the downloadFileSize should be (64*n)
#define BULK_PKT_SIZE	64

// note: makefile,option.a should be changed

#endif /*__OPTION_H__*/

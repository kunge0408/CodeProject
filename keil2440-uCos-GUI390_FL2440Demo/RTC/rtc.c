/*--------------------------------------------------------------------------*/
/*  RTC controls                                                            */


#include "2440addr.h"
#include "rtc.h"

#define TO_BCD(n)		((((n) / 10) << 4) | ((n) % 10))

#define FROM_BCD(n)		((((n) >> 4) * 10) + ((n) & 0xf))


BOOL rtc_gettime (RTC *rtc)
{	

	rtc->sec = FROM_BCD(rBCDSEC & 0x7f);
	rtc->min = FROM_BCD(rBCDMIN & 0x7f);
	rtc->hour = FROM_BCD(rBCDHOUR & 0x3f);
	rtc->mday =  FROM_BCD(rBCDDAY & 0x03f);
	rtc->month =FROM_BCD(rBCDMON & 0x1f);
	rtc->year = (2000 + rBCDYEAR);

	return TRUE;
}




BOOL rtc_settime (const RTC *rtc)
{

	rRTCCON = 1 ;		//RTC read and write enable

	rBCDYEAR = (unsigned char)TO_BCD(rtc->year%100);	//年
    rBCDMON  = (unsigned char)TO_BCD(rtc->month);		//月
    rBCDDAY	 = (unsigned char)TO_BCD(rtc->mday);			//日	
	//rBCDDATE = wDayOfWeek+1;						//星期
	rBCDHOUR = (unsigned char)TO_BCD(rtc->hour);		//小时
	rBCDMIN  = (unsigned char)TO_BCD(rtc->min);		//分
	rBCDSEC  = (unsigned char)TO_BCD(rtc->sec);		//秒
	
	rRTCCON &= ~1 ;		//RTC read and write disable

	return TRUE;	
	
}




BOOL rtc_init (void)
{	
	int wYear, wMonth,wDay,wDayOfWeek,wHour,wMinute,wSecond;

    wYear = 2009;
    wMonth = 3;
    wDay = 14;
    wDayOfWeek = 6;
    wHour= 19;
    wMinute = 57;
    wSecond = 30;
	
	rRTCCON = 1 ;		//RTC read and write enable

	rBCDYEAR = (unsigned char)TO_BCD(wYear%100);	//年
    rBCDMON  = (unsigned char)TO_BCD(wMonth);		//月
    rBCDDAY	 = (unsigned char)TO_BCD(wDay);			//日	
	rBCDDATE = wDayOfWeek+1;						//星期
	rBCDHOUR = (unsigned char)TO_BCD(wHour);		//小时
	rBCDMIN  = (unsigned char)TO_BCD(wMinute);		//分
	rBCDSEC  = (unsigned char)TO_BCD(wSecond);		//秒
	
	rRTCCON &= ~1 ;		//RTC read and write disable
	
	return TRUE;
	
}


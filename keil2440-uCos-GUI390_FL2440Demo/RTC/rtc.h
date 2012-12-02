#include "integer.h"

typedef struct {
	WORD	year;
	BYTE	month;
	BYTE	mday;
	BYTE	hour;
	BYTE	min;
	BYTE	sec;
} RTC;

BOOL rtc_init (void);						/* Initialize RTC */
BOOL rtc_gettime (RTC*);					/* Get time */
BOOL rtc_settime (const RTC*);				/* Set time */


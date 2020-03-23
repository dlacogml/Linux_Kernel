/* 
 *
 * rtc.h - 
 * 
 */

#ifndef _RTC_H
#define _RTC_H

#define RTC_PORT    0x70
#define RTC_DATA    0x71
#define RTC_A       0x8A
#define RTC_B       0x8B
#define RTC_C       0x8C

void rtc_init();

#endif

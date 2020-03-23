/* 
 * 
 * rtc.c - 
 * 
 */

#include "lib.h"
#include "rtc.h"
#include "i8259.h"


/* rtc_init
 * DESCRIPTION: initializes the RTC
 * INPUT: nothing
 * OUTPUT: nothing
 * SIDE EFFECT: 
 */
void rtc_init() {
    
    cli();			
    outb(RTC_B, RTC_PORT);		
    char prev = inb(RTC_DATA);	
    outb(RTC_B, RTC_PORT);		
    outb(prev | 0x40, RTC_DATA);	
    sti();
    
}

void rtc_set_rate(unsigned char rate) {

    rate &= 0x0F;
    cli();

    outb(RTC_A, RTC_PORT);		// set index to register A, disable NMI
    char prev = inb(RTC_DATA);	// get initial value of register A
    outb(RTC_A, RTC_PORT);		// reset index to A
    outb((prev & 0xF0) | rate, RTC_DATA); //write only our rate to A. Note, rate is the bottom 4 bits.
    sti();

}

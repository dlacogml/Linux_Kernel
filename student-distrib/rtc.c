/* 
 * 
 * rtc.c - functions to interact with the rtc 
 * 
 */

#include "lib.h"
#include "rtc.h"
#include "i8259.h"

#define RTC_IRQ     8

static volatile char INT_RECEIVED = 0; //check if an rtc int is received
/* rtc_init
 * DESCRIPTION: initializes the RTC, enables interrupt request for rtc
 * INPUT: nothing
 * OUTPUT: nothing
 * SIDE EFFECT: 
 */
void rtc_init() 
{
    cli();	
    enable_irq(RTC_IRQ);         //enable the irq for rtc on pic 
    outb(RTC_B, RTC_PORT);		 //select reg B and disabke nmi
    char prev = inb(RTC_DATA);	 //read the current value of register B
    outb(RTC_B, RTC_PORT);		 //reset
    outb(prev | 0x40, RTC_DATA); //or with 0x40 to enbable bit 6 of Reg B PIE
    sti(); 
}

/* rtc_set_rate
 * DESCRIPTION: sets the rate of the rtc
 * INPUT: rate
 * OUTPUT: nothing
 * SIDE EFFECT: 
 */
void rtc_set_rate(unsigned char rate) 
{

    rate &= 0x0F; //rate is and-ed with 0x0F (15 base 10) to make sure rate is not above 15
    cli();
    outb(RTC_A, RTC_PORT);		// set index to register A, disable NMI
    char curr = inb(RTC_DATA);	// get initial value of register A
    outb(RTC_A, RTC_PORT);		// reset index to A
    outb((curr & 0xF0) | rate, RTC_DATA); //write only our rate to A. Note, rate is the bottom 4 bits.
    sti();

}

/* handler40
 * DESCRIPTION: handler for RTC interrupts
 * INPUT: nothing
 * OUTPUT: nothing
 * SIDE EFFECT: 
 */
void handler40(){
    cli();            
    INT_RECEIVED = 1;       //enable INT_RECEIVED when an interrput is raised
    test_interrupts();      // checks if the rtc works
    outb(RTC_C, RTC_PORT);	// set index to register A, disable NMI
    inb(RTC_DATA);		    // retrieve rtc_data
    send_eoi(RTC_IRQ);            // end of interrupt
    sti();
}

//uint32_t rtc_open(const uint8_t* filename)
//interface: set the frequency to be 2HZ
//
//
//

uint32_t rtc_open(const uint8_t* filename)
{
    cli();      
    //lowest freqency is 2HZ which is MAX_INT_RATE >> ((rate = 15)-1)
    unsigned char rate = 15;
    outb(RTC_A, RTC_PORT);		// set index to register A, disable NMI
    char curr = inb(RTC_DATA);	// get initial value of register A
    outb(RTC_A, RTC_PORT);		// reset index to A
    outb((curr & 0xF0) | rate, RTC_DATA); //write only our rate to A. Note, rate is the bottom 4 bits.
    sti();
    return 0;
}
//uint32_t rtc_close(int32_t fd)
//interface: nothing
//input: fd
//output: none
//return value: 0
//side effect: none
uint32_t rtc_close(int32_t fd)
{
    //do nothing
    return 0;
}
//uint32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
//interface: block until the next interrupt, return 0
//input: fd:
//       buf:
//       nbytes:
//output: none
//return value: 0
//side effect: block until the next interrupt is received
uint32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
{
    //While an interrupt is not received
    while(!INT_RECEIVED)
    {
        //do nothing
    }
    INT_RECEIVED = 0; //reset INT_RECEIVED
    return 0;
}
//uint32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
//interface: write to rtc with rates corresponding to the correct frequency input
//input: buf: pointer to frequency
//output: none
//return value: -1 : failure
//              0  : succuss
//side effect: change the interrupt rate in rtc
uint32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes)
{
    int freq = *((int*)buf);
    //check if buf is NULL, frequency is NULL, frequency is power of 2, frequency is within the range
    if(!buf || freq <=0 || (freq & (freq-1)) || freq > K_MAX_INT_FREQ)
        return -1;
    //frequency = MAX_INT_RATE >> (RATE - 1)
    //maximum kernel can hace is 6 which corresponds to frequency of 1024HZ
    //K_MAX_INT_RATE = MAX_INT_RATE >> (6-1)
    unsigned char rate = K_MAX_INT_RATE;
    while(MAX_INT_FREQ >> (rate - 1) != freq)
    {
        rate++; //increment rate until we find the correct rate corresponding to the right frequency
    }
    cli();
    outb(RTC_A, RTC_PORT);		          // set index to register A, disable NMI
    char curr = inb(RTC_DATA);	          // get initial value of register A
    outb(RTC_A, RTC_PORT);		          // reset index to A
    outb((curr & 0xF0) | rate, RTC_DATA); //write only our rate to A. Note, rate is the bottom 4 bits.
    sti();
    return 0;
}


/* 
 * 
 * rtc.c - functions to interact with the rtc 
 * 
 */

#include "lib.h"
#include "rtc.h"
#include "i8259.h"
#include "keyboard.h"
#include "filesystem.h"

#define RTC_IRQ     8
#define BIT6        0x40
#define HIGH_MASK   0xF0

static volatile int8_t INT_RECEIVED = 0; //check if an rtc int is received
// static volatile int8_t rate;
/* rtc_init
 * DESCRIPTION: initializes the RTC, enables interrupt request for rtc
 * INPUT: nothing
 * OUTPUT: nothing
 * SIDE EFFECT: 
 */
void rtc_init() 
{
    disable_irq(RTC_IRQ);
    send_eoi(RTC_IRQ);
    outb(RTC_B, RTC_PORT);		 //select reg B and disabke nmi
    char prev = inb(RTC_DATA);	 //read the current value of register B
    outb(RTC_B, RTC_PORT);		 //reset
    outb(prev | BIT6, RTC_DATA); //or with 0x40 to enbable bit 6 of Reg B PIE
    // lowest freqency is 2HZ which is MAX_INT_RATE >> ((rate = 15)-1)
    // t_s[cur_ter].rate = 6;
    uint8_t rt = 6;
    outb(RTC_A, RTC_PORT);		// set index to register A, disable NMI
    int8_t curr = inb(RTC_DATA);	// get initial value of register A
    outb(RTC_A, RTC_PORT);		// reset index to A
    outb((curr & HIGH_MASK) | rt, RTC_DATA); //write only our rate to A. Note, rate is the bottom 4 bits.
    enable_irq(RTC_IRQ);         //enable the irq for rtc on pic 
}

/* handler40
 * DESCRIPTION: handler for RTC interrupts
 * INPUT: nothing
 * OUTPUT: nothing
 * SIDE EFFECT: 
 */
void handler40(){     
    // disable_irq(RTC_IRQ);
    send_eoi(RTC_IRQ);  
    pcb_t* pcb_pointer;
    int i;
    for(i = 0; i < 3; i++){
        if(t_s[i].term_started){
            pcb_pointer = (pcb_t*)((_8MB - t_s[i].current_running_pid * _8KB - END_OFFSET) & PCB_MASK);
            if(pcb_pointer->counter > 0) {
                pcb_pointer->counter--;
            }
        } 
    }
    // printf("%d ", t_s[cur_ter].counter);
    // INT_RECEIVED = 1;       //enable INT_RECEIVED when an interrput is raised
    // test_interrupts();      // checks if the rtc works
    outb(RTC_C, RTC_PORT);	// set index to register A, disable NMI
    inb(RTC_DATA);		    // retrieve rtc_data
    // enable_irq(RTC_IRQ);    
}

//uint32_t rtc_open(const uint8_t* filename)
//interface: set the frequency to be 2HZ
//
//
//

int32_t rtc_open(const uint8_t* filename)
{     
    pcb_t* pcb_pointer = (pcb_t*)((_8MB - t_s[cur_ter].current_running_pid * _8KB - END_OFFSET) & PCB_MASK);
    pcb_pointer->rtc_freq = 2;
    pcb_pointer->counter = 0;
    return 0;
}
//uint32_t rtc_close(int32_t fd)
//interface: nothing
//input: fd
//output: none
//return value: 0
//side effect: none
int32_t rtc_close(int32_t fd)
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
int32_t rtc_read(int32_t fd, uint8_t* buf, int32_t nbytes)
{
//    disable_irq(0);
//    outb(RTC_A, RTC_PORT);  
//    t_s[cur_ter].freq = 32768 >> ((inb(RTC_DATA) & 0x0F) - 1);
//    printf("%d", t_s[cur_ter].freq);
    pcb_t* pcb_pointer = (pcb_t*)((_8MB - t_s[cur_ter].current_running_pid * _8KB - END_OFFSET) & PCB_MASK);
    pcb_pointer->counter = 1024 / pcb_pointer->rtc_freq;
//    printf("%d", t_s[cur_ter].counter);
    //While an interrupt is not received
    while(pcb_pointer->counter > 0)
    {
        // t_s[cur_ter].rate++;
        //do nothing
    }
    // enable_irq(0);
    return 0;
}
//uint32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
//interface: write to rtc with rates corresponding to the correct frequency input
//input: buf: pointer to frequency
//output: none
//return value: -1 : failure
//              0  : succuss
//side effect: change the interrupt rate in rtc
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes)
{
    uint16_t freq = *((int*)buf);
    //check if buf is NULL, frequency is NULL, frequency is power of 2, frequency is within the range
    if(!buf || freq <=0 || (freq & (freq-1)) || freq > K_MAX_INT_FREQ)
        return -1;    
    //frequency = MAX_INT_RATE >> (RATE - 1)
    //maximum kernel can hace is 6 which corresponds to frequency of 1024HZ
    //K_MAX_INT_RATE = MAX_INT_RATE >> (6-1)
    pcb_t* pcb_pointer = (pcb_t*)((_8MB - t_s[cur_ter].current_running_pid * _8KB - END_OFFSET) & PCB_MASK);

    unsigned char rate = K_MAX_INT_RATE;
    // while(MAX_INT_FREQ >> (rate - 1) != freq)
    // {
    //     rate++; //increment rate until we find the correct rate corresponding to the right frequency
    // }

    pcb_pointer->rtc_freq = (int) freq; 

    // outb(RTC_A, RTC_PORT);		          // set index to register A, disable NMI
    // int8_t curr = inb(RTC_DATA);	          // get initial value of register A
    // outb(RTC_A, RTC_PORT);		          // reset index to A
    // outb((curr & HIGH_MASK) | rate, RTC_DATA); //write only our rate to A. Note, rate is the bottom 4 bits.
    return 0;
}


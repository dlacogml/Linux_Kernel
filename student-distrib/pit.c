#include "pit.h"


void handler32(){
    disable_irq(0);
    send_eoi(0);
    schedule();
    enable_irq(0);
}


void init_pit(int hz) {
    disable_irq(0);
    send_eoi(0);
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    outb(0x36, 0x43);             /* Set our command byte 0x36 */
    outb(divisor & 0xFF, 0x40);   /* Set low byte of divisor */
    outb(divisor >> 8, 0x40);     /* Set high byte of divisor */
    enable_irq(0);
}


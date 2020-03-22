/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */
/* Initialize the 8259 PIC */
void i8259_init(void) {
    outb(0xff, MASTER_8259_PORT + 1);
    outb(0xff, SLAVE_8259_PORT + 1);

    outb_p(0x11, MASTER_8259_PORT);
    outb_p(0x20 + 0, MASTER_8259_PORT + 1);
    outb_p(0x04, MASTER_8259_PORT + 1);
    outb_p(0x01, MASTER_8259_PORT + 1);

    outb_p(0x11, SLAVE_8259_PORT);
    outb_p(0x20 + 8, SLAVE_8259_PORT + 1);
    outb_p(0x04, SLAVE_8259_PORT + 1);
    outb_p(0x01, SLAVE_8259_PORT + 1);

    udelay(100);
    // outb(cached_21, MASTER_8259_PORT + 1);


}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
}

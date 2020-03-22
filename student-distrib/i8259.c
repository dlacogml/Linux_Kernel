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
    // unsigned char a1, a2;
 
	// a1 = inb(MASTER_8259_DATA);                        // save masks
	// a2 = inb(SLAVE_8259_DATA);
    master_mask = 0xff;
    slave_mask = 0xff;

    outb(master_mask, MASTER_8259_DATA);
    outb(slave_mask, SLAVE_8259_DATA);

    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_DATA);
    outb(ICW3_MASTER, MASTER_8259_DATA);
    outb(ICW4, MASTER_8259_DATA);

    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_8259_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_DATA);
    outb(ICW4, SLAVE_8259_DATA);

    // udelay(100);
    // outb(cached_21, MASTER_8259_PORT + 1);


}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
 
    if(irq_num < 8) {
        master_mask = master_mask | (1 << irq_num);
        outb(MASTER_8259_DATA, master_mask);
    } else {
        irq_num -= 8;
        slave_mask = slave_mask | (1 << irq_num);
        outb(SLAVE_8259_DATA, slave_mask);
    }

}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    if(irq_num < 8) {
        master_mask = master_mask & ~(1 << irq_num);
        outb(MASTER_8259_DATA, master_mask);
    } else {
        irq_num -= 8;
        slave_mask = slave_mask & ~(1 << irq_num);
        outb(SLAVE_8259_DATA, slave_mask);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if(irq_num >= 8){
		outb(SLAVE_8259_PORT, EOI);
    }
	outb(MASTER_8259_PORT, EOI);
}

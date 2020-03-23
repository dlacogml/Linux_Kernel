/*
 * idt.c - 
 * 
 */

#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "rtc.h"
#include "keyboard.h"


/* init_idt()
 * DESCRIPTION: initializes the idt
 * INPUT: none
 * OUTPUT: none
 * SIDE EFFECT:
 */
void init_idt() {
    idt_desc_t desc;

        /* Exceptions */
        desc.present = 0x1;
        desc.dpl = 0x0;
        desc.reserved0 = 0x0;
        desc.size = 0x1;
        desc.reserved1 = 0x1;
        desc.reserved2 = 0x1;
        desc.reserved3 = 0x0;
        desc.reserved4 = 0x0;
        desc.seg_selector = KERNEL_CS;

        SET_IDT_ENTRY(desc, handler0);
        idt[0] = desc;

        SET_IDT_ENTRY(desc, handler1);
        idt[1] = desc;

        SET_IDT_ENTRY(desc, handler3);
        idt[3] = desc;

        SET_IDT_ENTRY(desc, handler4);
        idt[4] = desc;

        SET_IDT_ENTRY(desc, handler5);
        idt[5] = desc;

        SET_IDT_ENTRY(desc, handler6);
        idt[6] = desc;

        SET_IDT_ENTRY(desc, handler7);
        idt[7] = desc;

        SET_IDT_ENTRY(desc, handler8);
        idt[8] = desc;

        SET_IDT_ENTRY(desc, handler9);
        idt[9] = desc;

        SET_IDT_ENTRY(desc, handler10);
        idt[10] = desc;

        SET_IDT_ENTRY(desc, handler11);
        idt[11] = desc;

        SET_IDT_ENTRY(desc, handler12);
        idt[12] = desc;

        SET_IDT_ENTRY(desc, handler13);
        idt[13] = desc;

        SET_IDT_ENTRY(desc, handler14);
        idt[14] = desc;

        SET_IDT_ENTRY(desc, handler16);
        idt[16] = desc;

        SET_IDT_ENTRY(desc, handler17);
        idt[17] = desc;

        SET_IDT_ENTRY(desc, handler18);
        idt[18] = desc;

        SET_IDT_ENTRY(desc, handler19);
        idt[19] = desc;
        
        /* Interrupts */
        SET_IDT_ENTRY(desc, handler2);
        idt[2] = desc;

        SET_IDT_ENTRY(desc, handler33);
        idt[33] = desc;

        SET_IDT_ENTRY(desc, handler40);
        idt[40] = desc;

        /* System Call */
        desc.dpl = 0x3;


        SET_IDT_ENTRY(desc, handler128);
        idt[0x80] = desc;

        lidt(idt_desc_ptr);
}

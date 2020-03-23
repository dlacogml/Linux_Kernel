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
#include "idt_asm.h"

/* void handler[#]()
    DESCRIPTION: These functions prints the descriptions of the specific interrupt called
    and freezes the kernel for an indefinite amount of time
    INPUT: NONE
    OUTPUT: NONE
*/

void handler0(){
    clear();
    printf("Divide by zero");
    while(1){

    }
}

void handler1(){
    clear();
    printf("Debug Exception");
    while(1){

    }
}

void handler2(){
    clear();
    printf("NMI Interrupt");
    while(1){

    }
}
void handler3(){
    clear();
    printf("Breakpoint Exception");
    while(1){

    }
}
void handler4(){
    clear();
    printf("Overflow Exception");
    while(1){

    }
}

void handler5(){
    clear();
    printf("BOUND Range Exceeded Exception");
    while(1){

    }
}

void handler6(){
    clear();
    printf("Invalid Opcode Exception");
    while(1){

    }
}

void handler7(){
    clear();
    printf("Device Not Available Exception");
    while(1){

    }
}

void handler8(){
    clear();
    printf("Double Fault Exception");
    while(1){

    }
}

void handler9(){
    clear();
    printf("Debug Exception");
    while(1){

    }
}

void handler10(){
    clear();
    printf("Invalid TSS Exception");
    while(1){

    }
}

void handler11(){
    clear();
    printf("Segment Not Present");
    while(1){

    }
}

void handler12(){
    clear();
    printf("Stack Fault Exception");
    while(1){

    }
}

void handler13(){
    clear();
    printf("General Protection Exception");
    while(1){

    }
}

void handler14(){
    clear();
    printf("Page Fault Exception");
    while(1){

    }
}

void handler15(){
    clear();
    printf("Nothing");
    while(1){

    }
}

void handler16(){
    clear();
    printf("x87 FPU Floating Point Error");
    while(1){

    }
}

void handler17(){
    clear();
    printf("Alignment Check Exception");
    while(1){

    }
}

void handler18(){
    clear();
    printf("Machine Check Exception");
    while(1){

    }
}

void handler19(){
    clear();
    printf("SIMD Floating Point Exception");
    while(1){

    }
}

void handler128(){
    printf("System Call");

}

/* init_idt()
 * DESCRIPTION: initializes the idt
 * INPUT: none
 * OUTPUT: none
 * SIDE EFFECT:
 */
void init_idt() {
    idt_desc_t desc;

    /* Setting specific bits for interrupt gate */
    desc.present = 0x1;
    desc.dpl = 0x0;
    desc.reserved0 = 0x0;
    desc.size = 0x1;
    desc.reserved1 = 0x1;
    desc.reserved2 = 0x1;
    desc.reserved3 = 0x0;
    desc.reserved4 = 0x0;
    desc.seg_selector = KERNEL_CS;

    /* Sets IDT entries in the interrupt descriptor table */
    SET_IDT_ENTRY(desc, handler0);
    idt[0] = desc; //set desc equal to this index (0) of IDT

    SET_IDT_ENTRY(desc, handler1);
    idt[1] = desc; //set desc equal to this index (1) of IDT

    SET_IDT_ENTRY(desc, handler2);
    idt[2] = desc; //set desc equal to this index (2) of IDT

    SET_IDT_ENTRY(desc, handler3);
    idt[3] = desc; //set desc equal to this index (3) of IDT

    SET_IDT_ENTRY(desc, handler4);
    idt[4] = desc; //set desc equal to this index (4) of IDT

    SET_IDT_ENTRY(desc, handler5);
    idt[5] = desc; //set desc equal to this index (5) of IDT

    SET_IDT_ENTRY(desc, handler6);
    idt[6] = desc; //set desc equal to this index (6) of IDT

    SET_IDT_ENTRY(desc, handler7);
    idt[7] = desc; //set desc equal to this index (7) of IDT

    SET_IDT_ENTRY(desc, handler8);
    idt[8] = desc; //set desc equal to this index (8) of IDT

    SET_IDT_ENTRY(desc, handler9);
    idt[9] = desc; //set desc equal to this index (9) of IDT

    SET_IDT_ENTRY(desc, handler10);
    idt[10] = desc; //set desc equal to this index (10) of IDT

    SET_IDT_ENTRY(desc, handler11);
    idt[11] = desc; //set desc equal to this index (11) of IDT

    SET_IDT_ENTRY(desc, handler12);
    idt[12] = desc; //set desc equal to this index (12) of IDT

    SET_IDT_ENTRY(desc, handler13);
    idt[13] = desc; //set desc equal to this index (13) of IDT

    SET_IDT_ENTRY(desc, handler14);
    idt[14] = desc; //set desc equal to this index (14) of IDT

    SET_IDT_ENTRY(desc, handler16);
    idt[16] = desc; //set desc equal to this index (16) of IDT

    SET_IDT_ENTRY(desc, handler17);
    idt[17] = desc; //set desc equal to this index (17) of IDT

    SET_IDT_ENTRY(desc, handler18);
    idt[18] = desc; //set desc equal to this index (18) of IDT

    SET_IDT_ENTRY(desc, handler19);
    idt[19] = desc; //set desc equal to this index (19) of IDT
    
    /* Interrupts */
    SET_IDT_ENTRY(desc, asmHandler2);
    idt[2] = desc;

    SET_IDT_ENTRY(desc, asmHandler33);
    idt[33] = desc;

    SET_IDT_ENTRY(desc, asmHandler40);
    idt[40] = desc;

    /* System Call */
    desc.dpl = 0x3;


    SET_IDT_ENTRY(desc, asmHandler128);
    idt[0x80] = desc;

    /* Load IDT */
    lidt(idt_desc_ptr);
}



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


void handler0();
void handler1();
void handler2();
void handler3();
void handler4();
void handler5();
void handler6();
void handler7();
void handler8();
void handler9();
void handler10();
void handler11();
void handler12();
void handler13();
void handler14();
void handler15();
void handler16();
void handler17();
void handler18();
void handler19();
void handler33();
void handler40();
void handler128();

void handler0(){
    printf("Divide by zero");
    while(1){

    }
}

void handler1(){
    printf("Debug Exception");
    while(1){

    }
}

void handler2(){
    printf("NMI Interrupt");
    while(1){

    }
}
void handler3(){
    printf("Breakpoint Exception");
    while(1){

    }
}
void handler4(){
    printf("Overflow Exception");
    while(1){

    }
}

void handler5(){
    printf("BOUND Range Exceeded Exception");
    while(1){

    }
}
void handler6(){
    printf("Invalid Opcode Exception");
    while(1){

    }
}
void handler7(){
    printf("Device Not Available Exception");
    while(1){

    }
}
void handler8(){
    printf("Double Fault Exception");
    while(1){

    }
}
void handler9(){
    printf("Debug Exception");
    while(1){

    }
}
void handler10(){
    printf("Invalid TSS Exception");
    while(1){

    }
}

void handler11(){
    printf("Segment Not Present");
    while(1){

    }
}
void handler12(){
    printf("Stack Fault Exception");
    while(1){

    }
}

void handler13(){
    printf("General Protection Exception");
    while(1){

    }
}

void handler14(){
    printf("Page Fault Exception");
    while(1){

    }
}

void handler15(){
    printf("Nothing");
    while(1){

    }
}
void handler16(){
    printf("x87 FPU Floating Point Error");
    while(1){

    }
}
void handler17(){
    printf("Alignment Check Exception");
    while(1){

    }
}

void handler18(){
    printf("Machine Check Exception");
    while(1){

    }
}
void handler19(){
    printf("SIMD Floating Point Exception");
    while(1){

    }
}


void handler33(){
    cli();
    int8_t key = inb(0x60);
    printf("%d\n", key);
    // if (key > 0 && keyboard_map[key] != 0){
    //     printf("%c", keyboard_map[key]);
    // }
    send_eoi(1);
    sti();
}

void handler40(){
    cli();
    printf("RTC");
    test_interrupts();
    outb(RTC_C, RTC_PORT);	
    inb(RTC_DATA);		
    send_eoi(8);
    sti();
}

void handler128(){
    printf("System Call");
    while(1){

    }
}

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



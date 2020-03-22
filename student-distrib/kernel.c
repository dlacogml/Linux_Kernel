/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"

#define RUN_TESTS
#define RTC_PORT    0x70
#define RTC_DATA    0x71
#define RTC_A       0x8A
#define RTC_B       0x8B
#define RTC_C       0x8C
/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags, bit)   ((flags) & (1 << (bit)))
unsigned char keyboard_map[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
  '9', '0', '-', '=', '\b',     /* Backspace */
  '\t',                 /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
 '\'', '`',   0,                /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
  'm', ',', '.', '/',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};
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
// void handler32();
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

// void handler32(){
//     printf("PIC");
//     while(1){

//     }
// }

void handler33(){
    cli();
    int8_t key = inb(0x60);
    if (key > 0 && keyboard_map[key] != 0){
        printf("%c", keyboard_map[key]);
    }
    send_eoi(1);
    sti();
}

void handler40(){
    cli();
    printf("RTC");
    test_interrupts();
    outb(RTC_C, RTC_PORT);	// select register C
    inb(RTC_DATA);		// just throw away contents
    send_eoi(8);
    sti();
}

void handler128(){
    printf("System Call");
    while(1){

    }
}
/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void entry(unsigned long magic, unsigned long addr) {

    multiboot_info_t *mbi;

    /* Clear the screen. */
    clear();

    /* Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("Invalid magic number: 0x%#x\n", (unsigned)magic);
        return;
    }

    /* Set MBI to the address of the Multiboot information structure. */
    mbi = (multiboot_info_t *) addr;

    /* Print out the flags. */
    // printf("flags = 0x%#x\n", (unsigned)mbi->flags);

    /* Are mem_* valid? */
    if (CHECK_FLAG(mbi->flags, 0))
        // printf("mem_lower = %uKB, mem_upper = %uKB\n", (unsigned)mbi->mem_lower, (unsigned)mbi->mem_upper);

    /* Is boot_device valid? */
    if (CHECK_FLAG(mbi->flags, 1))
        // printf("boot_device = 0x%#x\n", (unsigned)mbi->boot_device);

    /* Is the command line passed? */
    if (CHECK_FLAG(mbi->flags, 2))
        // printf("cmdline = %s\n", (char *)mbi->cmdline);

    if (CHECK_FLAG(mbi->flags, 3)) {
        int mod_count = 0;
        int i;
        module_t* mod = (module_t*)mbi->mods_addr;
        while (mod_count < mbi->mods_count) {
            // printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
            // printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
            // printf("First few bytes of module:\n");
            for (i = 0; i < 16; i++) {
                // printf("0x%x ", *((char*)(mod->mod_start+i)));
            }
            printf("\n");
            mod_count++;
            mod++;
        }
    }
    /* Bits 4 and 5 are mutually exclusive! */
    if (CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5)) {
        // printf("Both bits 4 and 5 are set.\n");
        return;
    }

    /* Is the section header table of ELF valid? */
    if (CHECK_FLAG(mbi->flags, 5)) {
        elf_section_header_table_t *elf_sec = &(mbi->elf_sec);
        // printf("elf_sec: num = %u, size = 0x%#x, addr = 0x%#x, shndx = 0x%#x\n",
                // (unsigned)elf_sec->num, (unsigned)elf_sec->size,
                // (unsigned)elf_sec->addr, (unsigned)elf_sec->shndx);
    }

    /* Are mmap_* valid? */
    if (CHECK_FLAG(mbi->flags, 6)) {
        memory_map_t *mmap;
        // printf("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
                // (unsigned)mbi->mmap_addr, (unsigned)mbi->mmap_length);
        // for (mmap = (memory_map_t *)mbi->mmap_addr;
        //         (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
        //         mmap = (memory_map_t *)((unsigned long)mmap + mmap->size + sizeof (mmap->size)))
            // printf("    size = 0x%x, base_addr = 0x%#x%#x\n    type = 0x%x,  length    = 0x%#x%#x\n",
                    // (unsigned)mmap->size,
                    // (unsigned)mmap->base_addr_high,
                    // (unsigned)mmap->base_addr_low,
                    // (unsigned)mmap->type,
                    // (unsigned)mmap->length_high,
                    // (unsigned)mmap->length_low);
    }

    /* Construct an LDT entry in the GDT */
    {
        seg_desc_t the_ldt_desc;
        the_ldt_desc.granularity = 0x0;
        the_ldt_desc.opsize      = 0x1;
        the_ldt_desc.reserved    = 0x0;
        the_ldt_desc.avail       = 0x0;
        the_ldt_desc.present     = 0x1;
        the_ldt_desc.dpl         = 0x0;
        the_ldt_desc.sys         = 0x0;
        the_ldt_desc.type        = 0x2;

        SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
        ldt_desc_ptr = the_ldt_desc;
        lldt(KERNEL_LDT);
    }

    /* Construct a TSS entry in the GDT */
    {
        seg_desc_t the_tss_desc;
        the_tss_desc.granularity   = 0x0;
        the_tss_desc.opsize        = 0x0;
        the_tss_desc.reserved      = 0x0;
        the_tss_desc.avail         = 0x0;
        the_tss_desc.seg_lim_19_16 = TSS_SIZE & 0x000F0000;
        the_tss_desc.present       = 0x1;
        the_tss_desc.dpl           = 0x0;
        the_tss_desc.sys           = 0x0;
        the_tss_desc.type          = 0x9;
        the_tss_desc.seg_lim_15_00 = TSS_SIZE & 0x0000FFFF;

        SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

        tss_desc_ptr = the_tss_desc;

        tss.ldt_segment_selector = KERNEL_LDT;
        tss.ss0 = KERNEL_DS;
        tss.esp0 = 0x800000;
        ltr(KERNEL_TSS);
    }

    {
        idt_desc_t desc;

        /* Exceptions */
        desc.present = 0x1;
        desc.dpl = 0x0;
        desc.reserved0 = 0x0;
        desc.size = 0x1;
        desc.reserved1 = 0x1;
        desc.reserved2 = 0x1;
        desc.reserved3 = 0x1;
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
        desc.reserved3 = 0x0; // change to interrupt
        SET_IDT_ENTRY(desc, handler2);
        idt[2] = desc;

        // SET_IDT_ENTRY(desc, handler32);
        // idt[32] = desc;

        SET_IDT_ENTRY(desc, handler33);
        idt[33] = desc;

        SET_IDT_ENTRY(desc, handler40);
        idt[40] = desc;

        /* System Call */
        desc.dpl = 0x3;
        desc.size = 0x0;
        desc.reserved2 = 0x0;
        desc.reserved3 = 0x1;
        desc.seg_selector = KERNEL_TSS;

        SET_IDT_ENTRY(desc, handler128);
        idt[0x80] = desc;

        lidt(idt_desc_ptr);

    }

    /* Init the PIC */
    i8259_init();
    

    /* Initialize devices, memory, filesystem, enable device interrupts on the
     * PIC, any other initialization stuff... */
    
    enable_irq(1);
    enable_irq(2);
    enable_irq(8);

    cli();			// disable interrupts
    outb(RTC_B, RTC_PORT);		// select register B, and disable NMI
    char prev = inb(RTC_DATA);	// read the current value of register B
    outb(RTC_B, RTC_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_DATA);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    sti();

    unsigned char rate = 6;
    rate &= 0x0F;			// rate must be above 2 and not over 15
    cli();
    outb(RTC_A, RTC_PORT);		// set index to register A, disable NMI
    prev = inb(RTC_DATA);	// get initial value of register A
    outb(RTC_A, RTC_PORT);		// reset index to A
    outb((prev & 0xF0) | rate, RTC_DATA); //write only our rate to A. Note, rate is the bottom 4 bits.
    sti();

    /* Enable interrupts */
    /* Do not enable the following until after you have set up your
     * IDT correctly otherwise QEMU will triple fault and simple close
     * without showing you any output */
    printf("Enabling Interrupts\n");
    sti();

#ifdef RUN_TESTS
    /* Run tests */
    launch_tests();
#endif
    /* Execute the first program ("shell") ... */

    /* Spin (nicely, so we don't chew up cycles) */
    asm volatile (".1: hlt; jmp .1;");
}



#include "keyboard.h"
#include "rtc.h"
#include "idt.h"
#include "lib.h"

#ifndef IDT_ASM_H
#define IDT_ASM_H

/*asmHandler functions for interrupts*/
void asmHandler2();
void asmHandler33();
void asmHandler40();
void asmHandler128();

#endif 

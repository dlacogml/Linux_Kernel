/*
 *  idt_asm.S - assembly linkage for the interrupt handlers
 *
 */

.text
/* making the asmHandler for handler 2, handler 33, handler 40 and handler 128*/
.globl asmHandler2
.globl asmHandler33
.globl asmHandler40
.globl asmHandler128

/* asmHandler2
 * INTERFACE: calls handler2
 * INPUT: none
 * OUTPUT: none
 * side effect: returns from the interrupt properly
 */
asmHandler2:
    pushl $2            # interrupt handler #2, push the interrupt number
    call handler2       # call the handler
    addl $4, %esp       # pop the interrupt number
    iret

/* asmHandler33
 * INTERFACE: calls handler33
 * INPUT: none
 * OUTPUT: none
 * side effect: returns from the interrupt properly
 */
asmHandler33:
    pushl $33           # interrupt handler #33, push the interrupt number
    call handler33      # call the handler
    addl $4, %esp       # pop the interrupt number
    iret

/* asmHandler40
 * INTERFACE: calls handler40
 * INPUT: none
 * OUTPUT: none
 * side effect: returns from the interrupt properly
 */
asmHandler40:
    pushl $40           # interrupt handler #140, push the interrupt number
    call handler40      # call the handler
    addl $4, %esp       # pop the interrupt number
    iret

/* asmHandler128
 * INTERFACE: calls handler128
 * INPUT: none
 * OUTPUT: none
 * side effect: returns from the interrupt properly
 */
asmHandler128:  
    
    pushl $128             # interrupt handler #128, push the interrupt number
    subl $1, %eax
    cmpl $0, %eax
    jl invalid_sys_num
    cmpl $9, %eax
    ja invalid_sys_num
    push %edi
    push %esi
    /* push args */ 
    push %edx
    push %ecx
    push %ebx
     # jump jumptable
    call *jump_table(, %eax, 4)
    pop %ebx
    pop %ecx
    pop %edx
    pop %esi
    pop %edi 
    addl $4, %esp
    iret

invalid_sys_num:
    addl $4, %esp          # pop the interrupt number
    iret

jump_table:
.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn


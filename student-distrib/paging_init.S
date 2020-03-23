
.text
.globl loadPageDirectory
.globl enablePaging

loadPageDirectory:
    push %ebp
    mov %esp, %ebp

    mov 8(%esp), %eax

    mov %cr3, %ebx
    and $0x0000fff, %ebx
    or %eax, %ebx
    mov %ebx, %cr3

         
    mov %cr4, %eax
    and $0xffffffcf, %eax
    or $0x10, %eax
    mov %eax, %cr4
    mov %ebp, %esp
    pop %ebp
    ret


enablePaging:
    push %ebp
    mov %esp, %ebp
    # enable paging 
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0

    mov %ebp, %esp
    pop %ebp
    ret


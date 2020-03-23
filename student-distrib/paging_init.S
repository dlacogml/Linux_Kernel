
.text
.globl loadPageDirectory
.globl enablePaging

loadPageDirectory:
    pushl %ebp
    movl %esp, %ebp

    pushl %ebx
    pushl %eax
    movl 8(%ebp), %eax

    movl %cr3, %ebx
    andl $0x0000fff, %ebx
    orl %eax, %ebx
    movl %ebx, %cr3

         
    movl %cr4, %eax
    andl $0x000007cf, %eax
    orl $0x90, %eax
    movl %eax, %cr4
    
    popl %eax
    popl %ebx
    leave
    ret


enablePaging:
    pushl %ebp
    pushl %eax
    movl %esp, %ebp
    # enable paging 
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0
    popl %eax
    leave
    ret


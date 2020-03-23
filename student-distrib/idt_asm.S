.text
.globl asmHandler2
.globl asmHandler33
.globl asmHandler40
.globl asmHandler128

asmHandler2:
    pushl $2
    call handler2 
    addl $4, %esp
    iret


asmHandler33:
    pushl $33
    call handler33
    addl $4, %esp
    iret


asmHandler40:
    pushl $40
    call handler40
    addl $4, %esp
    iret


asmHandler128:    
    pushl $128
    call handler128
    addl $4, %esp
    iret
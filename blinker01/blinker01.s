

.globl _start
_start:
    mov sp,#0x40000000
    add sp,sp,#0x4000

    ldr r0,=0xFFFFFC10
    mov r1,#4
    str r1,[r0]

    ldr r0,=0xFFFFF400
    mov r1,#0x100


    str r1,[r0,#0x00]
    str r1,[r0,#0x10]

top:
    str r1,[r0,#0x30]
    mov r2,#0x30
a:
    subs r2,r2,#1
    bne a

    str r1,[r0,#0x34]
    mov r2,#0x30
b:
    subs r2,r2,#1
    bne b

    b top

hang: b hang

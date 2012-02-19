

.globl _start
_start:
    mov sp,#0x40000000
    add sp,sp,#0x4000

    ldr r0,=0x40000100
    mov r1,#4
    str r1,[r0]

    ldrb r2,[r0]
    add r2,r2,#1
    strb r2,[r0,#1]
    ldrb r2,[r0,#1]
    add r2,r2,#1
    strb r2,[r0,#2]

    ldr r2,[r0]
    mov r3,#0xE0000000
    str r2,[r3]



    ;@ sim halts on this write
    mov r3,#0xF0000000
    str r3,[r3]

hang: b hang



.globl _start
_start:
    b   reset  ;@ 0x00000000 reset
    b   halt   ;@ 0x00000004 undefined
    b   halt   ;@ 0x00000008 swi
    b   halt   ;@ 0x0000000C prefetch abort
    b   halt   ;@ 0x00000010 data abort
    b   halt   ;@ 0x00000014 address exception
    b   irq_handler   ;@ 0x00000018 irq
    b   fiq_handler   ;@ 0x0000001C fiq

halt:
    mov r0,#0xE0000000
    str r0,[r0]

    ;@ halt sim
    mov r0,#0xF0000000
    str r0,[r0]
    b hang

reset:
    mov sp,#0x41000000

    mov r0,#0x0C000002
    teqp pc,r0
    ldr sp,=0x41000000

    mov r0,#0x0C000001
    teqp pc,r0
    ldr sp,=0x40FFFF00

    mov r0,#0x0C000003
    teqp pc,r0
    ldr sp,=0x40FFFE00

    bl notmain
    mov r0,#0xF0000000
    str r0,[r0]

hang: b hang


irq_handler:
    push {r0}
    push {r1}
    mov r0,#0xD1000000
    mov r1,#0x0400
    str r1,[r0,#0x24]
    pop {r1}
    pop {r0}
    subs pc,lr,#4

fiq_handler:
    mov r10,#0xD1000000
    mov r11,#0x0800
    str r11,[r10,#0x24]
    subs pc,lr,#4



.globl PUT32
PUT32:
    str r1,[r0]
    mov pc,lr

.globl GET32
GET32:
    ldr r0,[r0]
    mov pc,lr

.globl ASMDELAY
ASMDELAY:
    subs r0,r0,#1
    bne ASMDELAY
    mov pc,lr

.globl __aeabi_idiv0
__aeabi_idiv0:
    b hang

.globl cache_enable
cache_enable:
    mov r0,#0xffffffff
    mcr 15, 0, r0, cr3, cr0, 0   @ cacheable area
    mov r0, #1
    mcr 15, 0, r0, cr2, cr0, 0   @ cache enable
    mov pc,lr


.globl IRQEN
IRQEN:
    ldr r0,=0x0400000F
    mov r1,pc
    and r1,r0
    teqp    pc, r1
    mov pc,lr

.globl FIQEN
FIQEN:
    ldr r0,=0x0800000F
    mov r1,pc
    and r1,r0
    teqp    pc, r1
    mov pc,lr

[bits 32]

extern kernel_init

global _start
_start:
    ; xchg bx,bx
    call kernel_init

    xchg bx,bx

    int 0x80;调用0x80中断函数,系统调用
    jmp $;阻塞
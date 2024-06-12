[bits 32]
extern console_init
extern memory_init
extern kernel_init
extern gdt_init
global _start
_start:
    push ebx;ards_count
    push eax;magic

    call console_init   ; 控制台初始化
    call gdt_init       ; 全局描述符初始化
    call memory_init    ; 内存初始化
    call kernel_init    ; 内核初始化

    xchg bx, bx

    mov eax, 0; 0 号系统调用
    int 0x80;
    jmp $; 阻塞
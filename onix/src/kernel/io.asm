[bits 32]

section .text;代码段

global inb;将inb导出

inb:
    push ebp
    mov ebp,esp;保存帧

    xor eax,eax
    mov edx,[ebp+8];port
    in al,dx

    jmp $+2;延迟时间
    jmp $+2
    jmp $+2
    
    leave
    ret
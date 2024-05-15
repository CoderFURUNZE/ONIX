[bits 32]

section .text;代码段

global inb;将inb导出

inb:
    push ebp
    mov ebp,esp;保存帧

    xor eax,eax
    mov edx,[ebp+8];port
    in al,dx;将端口号dx的8 bit 输入到al

    jmp $+2;延迟时间
    jmp $+2
    jmp $+2
    
    leave
    ret

global inw;将inw导出

inw:
    push ebp
    mov ebp,esp;保存帧

    xor eax,eax
    mov edx,[ebp+8];port
    in ax,dx;将端口号dx的8 bit 输入到ax

    jmp $+2;延迟时间
    jmp $+2
    jmp $+2
    
    leave
    ret

global outb;将outb导出

outb:
    push ebp
    mov ebp,esp;保存帧

    mov edx,[ebp+8];port
    mov eax,[ebp+12];value
    out dx,ax;将al中的8bit输入出道 端口号dx

    jmp $+2;延迟时间
    jmp $+2
    jmp $+2
    
    leave
    ret

global outw;将outw导出

outw:
    push ebp
    mov ebp,esp;保存帧

    mov edx,[ebp+8];port
    mov eax,[ebp+12];value
    out dx,ax;将ax中的8bit输入出到 端口号dx

    jmp $+2;延迟时间
    jmp $+2
    jmp $+2
    
    leave
    ret
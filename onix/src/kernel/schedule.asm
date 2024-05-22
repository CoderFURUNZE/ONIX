
global task_switch
task_switch:
    push ebp
    mov ebp,esp;保存栈帧

    push ebx
    push esi
    push edi

    mov eax,esp
    and eax,0xfffff000;current

    mov [eax],esp
    
    mov eax,[ebp+8];next
    mov esp,[eax]

    pop edi
    pop esi
    pop ebx
    pop ebp

    ret

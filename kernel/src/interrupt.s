global get_port
global set_port

global int_DE
global int_key

section .text

set_port:
    mov rdx, rdi
    mov rax, rsi
    out dx, al
    ret

get_port:
    mov rdx, rdi
    in al, dx
    ret

int_DE:         ; Division by zero service routine
    endbr64
    mov rcx, 1
    mov r14, 0xDEADBEEF
    iretq

int_key:
    endbr64
    mov rax, 0xDEADBEEF
    iretq
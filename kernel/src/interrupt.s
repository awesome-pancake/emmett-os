section .note.GNU-stack noalloc noexec nowrite progbits

global get_port
global set_port

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
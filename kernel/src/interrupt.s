section .note.GNU-stack noalloc noexec nowrite progbits

global get_port
global set_port

section .text

; Sets a command for a legacy port
set_port:
        mov rdx, rdi
        mov rax, rsi
        out dx, al
        ret

; Retrieves a scancode from a legacy port
get_port:
        mov rdx, rdi
        in al, dx
        ret
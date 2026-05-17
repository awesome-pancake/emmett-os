
global load_segments

section .text

load_segments:
    push rdi
    lea rax, [rel .cs_jump]
    push rax
    retfq
.cs_jump:
    mov ds, si
    mov es, si
    mov fs, si
    mov gs, si
    mov ss, si

    ret

    mov rcx, 0
    div rcx
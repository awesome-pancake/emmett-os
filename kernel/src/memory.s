
global load_segments

section .text

load_segments:
    push rdi                    ; Reloads the code segment register using a far return
    lea rax, [rel .cs_jump]
    push rax
    retfq
    
.cs_jump:
    mov ds, si                  ; Reloads other miscellaneous segment registers
    mov es, si
    mov fs, si
    mov gs, si
    mov ss, si

    ret
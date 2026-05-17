#ifndef KERNEL_ENTRY
    #include <start.h>
    #define KERNEL_ENTRY 1
#endif

// Gate descriptor for the IDT
struct gate_descriptor {
    uint16_t    offset_l;
    uint16_t    selector;
    uint8_t     stack_table; // Offset into task state segment, should be all zero
    uint8_t     attributes;
    uint16_t    offset_m;
    uint32_t    offset_h;
    uint32_t    reserved;
};

// Structure that points to the interrupt descriptor table
struct idt_descriptor {
    uint16_t    size;
    uint64_t    offset;
} __attribute__((packed));

// Initializes the interrupt descriptor table
struct idt_descriptor *init_idt(struct console_state *console, struct gate_descriptor *addr);

extern void int_DE();       // Division by zero exception handler
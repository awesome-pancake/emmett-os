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
struct idt_descriptor *init_idt(struct gate_descriptor *addr);

// Creates a new entry for the idt
void register_interrupt(struct gate_descriptor *addr, void (*interrupt)(), uint16_t segment, uint8_t attributes, uint8_t vector);

// Sets a value in a local APIC register
void set_lapic(uint16_t reg, uint32_t value);

// Gets a value from a local APIC register
uint32_t get_lapic(uint16_t reg);

// Retrieves a scancode from a legacy port
extern uint8_t get_port(uint8_t port);

// Sets a command for a legacy port
extern void set_port(uint8_t port, uint8_t value);

// Wrapper for the CONVERT_CODE array
char convert_code(uint8_t scan_code);

// Converts a PS/2 scan code into an ASCII character
extern const char CONVERT_CODE[256];

extern void int_DE();           // Division by zero exception handler
extern void int_key();          // Keyboard interrupt handler
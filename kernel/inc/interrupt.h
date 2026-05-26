#ifndef KERNEL_ENTRY
    #include <start.h>
    #define KERNEL_ENTRY 1
#endif

#ifndef INTERRUPT
    #define INTERRUPT 1
#endif

// Holds the different registers used in Local APIC, which is helpful for setting up interrupts.
typedef enum {
    LAPIC_ID = 0x20,                // Local APIC ID Register
    LAPIC_VERSION = 0x30,           // Local APIC Version Register
    LAPIC_TPR = 0x80,               // Task Priority Register
    LAPIC_APR = 0x90,               // Arbitration Priority Register
    LAPIC_PPR = 0xA0,               // Processor Priority Register
    LAPIC_EOI = 0xB0,               // End of Interrupt Register
    LAPIC_RRD = 0xC0,               // Remote Read Register
    LAPIC_LDR = 0xD0,               // Logical Destination Register
    LAPIC_DFR = 0xE0,               // Destination Format Register
    LAPIC_SPURIOUS = 0xF0,          // Spurious Interrupt Vector Register
    LAPIC_ERROR = 0x280,            // Error Status Register
    LAPIC_LVT_CMCI = 0x2F0,         // LVT Corrected Machine Check Vector
    LAPIC_LVT_TIMER = 0x320,        // LVT Hardware Timer Vector
    LAPIC_LVT_THERMAL = 0x330,      // LVT Thermal Sensor Vector
    LAPIC_LVT_PERFORMANCE = 0x340,  // LVT Performance Monitoring Vector
    LAPIC_LVT_LINT0 = 0x350,        // LVT Local Interrupt 0
    LAPIC_LVT_LINT1 = 0x360,        // LVT Local Interrupt 1
    LAPIC_LVT_ERROR = 0x370,        // LVT Error Vector
    LAPIC_INITIAL = 0x380,          // Initial Count for Timer
    LAPIC_CURRENT = 0x390,          // Current Count for Timer
    LAPIC_DIVIDE = 0x3E0            // Divide Configuration for Timer
} LAPIC_REGISTER;

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

// Disables the 8259 legacy PIC
void disable_legacy_pic();

// Initializes the local APIC
void init_lapic();

// Enables hardware interrupts
void start_interrupts();

void __attribute__((interrupt)) _division_isr(void *arg);           // Division by zero exception handler
void __attribute__((interrupt)) _timer_isr(void *arg);              // Hardware timer interrupt

extern const uint8_t PIC1COMMAND;   // Master PIC command port
extern const uint8_t PIC1DATA;      // Master PIC data port
extern const uint8_t PIC2COMMAND;   // Slave PIC command port
extern const uint8_t PIC2DATA;      // Slave PIC data port
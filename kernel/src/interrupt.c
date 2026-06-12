// Interrupts
// Emmett Hoffman
// June 12, 2026
// 
// Description:
// - All logic related to interrupts is located here.
// - This file contains functions for initializing the IDT and programmable interrupt controller (LAPIC).
// - Additionally, all interrupt service routines (ISRs) are located here.
//
// Extra for Experts:
// - Worked with low-level processor tables to initialize interrupts.
// - Communicated directly with hardware devices (LAPIC and timer interrupts)
// - Investigated GCC compiler attributes (__attribute__((interrupt)))

#ifndef INTERRUPT
        #include <interrupt.h>
        #define INTERRUPT 1
#endif

#ifndef CONSOLE
        #include <console.h>
        #define CONSOLE 1
#endif

void register_interrupt(struct gate_descriptor *addr, void (*interrupt)(), uint16_t segment, uint8_t attributes, uint8_t vector) {

        // Fill in the information for a given interrupt
        (addr + vector)->offset_l = (uint16_t)(0xFFFF & (uint64_t)interrupt);
        (addr + vector)->selector = segment;
        (addr + vector)->stack_table = 0;
        (addr + vector)->attributes = attributes;
        (addr + vector)->offset_m = (uint16_t)((0xFFFF0000 & (uint64_t)interrupt) >> 16);
        (addr + vector)->offset_h = (uint32_t)((0xFFFFFFFF00000000 & (uint64_t)interrupt) >> 32);
        (addr + vector)->reserved = 0;
}

struct idt_descriptor *init_idt(struct gate_descriptor *addr) {

        // Points to the currently used interrupt.
        void (*curr_interrupt)() = &_division_isr;

        for(int i=0; i<=255; i++){
                register_interrupt(addr, curr_interrupt, 0x0008, 0x8E, (uint8_t)i);
        }

        register_interrupt(addr, &_timer_isr, 0x0008, 0x8E, 0x20);  // Register the hardware timer service routine at vector 0x20
        register_interrupt(addr, &_general_protection_isr, 0x0008, 0x8E, 0x0D); // Register the #GP fault handler

        struct idt_descriptor *idt_ptr = (struct idt_descriptor*)0x2000;
        
        idt_ptr->size = 0xFFF;
        idt_ptr->offset = (uint64_t)addr;

        // Loads the location of the interrupt descriptor table
        asm volatile(
        "lidt %0;"
        :
        :"m"(*idt_ptr)
        );

        return idt_ptr;
}

void set_lapic(uint16_t reg, uint32_t value) {

        // Ensure properly formatted input, ie. 16 byte alignment and within one page
        if(reg >= 0xFFF || reg % 0x10 != 0){
        return;
        }

        // Find the memory mapped address of the APIC register
        uint64_t lapic_addr = 0xFEE00000;
        uint32_t *reg_ptr = (uint32_t*)(lapic_addr + (uint64_t)reg);

        // Write to the register
        *reg_ptr = value;
}

uint32_t get_lapic(uint16_t reg) {

        // Ensure properly formatted input, ie. 16 byte alignment and within one page
        if(reg >= 0xFFF || reg % 0x10 != 0){
        return 0xFFFFFFFF;
        }

        // Find the memory mapped address of the APIC register
        uint64_t lapic_addr = 0xFEE00000;
        uint32_t *reg_ptr = (uint32_t*)(lapic_addr + (uint64_t)reg);

        // Get the value of the register
        return *reg_ptr;
}

void disable_legacy_pic() {
        set_port(PIC1DATA, 0xFF);
        set_port(PIC2DATA, 0xFF);
}

void init_lapic(){

        // Disable the 8259 interrupt controllers
        disable_legacy_pic();

        // Send an EOI command to clear interrupts
        set_lapic(LAPIC_EOI, 0);

        // Set task priority to 0
        set_lapic(LAPIC_TPR, 0);
        
        // Configure spurious interrupt register
        set_lapic(LAPIC_SPURIOUS, 0x1FF);
        
        // Enable hardware interrupts
        start_interrupts();
        
        // Configure hardware timer register
        uint32_t initial_count = 0x100000;
        uint32_t divide_count = 0x10;
        set_lapic(LAPIC_DIVIDE, divide_count);         // Set division interval
        set_lapic(LAPIC_LVT_TIMER, (get_lapic(LAPIC_LVT_TIMER) & 0x0FF00) | 0x20020);         // Set the timer vector
        set_lapic(LAPIC_INITIAL, initial_count);        // Set initial count

}

void start_interrupts() {
        asm volatile(
        "sti;"
        );
}

void __attribute__((interrupt)) _timer_isr(void *arg) {

        // prints("\nTimer interrupt");
        set_lapic(LAPIC_EOI, 0);
}

void __attribute__((interrupt)) _division_isr(void *arg) {

        asm volatile(
        "movq $1, %%rcx"
        :
        :
        : "%rcx"
        );

        error("Division by zero exception.");
}

void __attribute__((interrupt)) _general_protection_isr(void *arg) {

        error("\nGeneral protection fault. Halting execution.");
        for(;;) {
        asm volatile("hlt");
        }
}

const uint8_t PIC1COMMAND = 0x20;
const uint8_t PIC1DATA = 0x21;
const uint8_t PIC2COMMAND = 0xA0;
const uint8_t PIC2DATA = 0xA1;
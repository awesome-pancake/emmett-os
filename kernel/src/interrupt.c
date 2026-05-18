#include <interrupt.h>

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
    void (*curr_interrupt)() = &int_DE;

    for(int i=0; i<=255; i++){
        register_interrupt(addr, curr_interrupt, 0x0008, 0x8E, (uint8_t)i);
    }

    struct idt_descriptor *idt_ptr = (struct idt_descriptor*)0x2000;
    
    idt_ptr->size = 0xFFF;
    idt_ptr->offset = (uint64_t)addr;

    // Loads the location of the interrupt descriptor table
    asm(
        "lidt %0;"
        :
        :"m"(*idt_ptr)
    );

    return idt_ptr;
}

void set_lapic(uint16_t reg, uint32_t value) {

    // Ensure properly formatted input
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

    // Ensure properly formatted input
    if(reg >= 0xFFF || reg % 0x10 != 0){
        return 0xFFFFFFFF;
    }

    // Find the memory mapped address of the APIC register
    uint64_t lapic_addr = 0xFEE00000;
    uint32_t *reg_ptr = (uint32_t*)(lapic_addr + (uint64_t)reg);

    // Get the value of the register
    return *reg_ptr;
}
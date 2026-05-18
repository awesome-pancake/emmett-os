#include <start.h>
#include <console.h>
#include <memory.h>
#include <interrupt.h>
#include <error.h>

struct console_state console;

int kernel_main(struct display *disp, struct efi_memory_map *efi_memory_map) {
    // Ensure that no hardware interrupts are generated during kernel setup
    asm("cli");

    // Set some console variables
    console.display = disp;
    console.cursor_x = 0;
    console.cursor_y = 0;

    // Set console background colour
    console.back_colour.blue = 0x1A;
    console.back_colour.green = 0x0A;
    console.back_colour.red = 0x1A;
    console.back_colour.reserved = 0x00;

    // Set console text colour
    console.text_colour.blue = 0xFF;
    console.text_colour.green = 0xFF;
    console.text_colour.red = 0xFF;
    console.text_colour.reserved = 0x00;

    // Clear the console
    cls();
    prints("\n\rEmmett OS\n\n\r");
    prints("Kernel successfully loaded.\n\r");
    
    // Initialize available memory allocation system
    // struct mem_header *memory_map = init_memory_map(&console, efi_memory_map);
    // prints(&console, "Memory allocation initialized.\n\r");
    // struct mem_header *memory_map = NULL;

    // Allocate space for GDT
    // struct segment_descriptor *gdt = (struct segment_descriptor*)allocate_pages(&memory_map, 1);
    struct segment_descriptor *gdt = (struct segment_descriptor*)0;
    prints("Space for GDT allocated: ");
    printn((uint64_t)gdt);
    prints("\n\r");

    // Initialize GDT
    struct gdt_descriptor *gdtr = init_gdt(gdt);
    prints("GDT successfully loaded. GDTR: ");
    printn((uint64_t)gdtr);
    prints("\n\r");

    // Allocate space for IDT
    struct gate_descriptor *idt = (struct gate_descriptor*)0x1000;
    prints("Space for IDT allocated: ");
    printn((uint64_t)idt);
    prints("\n\r");

    // // Initialize interrupts
    struct idt_descriptor *idtr = init_idt(idt);
    prints("IDT successfully loaded. IDTR: ");
    printn((uint64_t)idtr);
    prints("\n\r");

    printn((uint64_t)get_lapic(0x30));
    prints("\n\r");

    set_port(0x64, 0xAA);

    for(;;){
        uint8_t keycode = get_port(0x60);
        printn((uint64_t)keycode);
    }

    // Catches execution and ensures no undefined code is executed
    for(;;){
        asm("hlt");
    }
}
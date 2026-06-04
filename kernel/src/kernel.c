#include <emmettos.h>

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
    prints("\x0E\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x0F\n\x12      Emmett OS      \x12\n\x10\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x11\n");
    prints("Kernel successfully loaded.\n");
    
    // Initialize available memory allocation system
    // struct mem_header *memory_map = init_memory_map(&console, efi_memory_map);
    // prints(&console, "Memory allocation initialized.\n\r");
    // struct mem_header *memory_map = NULL;

    // Allocate space for GDT
    // struct segment_descriptor *gdt = (struct segment_descriptor*)allocate_pages(&memory_map, 1);
    struct segment_descriptor *gdt = (struct segment_descriptor*)0;
    prints("Space for GDT allocated: ");
    printn((uint64_t)gdt, 64);
    prints("\n");

    // Initialize GDT
    struct gdt_descriptor *gdtr = init_gdt(gdt);
    prints("GDT successfully loaded. GDTR: ");
    printn((uint64_t)gdtr, 64);
    prints("\n");

    // Allocate space for IDT
    struct gate_descriptor *idt = (struct gate_descriptor*)0x1000;
    prints("Space for IDT allocated: ");
    printn((uint64_t)idt, 64);
    prints("\n");

    // Initialize interrupts
    struct idt_descriptor *idtr = init_idt(idt);
    prints("IDT successfully loaded. IDTR: ");
    printn((uint64_t)idtr, 64);
    prints("\n");
    init_lapic();
    prints("Local interrupt controller initialized.\n\n");

    prints("Run 'help' for a list of commands.\n");

    // Prepare console for first command
    text_colour(COLOUR_PALETTE[3]);
    prints("kernel$ ");
    reset_colour();

    // Keyboard polling
    uint8_t prev_code = 0;
    bool shift_down = false;

    flush_input();
    for(;;){
        // Retrieve scancode from the PS/2 port
        uint8_t key_code = get_port(PS2DATA);

        // Ensure that the scan code is different before handling it
        if(prev_code != key_code){
            prev_code = poll_keyboard(key_code, &shift_down);
        }
    }

    // Catches execution and ensures no undefined code is executed
    for(;;){
        asm("hlt");
    }
}
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
    printn((uint64_t)gdt);
    prints("\n");

    // Initialize GDT
    struct gdt_descriptor *gdtr = init_gdt(gdt);
    prints("GDT successfully loaded. GDTR: ");
    printn((uint64_t)gdtr);
    prints("\n");

    // Allocate space for IDT
    struct gate_descriptor *idt = (struct gate_descriptor*)0x1000;
    prints("Space for IDT allocated: ");
    printn((uint64_t)idt);
    prints("\n");

    // Initialize interrupts
    struct idt_descriptor *idtr = init_idt(idt);
    prints("IDT successfully loaded. IDTR: ");
    printn((uint64_t)idtr);
    prints("\n\n");
    prints("Press ENTER to begin:");

    // Keyboard polling
    uint8_t prev_code = 0;
    bool shift_down = false;
    for(;;){
        // Retrieve scancode from the PS/2 port
        uint8_t key_code = get_port(PS2DATA);

        // Change the state if shift key is down
        if(key_code == 0x2A){
            shift_down = true;
        } else if (key_code == 0xAA){
            shift_down = false;
        }

        if(prev_code != key_code){
            char character = convert_code(key_code);

            // Display the character
            if(shift_down){
                printc('s');
            } else {
                printc(character);
            }

            update_cursor(character);

            // Detect a sent command
            if(character == '\n'){

                // Prepare console for next command
                text_colour(COLOUR_PALETTE[3]);
                prints("kernel$ ");
                reset_colour();
            }

            prev_code = key_code;
        }
    }

    // Catches execution and ensures no undefined code is executed
    for(;;){
        asm("hlt");
    }
}
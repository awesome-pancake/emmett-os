#include <start.h>
#include <console.h>
#include <memory.h>

#ifndef LIST
    #include <list.h>
#endif

int kernel_main(struct display *disp, struct efi_memory_map *efi_memory_map) {
    // Ensure that no hardware interrupts are generated during kernel setup
    asm("cli");

    // Initialize console state
    struct console_state console = {
        .display = disp,
        .cursor_x = 0,
        .cursor_y = 0,
        .back_colour = {
            .blue = 0x00,
            .green = 0x00,
            .red = 0x00,
            .reserved = 0x00
        },
        .text_colour = {
            .blue = 0xFF,
            .green = 0xFF,
            .red = 0xFF,
            .reserved = 0x00
        }
    };

    // Clear the console
    cls(&console);
    prints(&console, "Kernel successfully loaded.\n\r");
    
    // Initialize available memory allocation system
    struct mem_header *memory_map = init_memory_map(efi_memory_map);

    // Test page allocation and deallocation
    void *ptr_1 = allocate_pages(&memory_map, 0x2);
    void *ptr_2 = allocate_pages(&memory_map, 0x3);
    free_pages(&memory_map, ptr_1);
    free_pages(&memory_map, ptr_2);
    ptr_1 = NULL;
    ptr_2 = NULL;

    display_mem(&console, memory_map);
    
    // Catches execution and ensures no undefined code is executed
    for(;;){
        asm("hlt");
    }
}
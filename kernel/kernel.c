#include <start.h>
#include <console.h>
#include <memory.h>
#include <list.h>

int kernel_main(struct display *disp, struct memory_map *memory_map)
{
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

    cls(&console);
    prints(&console, "Kernel successfully loaded.\n\r");

    display_mem(&console, memory_map);
    
    // Catches execution and ensures no undefined code is executed
    for(;;){
        asm("hlt");
    }
}

int fill_screen(struct display *disp, struct display_colour colour)
{
    uint8_t *bf = disp->frame_buffer;
    uint32_t width = disp->horizontal_resolution;
    uint32_t height = disp->vertical_resolution;

    // for(int y=0; y<height; y++){
    //     for(int x=0; x<width; x++){
    //         // Get pointer to current pixel
    //         DISPLAY_COLOUR *px = (DISPLAY_COLOUR*)(bf + sizeof(DISPLAY_COLOUR)*(y*width + x));

    //         // Fill pixel
    //         px->reserved = 0;
    //         px->blue = colour.blue;
    //         px->green = colour.green;
    //         px->red = colour.red;
    //     }
    // }

    // Optimized version of the commented code
    uint32_t pixels = width * height;
    asm volatile(
        "movl (%2), %%eax;"
        "movl %1, %%ecx;"
        "movq %0, %%rdi;"
        "cld;"
        "rep stosl;"
        :
        : "r"(bf), "r"(pixels), "r"(&colour)
        : "rax", "rcx", "rdi"
    );

    return 0;
}
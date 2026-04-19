#include "kernel.h"
#include "font.h"

int kernel_main(DISPLAY *display, EFI_MEMORY_DESCRIPTOR *memory_map)
{
    // Ensure that no hardware interrupts are generated
    asm("cli");

    CONSOLE console = {
        .display = display,
        .cursor_x = 0,
        .cursor_y = 0,
        .back_colour = {
            .blue = 0xFF,
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
    prints(&console, "Hello world from the kernel!");
    
    for(;;){
        asm("hlt");
    }
}

int fill_screen(DISPLAY *display, DISPLAY_COLOUR colour)
{
    uint8_t *bf = display->frame_buffer;
    uint32_t width = display->horizontal_resolution;
    uint32_t height = display->vertical_resolution;

    // for(int y=0; y<height; y++){
    //     for(int x=0; x<width; x++){
    //         // Get pointer to current pixel
    //         DISPLAY_COLOUR *px = (DISPLAY_COLOUR*)(bf + 4*(y*width + x));

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

int cls(CONSOLE *console)
{
    return fill_screen(console->display, console->back_colour);
}

int printc(CONSOLE *console, uint8_t c)
{
    // Frame buffer and related helpful constants
    uint8_t *bf = console->display->frame_buffer;
    uint32_t width = console->display->horizontal_resolution;
    uint32_t height = console->display->vertical_resolution;

    // Draw the character
    for(uint8_t y=0; y<16; y++){
        for(uint8_t x=0; x<8; x++){
            // Offset from start address of framebuffer
            int offset = 4*(y*width + console->cursor_y*width + x + 8*console->cursor_x);

            // Pixel pointer
            DISPLAY_COLOUR *px = (DISPLAY_COLOUR*)(bf + offset);

            // Bitwise operations to determine if the current pixel is lit
            DISPLAY_COLOUR px_colour = (fixedsys.rows[c][y] & (0x80 >> x)) != 0 ? console->text_colour : console->back_colour;

            // Draw the pixel
            px->reserved = 0;
            px->blue = px_colour.blue;
            px->green = px_colour.green;
            px->red = px_colour.red;
        }
    }
    console->cursor_x++;
    return 0;
}

int prints(CONSOLE *console, uint8_t *str)
{
    // Naive print function
    int i = 0;
    while(str[i] != '\0'){
        printc(console, str[i]);
        i++;
    }
    return 0;
}
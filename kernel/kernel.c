#include "kernel.h"
#include "font.h"

int kernel_main(DISPLAY *display, EFI_MEMORY_DESCRIPTOR *memory_map)
{
    // Ensure that no hardware interrupts are generated
    asm("cli");

    cls(display);
    prints(display, "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOGS, the quick brown fox jumped over the lazy dogs.");
    
    for(;;){
        asm("hlt");
    }
}

int fill_screen(DISPLAY *display, DISPLAY_COLOUR colour)
{
    uint8_t *bf = display->frame_buffer;
    uint32_t width = display->horizontal_resolution;
    uint32_t height = display->vertical_resolution;

    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            // Get pointer to current pixel
            DISPLAY_COLOUR *px = (DISPLAY_COLOUR*)(bf + 4*(y*width + x));

            // Fill pixel
            px->reserved = 0;
            px->blue = colour.blue;
            px->green = colour.green;
            px->red = colour.red;
        }
    }

    return 0;
}

int cls(DISPLAY *display)
{
    DISPLAY_COLOUR black;
    black.blue = 0x0;
    black.green = 0x0;
    black.red = 0x0;
    black.reserved = 0x0;

    return fill_screen(display, black);
}

int printc(DISPLAY *display, unsigned char c, uint8_t offset)
{
    // Initialize some values
    uint8_t *bf = display->frame_buffer;
    uint32_t width = display->horizontal_resolution;
    uint32_t height = display->vertical_resolution;

    // Draw the character
    for(uint8_t y=0; y<16; y++){
        for(uint8_t x=0; x<8; x++){

            // Pixel pointer
            DISPLAY_COLOUR *px = (DISPLAY_COLOUR*)(bf + 4*(y*width + x + 8*offset));

            // Bitwise operations to determine if the current pixel is lit
            uint8_t px_colour = (fixedsys.rows[c][y] & (0x80 >> x)) != 0 ? 0xFF : 0x0;

            // Draw the pixel
            px->reserved = 0;
            px->blue = px_colour;
            px->green = px_colour;
            px->red = px_colour;
        }
    }
    return 0;
}

int prints(DISPLAY *display, char *str)
{
    // Naive print function
    int i = 0;
    while(str[i] != '\0'){
        printc(display, str[i], i);
        i++;
    }
    return 0;
}
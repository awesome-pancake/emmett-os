#include "kernel.h"

// int main(DISPLAY *display, EFI_MEMORY_DESCRIPTOR *memory_map)
// {
//     kernel_main(display, memory_map);
//     for(;;);

//     return 0;
// }

void fill_screen(DISPLAY *display)
{
    uint8_t *buffer = display->frame_buffer;
    uint32_t width = display->horizontal_resolution;
    uint32_t height = display->vertical_resolution;

    for(int y=0; y<height; y++){
        for(int x=0; x<4*width; x++){
            // Avoid reserved bytes
            if(x%4 != 4){
                uint8_t *px = buffer + x + 4*y*width;
                *px = 0xAA;
            }
        }
    }
}

int kernel_main(DISPLAY *display, EFI_MEMORY_DESCRIPTOR *memory_map)
{
    // Ensure that no hardware interrupts are generated
    asm("cli");

    // Fills the screen with grey
    fill_screen(display);

    for(;;);
}
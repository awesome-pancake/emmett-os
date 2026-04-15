#include "kernel.h"

int main(DISPLAY *display, MEMORY_DESCRIPTOR *memory_map){
    // Fill in a pixel
    *display->frame_buffer = 0xFF;
    for(;;);
}
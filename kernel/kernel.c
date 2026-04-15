#include "kernel.h"

int main(DISPLAY *display, MEMORY_DESCRIPTOR *memory_map)
{
    // Fills the screen with grey
    fill_screen(display);

    for(;;);
}
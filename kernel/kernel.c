#include "kernel.h"

int kernel_main(DISPLAY *display, EFI_MEMORY_DESCRIPTOR *memory_map)
{
    // Ensure that no hardware interrupts are generated
    asm("cli");

    // Fills the screen with grey
    fill_screen(display);

    // We are able to return here since this is called by entry_point.
    return 0;
}
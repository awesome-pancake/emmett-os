#include <stdint.h>
#include <stdbool.h>

#ifndef KERNEL_ENTRY
    #define KERNEL_ENTRY 1
#endif

/* UEFI */

// EFI memory descriptor
struct efi_memory_descriptor{
    uint32_t    type;
    uint32_t    pad;
    uint64_t    physical_start;
    uint64_t    virtual_start;
    uint64_t    pages;
    uint64_t    attribute;
};

/* Memory Management */

// Segment descriptor for the GDT
struct segment_descriptor {
    uint16_t    limit_w1;
    uint16_t    base_w1;
    uint8_t     base_w2;
    uint8_t     access;
    uint8_t     flags;
    uint8_t     base_w3;
};

/* Console and Display */

// Colour for frame buffer
struct display_colour {
    uint8_t     blue;
    uint8_t     green;
    uint8_t     red;
    uint8_t     reserved;
};

// Holds information about the display
struct display{
    uint8_t     *frame_buffer;
    uint32_t    horizontal_resolution;
    uint32_t    vertical_resolution;
};

// Main kernel function
int kernel_main(struct display *disp, struct efi_memory_descriptor *memory_map);

// Fills the screen with a given colour
int fill_screen(struct display *disp, struct display_colour colour);
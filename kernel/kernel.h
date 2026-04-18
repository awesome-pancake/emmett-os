#include <stdint.h>

// EFI memory descriptor
typedef struct{
    uint32_t    type;
    uint32_t    pad;
    uint64_t    physical_start;
    uint64_t    virtual_start;
    uint64_t    pages;
    uint64_t    attribute;
} EFI_MEMORY_DESCRIPTOR;

// Segment descriptor for the GDT
typedef struct{
    uint16_t    limit_w1;
    uint16_t    base_w1;
    uint8_t     base_w2;
    uint8_t     access;
    uint8_t     flags;
    uint8_t     base_w3;
} SEGMENT_DESCRIPTOR;

// Colour for blt
typedef struct{
    uint8_t     blue;
    uint8_t     green;
    uint8_t     red;
    uint8_t     reserved;
} COLOUR;

// Holds information about the display
typedef struct{
    uint8_t     *frame_buffer;
    uint32_t    horizontal_resolution;
    uint32_t    vertical_resolution;
} DISPLAY;

// Main kernel function
int kernel_main(DISPLAY *display, EFI_MEMORY_DESCRIPTOR *memory_map);

// Fills the screen with grey
void fill_screen(DISPLAY *display);

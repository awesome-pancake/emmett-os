#include <stdint.h>
#include <stdbool.h>

#ifndef KERNEL_H
    #define KERNEL_H 1
#endif

/* UEFI */

// EFI memory descriptor
typedef struct{
    uint32_t    type;
    uint32_t    pad;
    uint64_t    physical_start;
    uint64_t    virtual_start;
    uint64_t    pages;
    uint64_t    attribute;
} EFI_MEMORY_DESCRIPTOR;

/* Memory Management */

// Segment descriptor for the GDT
typedef struct{
    uint16_t    limit_w1;
    uint16_t    base_w1;
    uint8_t     base_w2;
    uint8_t     access;
    uint8_t     flags;
    uint8_t     base_w3;
} SEGMENT_DESCRIPTOR;

/* Console and Display */

// Colour for frame buffer
typedef struct{
    uint8_t     blue;
    uint8_t     green;
    uint8_t     red;
    uint8_t     reserved;
} DISPLAY_COLOUR;

// Holds information about the display
typedef struct{
    uint8_t     *frame_buffer;
    uint32_t    horizontal_resolution;
    uint32_t    vertical_resolution;
} DISPLAY;

/* Text Processing */

// Font information
typedef struct{
    uint8_t     rows[128][16];
} FONT;

/* Functions */

// Main kernel function
int kernel_main(DISPLAY *display, EFI_MEMORY_DESCRIPTOR *memory_map);

// Fills the screen with grey
int fill_screen(DISPLAY *display, DISPLAY_COLOUR colour);

// Clears the screen
int cls(DISPLAY *display);

// Displays a character
int printc(DISPLAY *display, char c, uint8_t offset);

// Prints a null terminated string
int prints(DISPLAY *display, char *str);
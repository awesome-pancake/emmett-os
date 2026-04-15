#include <stdint.h>

// EFI memory descriptor
typedef struct{
    uint32_t    type;
    uint32_t    pad;
    uint64_t    physical_start;
    uint64_t    virtual_start;
    uint64_t    pages;
    uint64_t    attribute;
} MEMORY_DESCRIPTOR;

typedef struct{
    uint8_t     blue;
    uint8_t     green;
    uint8_t     red;
} COLOUR;

typedef struct{
    uint8_t     *frame_buffer;
    uint32_t    horizontal_resolution;
    uint32_t    vertical_resolution;
} DISPLAY;

void fill_screen(DISPLAY *display)
{
    uint8_t *buffer = display->frame_buffer;
    uint32_t width = display->horizontal_resolution;
    uint32_t height = display->vertical_resolution;

    // Fill screen with grey
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
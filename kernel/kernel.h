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
    uint8_t     *frame_buffer;
    uint32_t    horizontal_resolution;
    uint32_t    vertical_resolution;
} DISPLAY;
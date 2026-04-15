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

int main(char *frame_buffer, MEMORY_DESCRIPTOR *memory_map){
    // Fill in a pixel
    *frame_buffer = 0xFF;
    for(;;);
}
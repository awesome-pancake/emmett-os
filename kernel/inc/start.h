#include <stdint.h>
#include <stddef.h>

#ifndef KERNEL_ENTRY
    #define KERNEL_ENTRY 1
#endif


/* UEFI and boot */

typedef enum {
    EfiReservedMemoryType, 
    EfiLoaderCode, 
    EfiLoaderData, 
    EfiBootServicesCode, 
    EfiBootServicesData, 
    EfiRuntimeServicesCode, 
    EfiRuntimeServicesData, 
    EfiConventionalMemory, 
    EfiUnusableMemory, 
    EfiACPIReclaimMemory, 
    EfiACPIMemoryNVS, 
    EfiMemoryMappedIO, 
    EfiMemoryMappedIOPortSpace, 
    EfiPalCode, 
    EfiPersistentMemory, 
    EfiUnacceptedMemoryType, 
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef enum {
    PRESENT = 0x1,
    READ_WRITE = 0x2,
    USER = 0x4,
    PWT = 0x8,
    PCD = 0x10,
    ACCESSED = 0x20,
    PAGE_SIZE = 0x80,
    EXECUTE_DISABLE = 0x8000000000000000
} PAGE_SETTINGS;

// EFI memory descriptor
struct efi_memory_descriptor{
    uint32_t    type;
    uint32_t    paddingl;
    uint64_t    physical_start;
    uint64_t    virtual_start;
    uint64_t    pages;
    uint64_t    attribute;
    uint64_t    paddingh;
};

struct efi_memory_map{
    struct efi_memory_descriptor    *descriptor_table;
    uint64_t                        map_size;
};

/* Console and Display */

// Font information
struct font{
    uint8_t     rows[128][16];
};

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

// Holds state of the console
struct console_state {
    struct display              *display;
    uint8_t                     cursor_x;
    uint8_t                     cursor_y;
    struct display_colour       back_colour;
    struct display_colour       text_colour;
};

// Main kernel function
int kernel_main(struct display *disp, struct efi_memory_map *memory_map);

/* Paging and virtual memory hierarchy, starting from the bottom: */

// Initializes a page table
uint64_t *init_page_table(uint64_t *table_ptr, uint64_t target_addr, PAGE_SETTINGS flags);

// Initializes a page directory
uint64_t *init_page_directory(uint64_t *table_ptr, uint64_t target_addr, PAGE_SETTINGS flags);

// Initializes a page directory pointer table
uint64_t *init_pdpt(uint64_t *table_ptr, uint64_t target_addr, PAGE_SETTINGS flags);

// Initializes a page map level 4
uint64_t *init_pml4(uint64_t *table_ptr, uint64_t target_addr, PAGE_SETTINGS flags);
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
    PAGE_PRESENT = 0x1,
    PAGE_READ_WRITE = 0x2,
    PAGE_USER = 0x4,
    PAGE_PWT = 0x8,
    PAGE_PCD = 0x10,
    PAGE_ACCESSED = 0x20,
    PAGE_SIZE = 0x80,
    PAGE_EXECUTE_DISABLE = 0x8000000000000000
} PAGE_SETTINGS;

typedef enum {
    DESCRIPTOR_PRESENT = 0x800000000000,
    DESCRIPTOR_K_PRIVILEGE = 0x000000000000,
    DESCRIPTOR_U_PRIVELEGE = 0x600000000000,
    DESCRIPTOR_TYPE = 0x100000000000,
    DESCRIPTOR_EXECUTABLE = 0x80000000000,
    DESCRIPTOR_DIRECTION = 0x40000000000,
    DESCRIPTOR_READ_WRITE = 0x20000000000,
    DESCRIPTOR_ACCESSED = 0x10000000000
} GDT_ACCESS;

typedef enum {
    DESCRIPTOR_GRANULARITY = 0x80000000000000,
    DESCRIPTOR_SIZE = 0x40000000000000,
    DESCRIPTOR_LONG_MODE = 0x20000000000000
} GDT_FLAGS;

// Entry in the global descriptor table
struct segment_descriptor {
    uint64_t descriptor;
};

// Structure that points to the global descriptor table
struct gdt_descriptor {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed));

// EFI memory descriptor
struct efi_memory_descriptor{
    uint32_t    type;
    uint32_t    padding_l;
    uint64_t    physical_start;
    uint64_t    virtual_start;
    uint64_t    pages;
    uint64_t    attribute;
    uint64_t    padding_h;
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

// Holds the current state of the console.
extern struct console_state console;
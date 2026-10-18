#pragma once

#include <curlywaffle.h>

// UEFI memory descriptor structure
struct EfiMemoryDescriptor {
    uint32_t    Type;           // Field size is 32 bits followed by 32 bit pad
    uint32_t    Pad;
    uint64_t    PhysicalStart;
    uint64_t    VirtualStart; 
    uint64_t    NumberOfPages;
    uint64_t    Attribute;    
};

// Kernel display structure
struct Display {
        uint8_t     *frame_buffer;
        uint32_t    horizontal_resolution;
        uint32_t    vertical_resolution;
};

// UEFI memory map
struct MemoryMap {
        struct EfiMemoryDescriptor      *descriptor_table;
        uint64_t                        map_size;
};

// Holds the entry points of all kernel startup processes
struct Proc {
        void    *kernel_init;
};

// Table of boot time information that is passed to the kernel
struct BootTable {
        struct Display          display;
        struct MemoryMap        *memory_map;
        struct Proc             boot_proc;
};
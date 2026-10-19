#ifndef KERNEL_ENTRY
        #include <start.h>
        #define KERNEL_ENTRY 1
#endif

#ifndef KMEMORY
        #define KMEMORY 1
#endif

// Types of memory in the UEFI memory map
enum efi_memory_type {
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
};

// Flags for the GDT access byte
enum gdt_access {
        DESCRIPTOR_PRESENT = 0x800000000000,
        DESCRIPTOR_K_PRIVILEGE = 0x000000000000,
        DESCRIPTOR_U_PRIVELEGE = 0x600000000000,
        DESCRIPTOR_TYPE = 0x100000000000,
        DESCRIPTOR_EXECUTABLE = 0x80000000000,
        DESCRIPTOR_DIRECTION = 0x40000000000,
        DESCRIPTOR_READ_WRITE = 0x20000000000,
        DESCRIPTOR_ACCESSED = 0x10000000000
};

// Flags for a segment descriptor in the GDT
enum gdt_flags {
        DESCRIPTOR_GRANULARITY = 0x80000000000000,
        DESCRIPTOR_SIZE = 0x40000000000000,
        DESCRIPTOR_LONG_MODE = 0x20000000000000
};

// Flags for a 48 byte chunk in the heap
enum kmalloc_flags {
        KMALLOC_FREE,   // Index of free flag in kmalloc_chunk
};

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

// Stores the UEFI memory map
struct efi_memory_map{
        struct efi_memory_descriptor    *descriptor_table;
        uint64_t                        map_size;
};

// Metadata for stack-like dynamic memory allocation
struct heap {
        uint64_t    size;       // Maximum size of the heap in bytes including overhead
        void        *top;       // Points to the top of the heap
};

// A chunk of memory in the heap for kmalloc
struct kmalloc_chunk {
        uint64_t    size;       // Size of the chunk in bytes including overhead
        bool        flags[8];   // Holds the state of the chunk, see kmalloc_flags
};

// Displays the efi memmap
int display_efi_mem(struct efi_memory_map *memory_map);

// Initializes the gdt
struct gdt_descriptor *init_gdt(struct segment_descriptor *addr);

// Reloads segments
extern void load_segments(uint16_t cs_selector, uint16_t ds_selector);

// Allocates a 48 byte chunk on the heap.
void *kmalloc(struct heap *heap);

// Initializes a heap structure.
struct heap *init_heap(void *address, uint64_t page_size);

extern const int MAP_SIZE;
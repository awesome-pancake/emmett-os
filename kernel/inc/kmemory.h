#ifndef KERNEL_ENTRY
    #include <start.h>
    #define KERNEL_ENTRY 1
#endif

#ifndef KMEMORY
    #define KMEMORY 1
#endif

// Types of memory that are available
typedef enum {
    RESERVED,
    AVAILABLE,
    PROCESS_TEXT,
    PROCESS_DATA,
    MMIO
} memory_type;

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

// Stores the UEFI memory map
struct efi_memory_map{
    struct efi_memory_descriptor    *descriptor_table;
    uint64_t                        map_size;
};

// Metadata at the start of every chunk of memory
struct mem_header {
    memory_type         type;
    uint32_t            pid;
    uint32_t            size;       // Size in pages
    uint32_t            reserved;   // Required to maintain 16 byte alignment
    struct mem_header   *fd;        // These two are only for unallocated memory chunks
    struct mem_header   *bk; 
};

// Metadata for stack-like dynamic memory allocation
struct heap {
    uint64_t     size;      // Maximum size of the heap in bytes
    void        *base;      // Points to the base of the heap
    void        *top;       // Points to the top of the heap
    uint64_t    padding;    // Explicitly pads heap metadata for 16 byte alignment
};

// Displays the efi memmap
int display_efi_mem(struct efi_memory_map *memory_map);

// Displays the currently available memory
int display_mem(struct mem_header *memory_map);

// Initializes available memory
struct mem_header *init_memory_map(struct efi_memory_map *memory_map);

// Allocates a certain number of free pages
void *allocate_pages(struct mem_header **memory_map, int pages);

// Frees previously allocated pages
void free_pages(struct mem_header **memory_map, void *ptr);

// Initializes the gdt
struct gdt_descriptor *init_gdt(struct segment_descriptor *addr);

// Reloads segments
extern void load_segments(uint16_t cs_selector, uint16_t ds_selector);

// Moves the heap pointer upwards by a certain number of bytes and returns a pointer to the allocated space.
void *push_heap(struct heap *curr_heap, int bytes);

// Initializes a heap structure.
struct heap *init_heap();

extern const int MAP_SIZE;
#ifndef KERNEL_ENTRY
    #include <start.h>
    #define KERNEL_ENTRY 1
#endif

#ifndef LIST
    #include <list.h>
    #define LIST 1
#endif

extern const int DESCRIPTOR_SIZE;

// Types of memory that are available
typedef enum {
    RESERVED,
    AVAILABLE,
    PROCESS_TEXT,
    PROCESS_DATA,
    MMIO
} memory_type;

// Map of the address space
struct memory_map {
    struct list         list;
    memory_type         type;
    uint64_t            pid;
    uint64_t            physical_address;
    uint64_t            virtual_address;
    uint64_t            pages;
};

// Chunk structure for heap data storage
struct memory_chunk {
    uint64_t            chunk_size; // Size in bytes including overhead
    struct list         *list;      // Links to the chunk before and after in the bin
};

// State of the heap
struct heap_state {
    uint64_t                flags;
    struct memory_chunk     *top_chunk; // Pointer to topmost chunk of the stack
    struct memory_chunk     *bins[1];    // List of every bin
};

// Initializes the kernel heap at a given page-aligned address
struct heap_state *init_heap(void *ptr, int page_size);

// Displays the efi memmap
int display_efi_mem(struct console_state *console, struct efi_memory_map *memory_map);

// Displays the kernel memory map
int display_mem(struct console_state *console, struct memory_map *memory_map);

// Uses the efi memory map to locate free pages, but does not change the memory map
void *efi_alloc_page(struct efi_memory_map *memory_map);

// Initializes the kernel memory map
struct memory_map *init_memory_map(struct console_state *console, struct efi_memory_map *memory_map, struct memory_map *ptr);
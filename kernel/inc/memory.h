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

// Metadata at the start of every chunk of memory
struct mem_header {
    memory_type         type;
    uint32_t            pid;
    uint32_t            size;       // Size in pages
    uint32_t            reserved;   // Required to maintain 16 byte alignment
    struct mem_header   *fd;        // These two are only for unallocated memory chunks
    struct mem_header   *bk; 
};

// Displays the efi memmap
int display_efi_mem(struct console_state *console, struct efi_memory_map *memory_map);

// Displays the currently available memory
int display_mem(struct console_state *console, struct mem_header *memory_map);

// Initializes available memory
struct mem_header *init_memory_map(struct efi_memory_map *memory_map);

// Allocates a certain number of free pages
void *allocate_pages(struct mem_header **memory_map, int pages);

// Frees previously allocated pages
void free_pages(struct mem_header **memory_map, void *ptr);
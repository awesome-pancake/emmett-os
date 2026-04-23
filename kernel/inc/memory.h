#ifndef KERNEL_ENTRY
    #include <start.h>
    #define KERNEL_ENTRY 1
#endif

#ifndef LIST
    #include <list.h>
    #define LIST 1
#endif

extern const int DESCRIPTOR_SIZE;

typedef enum {
    RESERVED,
    AVAILABLE,
    PROCESS,
    KERNEL,
    MMIO
} memory_type;

struct memory_map {
    memory_type         type;
    uint64_t            pid;
    uint64_t            physical_address;
    uint64_t            virtual_address;
    uint64_t            pages;
    struct list         list;
};

// Displays the memmap
int display_mem(struct console_state *console, struct efi_memory_map *memory_map);
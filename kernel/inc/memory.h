#ifndef KERNEL_ENTRY
    #include <start.h>
    #define KERNEL_ENTRY 1
#endif

// Displays the memmap
int display_mem(struct console_state *console, struct efi_memory_descriptor *memory_map);
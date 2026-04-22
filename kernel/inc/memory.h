#ifndef KERNEL_ENTRY
    #include <start.h>
    #define KERNEL_ENTRY 1
#endif

// Displays the memmap
int display_mem(struct console_state *console, struct memory_map *memory_map);
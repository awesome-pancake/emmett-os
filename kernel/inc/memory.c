#include <memory.h>
#include <console.h>

const int DESCRIPTOR_SIZE = sizeof(struct efi_memory_descriptor);

int display_mem(struct console_state *console, struct memory_map *memory_map)
{
    // Some title stuff
    prints(console, "Type:              ");
    prints(console, "Physical Start:    ");
    prints(console, "Virtual Start:     ");
    prints(console, "Pages:             ");
    prints(console, "Attribute:         \n\r");

    // Display every memory descriptor
    for(int i=0; i<memory_map->map_size/DESCRIPTOR_SIZE; i++){
        printn(console, memory_map->descriptor_table[i].type);
        printc(console, ' ');
        printn(console, memory_map->descriptor_table[i].physical_start);
        printc(console, ' ');
        printn(console, memory_map->descriptor_table[i].virtual_start);
        printc(console, ' ');
        printn(console, memory_map->descriptor_table[i].pages);
        printc(console, ' ');
        printn(console, memory_map->descriptor_table[i].attribute);
        prints(console, "\n\r");
    }

    return 0;
}
#include <memory.h>
#include <console.h>

const int DESCRIPTOR_SIZE = sizeof(struct efi_memory_descriptor);

int display_efi_mem(struct console_state *console, struct efi_memory_map *memory_map) {
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

int display_mem(struct console_state *console, struct mem_header *memory_map) {

    struct mem_header *curr_node = memory_map;

    prints(console, "Start:             Size:             \n\r");
    do {
        // Iterates through the memory map, displaying every entry
        printn(console, (uint64_t)curr_node);
        printc(console, ' ');
        printn(console, curr_node->size);
        prints(console, "\n\r");
        curr_node = (struct mem_header*)curr_node->fd;

    } while (curr_node != memory_map);

    return 0;
}

struct mem_header *init_memory_map(struct efi_memory_map *memory_map) {

    uint32_t current_type = 0;
    int page_id = -1;

    // Iteratively search the memory map
    while(current_type != EfiConventionalMemory && page_id+1<memory_map->map_size/DESCRIPTOR_SIZE){
        page_id++;
        current_type = memory_map->descriptor_table[page_id].type;
    }
    
    // Initialize linked list
    struct mem_header *map_head = (struct mem_header*)memory_map->descriptor_table[page_id].physical_start;
    struct mem_header *curr_node = map_head;
    struct mem_header *new_node = NULL;

    curr_node->fd = map_head;
    curr_node->bk = map_head;

    // Iterate through the efi memory map, adding a linked list node for every descriptor
    for(int i=0; i<memory_map->map_size/DESCRIPTOR_SIZE; i++){

        // Initialize new header at start of available memory chunk
        if(memory_map->descriptor_table[i].type == EfiConventionalMemory){
            new_node = (struct mem_header*)memory_map->descriptor_table[i].physical_start;
            new_node->size = memory_map->descriptor_table[i].pages;
            new_node->type = AVAILABLE;
            new_node->pid = 0;

            // Connect to other headers
            new_node->bk = curr_node;
            new_node->fd = map_head;
            curr_node->fd = new_node;
            map_head->bk = new_node;

            // Move current node
            curr_node = new_node;
        }
    }

    return map_head;
}

void* allocate_pages(struct console_state *console, struct mem_header *memory_map, int pages) {

    void *output_ptr = NULL;
    struct mem_header *curr_node = memory_map;

    // Find available chunks of adequate size
    do {

        if(curr_node->size > pages){ // Set new header for rest of chunk

            output_ptr = (void*)&curr_node->fd; // Gives address to overwrite pointers but not metadata
            break;
        } 
        else if (curr_node->size == pages) { // Link forward and backwards headers

            prints(console, "Yes! This one works!\n\r");

            output_ptr = (void*)&curr_node->fd;

            curr_node->fd->bk = curr_node->bk;
            curr_node->bk->fd = curr_node->fd;

            break;
        }
        curr_node = (struct mem_header*)curr_node->fd;

    } while (curr_node != memory_map);

    return output_ptr;
}
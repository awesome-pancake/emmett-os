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

int display_mem(struct console_state *console, struct memory_map *memory_map) {

    struct memory_map *curr_node = memory_map;

    do {
        // Iterates through the memory map, displaying every entry
        printn(console, curr_node->physical_address);
        prints(console, "\n\r");
        curr_node = (struct memory_map*)curr_node->list.fd;

    } while (curr_node != memory_map);

    return 0;
}

struct heap_state *init_heap(void* ptr, int page_size) {

    // Find pointers to state and start location of heap
    struct heap_state *state_ptr = (struct heap_state*)ptr;
    struct memory_chunk *heap_ptr = (struct memory_chunk*)(ptr + sizeof(struct heap_state));

    // Set heap state
    state_ptr->flags = 0;
    state_ptr->top_chunk = heap_ptr;
    state_ptr->bins[0] = heap_ptr;

    return state_ptr;
}

void *efi_alloc_page(struct efi_memory_map *memory_map) {
    void *output_ptr = NULL;
    uint32_t current_type = 0;
    int i = -1;

    // Iteratively search the memory map
    while(current_type != EfiConventionalMemory && i+1<memory_map->map_size/DESCRIPTOR_SIZE){
        i++;
        current_type = memory_map->descriptor_table[i].type;
    }
    output_ptr = (void*)memory_map->descriptor_table[i].physical_start;

    return output_ptr;
}

struct memory_map *init_memory_map(struct console_state *console, struct efi_memory_map *memory_map, struct memory_map *ptr) {
    
    // Initialize linked list
    struct memory_map *map_head = ptr;
    struct memory_map *curr_node = map_head;
    map_head->list.fd = &map_head->list;
    map_head->list.bk = &map_head->list;

    // Iterate through the efi memory map, adding a linked list node for every descriptor
    for(int i=0; i<memory_map->map_size/DESCRIPTOR_SIZE; i++){

        // Determine the address and data of the new node
        struct memory_map *new_node = ptr + i*sizeof(struct memory_map);
        new_node->physical_address = memory_map->descriptor_table[i].physical_start;
        new_node->virtual_address = memory_map->descriptor_table[i].virtual_start;
        new_node->pages = memory_map->descriptor_table[i].pages;
        new_node->pid = 0;
        // TODO: convert from efi memory type to kernel memory type

        list_add(&new_node->list, &curr_node->list);

        curr_node = new_node;
    }

    return map_head;
}

void *alloc_page(struct memory_map *memory_map, enum memory_type type) {

    void *page_ptr = NULL;
    struct memory_map *curr_node = memory_map;

    do {
        if(curr_node->type == AVAILABLE){
            page_ptr = (void*)curr_node->physical_address;

            // Commented until heap is possible
            // curr_node->type = type;
        }
        curr_node = (struct memory_map*)curr_node->list.fd;

    } while (curr_node != memory_map && page_ptr == NULL);

    return page_ptr;
}
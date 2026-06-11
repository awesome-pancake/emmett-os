// Memory management
// Emmett Hoffman
// June 12, 2026
// 
// Description:
// - Functions for initializing the PS/2 legacy keyboard controller are found here.
// - Additionally, keyboard input handling functions are found here.
//
// Extra for Experts:
// - Initialized the global descriptor table
// - Investigated GCC compiler attributes (__attribute__((packed)))

#ifndef KMEMORY
	#include <kmemory.h>
	#define KMEMORY 1
#endif

#ifndef CONSOLE
	#include <console.h>
	#define CONSOLE 1
#endif

// Different definition of a null pointer since the address 0 may be used.
#define K_NULL ((void*)0xFFFFFFFFFFFFFFFF);

const int MAP_SIZE = sizeof(struct efi_memory_descriptor);

int display_efi_mem(struct efi_memory_map *memory_map) {
	// Some title stuff
	prints("Type:              ");
	prints("Physical Start:    ");
	prints("Virtual Start:     ");
	prints("Pages:             ");
	prints("Attribute:         \n\r");

	// Display every memory descriptor
	for(int i=0; i<memory_map->map_size/DESCRIPTOR_SIZE; i++){
		printn(memory_map->descriptor_table[i].type, 64);
		printc(' ');
		printn(memory_map->descriptor_table[i].physical_start, 64);
		printc(' ');
		printn(memory_map->descriptor_table[i].virtual_start, 64);
		printc(' ');
		printn(memory_map->descriptor_table[i].pages, 64);
		printc(' ');
		printn(memory_map->descriptor_table[i].attribute, 64);
		prints("\n\r");
	}

	return 0;
}

int display_mem(struct mem_header *memory_map) {

	struct mem_header *curr_node = memory_map;

	prints("Start:             Size:             \n\r");
	do {
		// Iterates through the memory map, displaying every entry
		printn((uint64_t)curr_node, 64);
		printc(' ');
		printn(curr_node->size, 64);
		prints("\n\r");
		curr_node = (struct mem_header*)curr_node->fd;

	} while (curr_node != memory_map);

	return 0;
}

struct mem_header *init_memory_map(struct efi_memory_map *memory_map) {

	prints("Initializing memory allocation...\n\r");

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

void *allocate_pages(struct mem_header **memory_map, int pages) {

	void *output_ptr = K_NULL;
	struct mem_header *curr_node = *memory_map;

	// Find available chunks of adequate size
	do {

		if(curr_node->size > pages){ // If the chunk size is greater than the page requirement

			output_ptr = (void*)&curr_node->fd; // Gives address to overwrite pointers but not metadata

			// Set new header
			struct mem_header *new_header = (struct mem_header*)(4096*pages + (uint64_t)curr_node);
			new_header->type = curr_node->type;
			new_header->size = curr_node->size - pages;
			new_header->reserved = curr_node->reserved;

			// Insert new header
			new_header->fd = curr_node->fd;
			new_header->bk = curr_node->bk;
			curr_node->bk->fd = new_header;
			curr_node->fd->bk = new_header;
			curr_node->size = pages;
			
			// Handle if memory allocation is at the start of the list
			if(curr_node == *memory_map){
				*memory_map = new_header;
			}
			
			break;
		} 
		else if (curr_node->size == pages) { // If the page requirement exactly meets the chunk size

			// Handle if the memory occurs at the start of the list
			if(curr_node == *memory_map){
				*memory_map = curr_node->fd;
			}

			// Set the output
			output_ptr = (void*)&curr_node->fd;

			// Remove the current node from the list
			curr_node->fd->bk = curr_node->bk;
			curr_node->bk->fd = curr_node->fd;
			curr_node->size = pages;

			break;
		}
		curr_node = (struct mem_header*)curr_node->fd;

	} while (curr_node != *memory_map);

	return output_ptr;
}

void free_pages(struct mem_header **memory_map, void *ptr) {

	struct mem_header *curr_node = *memory_map;
	struct mem_header *free_node = (struct mem_header*)((char*)ptr - 0x10); // Recovers the address for the start of the page header
	struct mem_header *next_node = NULL;
	struct mem_header *prev_node = NULL;
	
	do {
		next_node = (struct mem_header*)(free_node->size*4096 + (uint64_t)free_node);
		prev_node = curr_node->bk;

		if((struct mem_header*)(prev_node->size*4096 + (uint64_t)prev_node) == free_node){

			// Patch together adjacent memory blocks
			prev_node->fd = curr_node->fd;
			prev_node->size += curr_node->size + free_node->size;
			curr_node->fd->bk = prev_node;

			return;

		} else if(next_node == curr_node){

			// Patch together two adjecent free memory blocks
			free_node->fd = curr_node->fd;
			free_node->bk = curr_node->bk;
			
			curr_node->fd->bk = free_node;
			curr_node->bk->fd = free_node;
			free_node->size += curr_node->size;

			// Handles if the next node is the map head
			if(next_node == *memory_map) {
				*memory_map = free_node;
			}

			return;
		}

		curr_node = curr_node->fd;

	} while (curr_node != *memory_map);

	// Handles if chunk is not adjacent to any free chunks
	curr_node = *memory_map;
	do {
		prev_node = curr_node->bk;
		if(((uint64_t)free_node < (uint64_t)curr_node && (uint64_t)free_node < (uint64_t)prev_node) || (uint64_t)free_node < (uint64_t)*memory_map) {

			// Insert chunk
			prev_node->fd = free_node;
			curr_node->bk = free_node;
			free_node->fd = curr_node;
			free_node->bk = prev_node;

			// Handle if node is before head node
			if(curr_node == *memory_map){
				*memory_map = free_node;
			}

			return;
		}

		curr_node = curr_node->fd;

	} while (curr_node != *memory_map);
}

struct gdt_descriptor *init_gdt(struct segment_descriptor *addr) {

	uint64_t cs_flags = DESCRIPTOR_ACCESSED | DESCRIPTOR_READ_WRITE | DESCRIPTOR_EXECUTABLE | DESCRIPTOR_TYPE | 
		DESCRIPTOR_K_PRIVILEGE | DESCRIPTOR_PRESENT | DESCRIPTOR_GRANULARITY | DESCRIPTOR_LONG_MODE;
	
	uint64_t ds_flags = DESCRIPTOR_ACCESSED | DESCRIPTOR_READ_WRITE | DESCRIPTOR_TYPE | DESCRIPTOR_K_PRIVILEGE | 
		DESCRIPTOR_PRESENT | DESCRIPTOR_GRANULARITY;

	// Required void entry
	addr->descriptor = 0x0000000000000000;

	// Code segment
	(addr+1)->descriptor = 0xF00000000FFFF | cs_flags;

	// Data segment
	(addr+2)->descriptor = 0xF00000000FFFF | ds_flags;

	// Points to the gdtr structure
	struct gdt_descriptor *gdt_ptr = (struct gdt_descriptor*)0xFE0;
	
	gdt_ptr->size = 0x17;
	gdt_ptr->offset = (uint64_t)addr;

	// Loads the location of the global descriptor table
	asm(
		"lgdt %0"
		:
		:"m"(*gdt_ptr)
	);

	load_segments(0x0008, 0x0010);

	return gdt_ptr;
}

void *kmalloc(struct heap *heap, int bytes) {

        void *ret_pointer = NULL;

        // Claim the free chunk at the top if the size is right
        if(heap->top != NULL && bytes <= 48) {
                ret_pointer = (void*)((struct kmalloc_chunk*)heap->top + 1);
                ((struct kmalloc_chunk*)heap->top)->flags[KMALLOC_FREE] = false;
        } else if (heap->top != NULL && bytes > 48) {
                // TODO: implement the case for large allocation sizes
        }

        struct kmalloc_chunk *top_chunk = (struct kmalloc_chunk*)heap + (heap->size/sizeof(struct kmalloc_chunk));
        void *temp_top = heap->top;
        
        // Adjust the top pointer
        for(struct kmalloc_chunk *chunk_ptr = (struct kmalloc_chunk*)heap->top; (uint64_t)chunk_ptr < (uint64_t)top_chunk; chunk_ptr += chunk_ptr->size/sizeof(struct kmalloc_chunk)) {
                if(chunk_ptr->flags[KMALLOC_FREE]) {
                        heap->top = chunk_ptr;
                        break;
                }
        }

        // Adjust the top pointer if the heap is full
        if(temp_top == heap->top) {
                heap->top = NULL;
        }

	return ret_pointer;
}

struct heap *init_heap(void *address, uint64_t page_size) {

	// Initialize the heap
	struct heap *new_heap = (struct heap*)address;
	new_heap->size = page_size * 0x1000;
	new_heap->top = (uint8_t*)address + sizeof(struct heap);

	// Fill the heap with initialized 64 byte chunks
	for(int i=0; i*64 < page_size*0x1000 - sizeof(struct heap); i++){

		// Locate the chunk to be initialized
		struct kmalloc_chunk *new_chunk = (struct kmalloc_chunk*)((uint8_t*)new_heap->top + i*64);

		// Initialize the data in the chunk
		new_chunk->flags[KMALLOC_FREE] = true;
		new_chunk->size = 64;
	}

	return new_heap;
}
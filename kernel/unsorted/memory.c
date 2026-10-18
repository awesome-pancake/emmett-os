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

void *kmalloc(struct heap *heap) {

        void *ret_pointer = NULL;

        // Claim the free chunk at the top if the size is right
        if(heap->top != NULL) {
                ret_pointer = (void*)((struct kmalloc_chunk*)heap->top + 1);
                ((struct kmalloc_chunk*)heap->top)->flags[KMALLOC_FREE] = false;
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
#include <start.h>
#include <console.h>
#include <memory.h>

#ifndef LIST
    #include <list.h>
#endif

int kernel_main(struct display *disp, struct efi_memory_map *efi_memory_map) {
    // Ensure that no hardware interrupts are generated during kernel setup
    asm("cli");

    // Initialize console state
    struct console_state console = {
        .display = disp,
        .cursor_x = 0,
        .cursor_y = 0,
        .back_colour = {
            .blue = 0x00,
            .green = 0x00,
            .red = 0x00,
            .reserved = 0x00
        },
        .text_colour = {
            .blue = 0xFF,
            .green = 0xFF,
            .red = 0xFF,
            .reserved = 0x00
        }
    };

    // Clear the console
    cls(&console);
    prints(&console, "Kernel successfully loaded.\n\r");
    
    // Initialize available memory allocation system
    // struct mem_header *memory_map = init_memory_map(&console, efi_memory_map);
    // prints(&console, "Memory allocation initialized.\n\r");
    struct mem_header *memory_map = NULL;

    // Allocate space for GDT
    // struct segment_descriptor *gdt = (struct segment_descriptor*)allocate_pages(&memory_map, 1);
    struct segment_descriptor *gdt = (struct segment_descriptor*)0;
    prints(&console, "Space for GDT allocated: ");
    printn(&console, (uint64_t)gdt);
    prints(&console, "\n\r");

    // Initialize GDT
    init_gdt(&console, memory_map, gdt);
    prints(&console, "GDT successfully loaded somehow??\n\r");
    
    // Catches execution and ensures no undefined code is executed
    for(;;){
        asm("hlt");
    }
}

uint64_t *init_page_table(uint64_t *table_ptr, uint64_t target_addr, PAGE_SETTINGS flags) {

    // Fills a page table
    for(int i=0; i<512; i++){
        *(table_ptr + i) = ((target_addr + 0x1000 * i) & 0xFFFFFFFFF000) | flags;
    }
    return table_ptr;
}

uint64_t *init_page_directory(uint64_t *table_ptr, uint64_t target_addr, PAGE_SETTINGS flags) {

    // Fills a page directory
    for(int i=0; i<512; i++){
        *(table_ptr + i) = ((target_addr + 0x1000 * i) & 0xFFFFFFFFF000) | flags;
    }
    return table_ptr;
}

uint64_t *init_pdpt(uint64_t *table_ptr, uint64_t target_addr, PAGE_SETTINGS flags) {

    uint64_t page_size = 0x1000;

    // Change the size of the page depending on the PS flag
    if(flags & PAGE_SIZE) {
        page_size = 0x1000000000;
    }

    // Fills a page directory pointer table
    for(int i=0; i<512; i++){
        *(table_ptr + i) = ((target_addr + page_size * i) & 0xFFFFFFFFF000) | flags;
    }
    return table_ptr;
}

uint64_t *init_pml4(uint64_t *table_ptr, uint64_t target_addr, PAGE_SETTINGS flags) {

    // Fills a page map level 4 table
    for(int i=0; i<512; i++){
        *(table_ptr + i) = ((target_addr + 0x1000 * i) & 0xFFFFFFFFF000) | flags;
    }
    return table_ptr;
}

uint64_t *init_paging(struct mem_header **memory_map) {
    
    return 0;
}

void init_gdt(struct console_state *console, struct mem_header *memory_map, struct segment_descriptor *addr) {

    uint64_t cs_flags = DESCRIPTOR_ACCESSED | DESCRIPTOR_READ_WRITE | DESCRIPTOR_EXECUTABLE | DESCRIPTOR_TYPE | 
        DESCRIPTOR_K_PRIVILEGE | DESCRIPTOR_PRESENT | DESCRIPTOR_GRANULARITY;
    
    uint64_t ds_flags = DESCRIPTOR_ACCESSED | DESCRIPTOR_READ_WRITE | DESCRIPTOR_TYPE | DESCRIPTOR_K_PRIVILEGE | 
        DESCRIPTOR_PRESENT | DESCRIPTOR_GRANULARITY | DESCRIPTOR_LONG_MODE;

    addr->descriptor_l = 0x0000000000000000; // Void entry
    addr->descriptor_h = 0x0000000000000000;

    // Code segment
    (addr+1)->descriptor_l = 0x0000000000000000 | cs_flags;
    (addr+1)->descriptor_h = 0x0000000000000000;

    // Data segment
    (addr+2)->descriptor_l = 0x0000000000000000 | ds_flags;
    (addr+2)->descriptor_h = 0x0000000000000000;

    // struct gdt_descriptor *gdt_ptr = (struct gdt_descriptor*)allocate_pages(&memory_map, 1);
    struct gdt_descriptor *gdt_ptr = (struct gdt_descriptor*)0x1000;

    printn(console, (uint64_t)gdt_ptr);
    
    gdt_ptr->size = 47;
    gdt_ptr->offset = (uint64_t)addr;

    asm(
        "lgdt %0"
        :
        :"m"(*gdt_ptr)
    );
}
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
    struct mem_header *memory_map = init_memory_map(efi_memory_map);

    // Allocates space for a page table and tests it
    uint64_t *page_table = (uint64_t*)allocate_pages(&memory_map, 1) - 2;
    uint64_t *page_dir = (uint64_t*)allocate_pages(&memory_map, 1) - 2;
    uint64_t *pdpt = (uint64_t*)allocate_pages(&memory_map, 1) - 2;
    uint64_t *pml4 = (uint64_t*)allocate_pages(&memory_map, 1) - 2;

    // Initialize the page system
    init_page_table(page_table, 0x000000000000, PRESENT | READ_WRITE);
    init_page_directory(page_dir, (uint64_t)page_table, READ_WRITE);
    init_page_directory(pdpt, (uint64_t)page_dir, READ_WRITE);
    init_page_directory(pml4, (uint64_t)pdpt, READ_WRITE);

    *(pml4 + 384) = ((uint64_t)pdpt & 0xFFFFFFFFF000) | PRESENT | READ_WRITE;
    *pdpt = ((uint64_t)page_dir & 0xFFFFFFFFF000) | PRESENT | READ_WRITE;
    *page_dir = ((uint64_t)page_table & 0xFFFFFFFFF000) | PRESENT | READ_WRITE;

    for(int i=0; i<512; i++){
        printn(&console, *(pdpt + i));
        printc(&console, ' ');
    }
    
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

    // Fills a page directory pointer table
    for(int i=0; i<512; i++){
        *(table_ptr + i) = ((target_addr + 0x1000 * i) & 0xFFFFFFFFF000) | flags;
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
    
    uint64_t *pml4 = (uint64_t*)allocate_pages(memory_map, 1);
    uint64_t *pdpt = (uint64_t*)allocate_pages(memory_map, 1);
    uint64_t *pdir = (uint64_t*)allocate_pages(memory_map, 1);
    uint64_t *ptab = (uint64_t*)allocate_pages(memory_map, 1);

    // Fill the pml4 with non present pages
    for(int i=0; i<512; i++){
        *(pml4 + i) = READ_WRITE;
    }

    // Set the 384th entry of the pml4, corresponds to 0xC0...
    // Chunk granted R/W, present, and supervisor permissions.
    *(pml4 + 384) = ((uint64_t)pdpt & 0x0000FFFFFFFFF000) | PRESENT | READ_WRITE;

    // Fill the pdpt with non present pages
    for(int i=0; i<512; i++){
        *(pdpt + i) = READ_WRITE;
    }

    // Set the 0th entry of the page directory pointer table
    *pdpt = ((uint64_t)pdir & 0x0000FFFFFFFFF000) | PRESENT | READ_WRITE;

    // Fill the page directory with non present pages
    for(int i=0; i<512; i++){
        *(pdir + i) = READ_WRITE;
    }

    // Set the 0th entry of the page directory
    *pdir = ((uint64_t)ptab & 0x0000FFFFFFFFF000) | PRESENT | READ_WRITE;

    // Fill the page table with the first 512 pages
    for(int i=0; i<512; i++){
        *(ptab + i) = (0x1000 * i) | PRESENT | READ_WRITE;
    }

    return pml4;
}
#include <start.h>
#include <console.h>
#include <memory.h>
#include <interrupt.h>

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
        .cursor_y = 4,
        .back_colour = {
            .blue = 0x0E,
            .green = 0x0A,
            .red = 0x1A,
            .reserved = 0x00
        },
        .text_colour = {
            .blue = 0xFF,
            .green = 0xFF,
            .red = 0xFF,
            .reserved = 0x00
        }
    };

    struct console_state console_red = {
        .display = disp,
        .cursor_x = 0,
        .cursor_y = 0,
        .back_colour = {
            .blue = 0x15,
            .green = 0x15,
            .red = 0xD7,
            .reserved = 0x00
        },
        .text_colour = {
            .blue = 0x0E,
            .green = 0x0A,
            .red = 0x1A,
            .reserved = 0x00
        }
    };

    // Clear the console
    cls(&console);
    prints(&console_red, "                                                                        \n\r Emmett OS                                                              \n\r                                                                        \n");
    prints(&console, "Kernel successfully loaded.\n\r");
    
    // Initialize available memory allocation system
    // struct mem_header *memory_map = init_memory_map(&console, efi_memory_map);
    // prints(&console, "Memory allocation initialized.\n\r");
    // struct mem_header *memory_map = NULL;

    // Allocate space for GDT
    // struct segment_descriptor *gdt = (struct segment_descriptor*)allocate_pages(&memory_map, 1);
    struct segment_descriptor *gdt = (struct segment_descriptor*)0;
    prints(&console, "Space for GDT allocated: ");
    printn(&console, (uint64_t)gdt);
    prints(&console, "\n\r");

    // Initialize GDT
    struct gdt_descriptor *gdtr = init_gdt(&console, gdt);
    prints(&console, "GDT successfully loaded. GDTR: ");
    printn(&console, (uint64_t)gdtr);
    prints(&console, "\n\r");

    // Allocate space for IDT
    struct gate_descriptor *idt = (struct gate_descriptor*)0x1000;
    prints(&console, "Space for IDT allocated: ");
    printn(&console, (uint64_t)idt);
    prints(&console, "\n\r");

    // // Initialize interrupts
    struct idt_descriptor *idtr = init_idt(&console, idt);
    prints(&console, "IDT successfully loaded. IDTR: ");
    printn(&console, (uint64_t)idtr);
    prints(&console, "\n\r");
    
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

struct gdt_descriptor *init_gdt(struct console_state *console, struct segment_descriptor *addr) {

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

    printn(console, *(uint64_t*)0xFE2);
    prints(console, "\n\r");

    // Loads the location of the global descriptor table
    asm(
        "lgdt %0"
        :
        :"m"(*gdt_ptr)
    );

    load_segments(0x0008, 0x0010);

    return gdt_ptr;
}

struct idt_descriptor *init_idt(struct console_state *console, struct gate_descriptor *addr) {

    // Points to the currently used interrupt.
    void (*curr_interrupt)() = &int_DE;

    printn(console, (uint64_t)curr_interrupt);
    prints(console, "\n\r");

    for(int i=0; i<256; i++){
        (addr + i)->offset_l = (uint16_t)(0xFFFF & (uint64_t)curr_interrupt);
        (addr + i)->selector = 0x0008; // Points to code segment
        (addr + i)->stack_table = 0;
        (addr + i)->attributes = 0x8E; // Interrupt gate
        (addr + i)->offset_m = (uint16_t)((0xFFFF0000 & (uint64_t)curr_interrupt) >> 16);
        (addr + i)->offset_h = (uint32_t)((0xFFFFFFFF00000000 & (uint64_t)curr_interrupt) >> 32);
        (addr + i)->reserved = 0;
    }

    printn(console, *(uint64_t*)addr);
    prints(console, "\n\r");
    printn(console, *((uint64_t*)addr+1));
    prints(console, "\n\r");

    struct idt_descriptor *idt_ptr = (struct idt_descriptor*)0x2000;
    
    idt_ptr->size = 0xFFF;
    idt_ptr->offset = (uint64_t)addr;

    printn(console, *(uint64_t*)0x2002);
    prints(console, "\n\r");

    // Loads the location of the interrupt descriptor table
    asm(
        "lidt %0;"
        :
        :"m"(*idt_ptr)
    );

    int x = 1/0;

    return idt_ptr;
}
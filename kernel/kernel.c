#include <start.h>
#include <console.h>
#include <memory.h>

int kernel_main(struct display *disp, struct efi_memory_descriptor *memory_map)
{
    // Ensure that no hardware interrupts are generated
    asm("cli");

    struct console_state console = {
        .display = disp,
        .cursor_x = 0,
        .cursor_y = 0,
        .back_colour = {
            .blue = 0xFF,
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

    cls(&console);
    prints(&console, "Hello world from the kernel! BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH\n\r");
    prints(&console, "Hello again! 0123456789 {} () [] | ~ ` ' \\ // C:/Users/Emmett !@#$%^&*-+_=\n\r");
    printn(&console, 65537);
    prints(&console, "\n\r");

    display_mem(&console, memory_map);
    
    for(;;){
        asm("hlt");
    }
}

int fill_screen(struct display *disp, struct display_colour colour)
{
    uint8_t *bf = disp->frame_buffer;
    uint32_t width = disp->horizontal_resolution;
    uint32_t height = disp->vertical_resolution;

    // for(int y=0; y<height; y++){
    //     for(int x=0; x<width; x++){
    //         // Get pointer to current pixel
    //         DISPLAY_COLOUR *px = (DISPLAY_COLOUR*)(bf + sizeof(DISPLAY_COLOUR)*(y*width + x));

    //         // Fill pixel
    //         px->reserved = 0;
    //         px->blue = colour.blue;
    //         px->green = colour.green;
    //         px->red = colour.red;
    //     }
    // }

    // Optimized version of the commented code
    uint32_t pixels = width * height;
    asm volatile(
        "movl (%2), %%eax;"
        "movl %1, %%ecx;"
        "movq %0, %%rdi;"
        "cld;"
        "rep stosl;"
        :
        : "r"(bf), "r"(pixels), "r"(&colour)
        : "rax", "rcx", "rdi"
    );

    return 0;
}

int cls(struct console_state *console)
{
    return fill_screen(console->display, console->back_colour);
}

int printc(struct console_state *console, uint8_t c)
{
    // Frame buffer and related helpful constants
    uint8_t *bf = console->display->frame_buffer;
    uint32_t width = console->display->horizontal_resolution;
    uint32_t height = console->display->vertical_resolution;

    // Draw the character
    for(uint8_t y=0; y<16; y++){
        for(uint8_t x=0; x<8; x++){
            // Offset from start address of framebuffer
            int offset = sizeof(struct display_colour)*(y*width + FONT_HEIGHT*console->cursor_y*width + x + FONT_WIDTH*console->cursor_x);

            // Pixel pointer
            struct display_colour *px = (struct display_colour*)(bf + offset);

            // Bitwise operations to determine if the current pixel is lit
            struct display_colour px_colour = (FIXEDSYS.rows[c][y] & (0x80 >> x)) != 0 ? console->text_colour : console->back_colour;

            // Draw the pixel
            px->reserved = 0;
            px->blue = px_colour.blue;
            px->green = px_colour.green;
            px->red = px_colour.red;
        }
    }

    // Determine the new position of the cursor
    switch(c){
        case '\n':
            console->cursor_y++;
            break;
        case '\r':
            console->cursor_x = 0;
            break;
        default:
            if(console->cursor_x + 1 == width/FONT_WIDTH){
                console->cursor_x = 0;
                console->cursor_y++;
            } else {
                console->cursor_x++;
            }
            break;
    }

    return 0;
}

int prints(struct console_state *console, uint8_t *str)
{
    int i = 0;
    while(str[i] != '\0'){
        printc(console, str[i]);
        i++;
    }
    return 0;
}

int printn(struct console_state *console, uint64_t num)
{
    char nib_to_hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char hex_string[11] = {'0', 'x'};

    for(int x=0; x<8; x++){
        unsigned char nibble = ((0xF0000000>>(4*x)) & num) >> (4*(7-x));
        hex_string[x+2] = nib_to_hex[nibble];
    }

    return prints(console, hex_string);
}

int display_mem(struct console_state *console, struct efi_memory_descriptor *memory_map)
{
    // TODO: Remove this function
    printn(console, memory_map[1].type);
    prints(console, "\n\r");
    printn(console, memory_map[1].pages);
    prints(console, "\n\r");
    printn(console, memory_map[1].physical_start);
    prints(console, "\n\r");
    printn(console, memory_map[1].virtual_start);

    return 0;
}
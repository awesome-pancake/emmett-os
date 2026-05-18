#ifndef KERNEL_ENTRY
    #include <start.h>
#endif

// Fills the screen with a given colour
int fill_screen(struct display *disp, struct display_colour colour);

// Clears the screen
int cls();

// Displays a character
int printc(char c);

// Prints a null terminated string
int prints(char *str);

// Prints a number
int printn(uint64_t num);

extern const uint16_t FONT_WIDTH;
extern const uint16_t FONT_HEIGHT;
extern const struct font FIXEDSYS;
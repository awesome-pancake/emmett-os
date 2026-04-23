#ifndef KERNEL_ENTRY
    #include <start.h>
#endif

// Fills the screen with a given colour
int fill_screen(struct display *disp, struct display_colour colour);

// Clears the screen
int cls(struct console_state *console);

// Displays a character
int printc(struct console_state *console, char c);

// Prints a null terminated string
int prints(struct console_state *console, char *str);

// Prints a number
int printn(struct console_state *console, uint64_t num);

extern const uint16_t FONT_WIDTH;
extern const uint16_t FONT_HEIGHT;

extern const struct font FIXEDSYS;
#ifndef KERNEL_ENTRY
    #include <start.h>
#endif

// Fills the screen with a given colour
int fill_screen(struct display *disp, struct display_colour colour);

// Clears the screen
int cls();

// Displays a character at the current console cursor position
int printc(char c);

// Prints a null terminated string
int prints(char *str);

// Prints a number
int printn(uint64_t num);

// Moves the cursor according to the character
void update_cursor(char character);

// Draws a rainbow
int rainbow();

// Changes the console text colour
int text_colour(struct display_colour colour);

// Changes the console background colour
int fill_colour(struct display_colour colour);

// Resets the colour to the console defaults
void reset_colour();

// Shifts the entire console one cursor position upward
void move_console();

extern const uint16_t FONT_WIDTH;
extern const uint16_t FONT_HEIGHT;
extern const struct font FIXEDSYS;
extern const struct display_colour COLOUR_PALETTE[8];
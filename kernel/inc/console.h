#ifndef KERNEL_ENTRY
    #include <start.h>
    #define KERNEL_ENTRY 1
#endif

#ifndef CONSOLE
    #define CONSOLE 1
#endif

// Font information
struct font{
    uint8_t     rows[128][16];
};

// Colour for frame buffer
struct display_colour {
    uint8_t     blue;
    uint8_t     green;
    uint8_t     red;
    uint8_t     reserved;
};

// Holds information about the display
struct display{
    uint8_t     *frame_buffer;
    uint32_t    horizontal_resolution;
    uint32_t    vertical_resolution;
};

// Holds state of the console
struct console_state {
    struct display              *display;
    uint8_t                     cursor_x;
    uint8_t                     cursor_y;
    struct display_colour       back_colour;
    struct display_colour       text_colour;
};

// Fills the screen with a given colour
int fill_screen(struct display *disp, struct display_colour colour);

// Clears the screen
int cls();

// Prints instructions
int help();

// Displays a character at the current console cursor position
int printc(char c);

// Prints a null terminated string
int prints(char *str);

// Prints a number
int printn(uint64_t num);

// Prints an error
int error(char *str);

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
extern struct console_state console;    // Holds the state of the console
#ifndef KERNEL_ENTRY
    #include <start.h>
    #define KERNEL_ENTRY 1
#endif

#ifndef KEYBOARD
    #define KEYBOARD 1
#endif

// Wrapper for the CONVERT_CODE array
char convert_code(uint8_t scan_code);

// Initializes the PS/2 controller
void init_ps2();

// Updates the state of the console based on keyboard polling and returns the current scan code
uint8_t poll_keyboard(uint8_t scan_code, bool *shift_state);

// Some global variables for keyboard polling
extern char input_buffer[32];

// Constants relevant to the legacy keyboard

extern const char CONVERT_CODE[256];
extern const uint8_t PS2COMMAND;
extern const uint8_t PS2DATA;
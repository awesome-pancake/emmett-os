#ifndef KERNEL_ENTRY
    #include <start.h>
    #define KERNEL_ENTRY 1
#endif

#ifndef KEYBOARD
    #define KEYBOARD 1
#endif

// Length of the input stream in bytes
#define BUFFER_LENGTH (65)

// Wrapper for the CONVERT_CODE array
char convert_code(uint8_t scan_code);

// Initializes the PS/2 controller
void init_ps2();

// Updates the state of the console based on keyboard polling and returns the current scan code
uint8_t poll_keyboard(uint8_t scan_code, bool *shift_state);

// Parses a command from the command line.
int parse_command(int argc, char **argv);

// Resets the input stream
void flush_input();

// Some global variables for keyboard polling
extern char input_buffer[BUFFER_LENGTH];

// Maximum size of the arguments variable
extern const int ARGV_SIZE;

// Constants relevant to the legacy keyboard
extern const char CONVERT_CODE[256];
extern const uint8_t PS2COMMAND;
extern const uint8_t PS2DATA;
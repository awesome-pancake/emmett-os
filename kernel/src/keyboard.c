// Legacy keyboard implementation
// Emmett Hoffman
// June 12, 2026
// 
// Description:
// - Functions for initializing the PS/2 legacy keyboard controller are found here.
// - Additionally, keyboard input handling functions are found here.
//
// Extra for Experts:
// - Used port commands (in, out) to communicate directly with legacy keyboard.

#ifndef KEYBOARD
    #include <keyboard.h>
    #define KEYBOARD 1
#endif

#ifndef CONSOLE
    #include <console.h>
    #define CONSOLE 1
#endif

#ifndef KSTRING
    #include <kstring.h>
    #define KSTRING 1
#endif

#ifndef COMMAND
    #include <command.h>
    #define COMMAND 1
#endif

char convert_code(uint8_t scan_code) {
    return CONVERT_CODE[scan_code];
}

void init_ps2() {
    // TODO:
    // Disable both ports
    // Do some stuff
    // Enable devices
    // Success
}

uint8_t poll_keyboard(uint8_t scan_code, bool *shift_state){

    if(scan_code == 0x2A) {             // Handles shift down events
        *shift_state = true;
        return scan_code;
    } else if (scan_code == 0xAA) {     // Handles shift up events
        *shift_state = false;
        return scan_code;
    }

    char character = convert_code(scan_code);

    // Do nothing if null
    if(character == '\0'){
        return scan_code;
    }

    // Display the character
    if(*shift_state){
        char upper_char = ktoupper(character);
        printc(upper_char);

        if(character != '\n' && character != '\b'){
            kstrncat(input_buffer, &upper_char, 1);
        }
    } else {
        printc(character);

        if(character != '\n' && character != '\b'){
            kstrncat(input_buffer, &character, 1);
        }
    }

    // Doesn't allow backspace beyond the start of the input buffer and fixes some newline formatting stuff
    if((character != '\b' || input_buffer[0] != '\0') && character != '\n'){
        update_cursor(character);
    }

    // Handles backspaces
    int input_length = kstrlen(input_buffer);
    if(character == '\b') {
        input_buffer[input_length - 1] = '\0';
    }

    // Continue execution if a sent command is detected
    if(character != '\n'){
        return scan_code;
    }

    char *save_ptr = NULL;
    char *token = kstrtok_r(input_buffer, ' ', &save_ptr);

    int argc = 0;
    char *argv[32];

    // Iterate through every token
    while(token != NULL && input_length > 0){

        argv[argc] = token;

        // Ensure argv does not overflow
        if(argc < ARGV_SIZE - 1){
            argc += 1;
        } else {
            error("\nToo many command arguments. Parsing first 32:");
            break;
        }

        // Find next token
        token = kstrtok_r(NULL, ' ', &save_ptr);
    }

    // Set the null terminator of argv
    argv[argc] = NULL;

    // Parse the command and reset the console
    if(input_length > 0){
        parse_command(argc, argv);
    }
    flush_input();

    // Prepare console for next command
    text_colour(COLOUR_PALETTE[3]);
    prints("\n");
    prints("kernel$ ");
    reset_colour();

    return scan_code;
}

void flush_input() {
    
    for(int i=0; i<BUFFER_LENGTH; i++){
        input_buffer[i] = '\0';
    }
}

char input_buffer[BUFFER_LENGTH] = {0};    // Console input stream

const uint8_t PS2COMMAND = 0x64;    // PS/2 command port
const uint8_t PS2DATA = 0x60;       // PS/2 data port

// Converts a PS/2 scancode into a character
const char CONVERT_CODE[256] = {
    '\0', '\0', '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\0',
    'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  'o',  'p',  '[',  ']',  '\n', '\0', 'a',  's',
    'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  '\'', '`',  '\0', '\\', 'z',  'x',  'c',  'v',
    'b',  'n',  'm',  ',',  '.',  '/',  '\0', '*',  '\0', ' ',  '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
};
// Command Handling
// Emmett Hoffman
// June 12, 2026
// 
// Description:
// - Every command in the console is implemented here.
// - Each command is passed the standard arguments for a main function in C:
//     - eg. echo(int argc, char **argv);
//     - argv stores text arguments, and argc stores the number of text arguments
//
// Extra for Experts:
// - String operations in C

#ifndef COMMAND
    #include <command.h>
    #define COMMAND 1
#endif

#ifndef CONSOLE
    #include <console.h>
    #define CONSOLE 1
#endif

#ifndef KSTRING
    #include <kstring.h>
    #define KSTRING 1
#endif

int parse_command(int argc, char **argv) {

    bool parse_state = false; // Set to false if reading a command, and true if reading arguments
    int (*command)(int, char**) = NULL; // Command to execute

    for(int i=0; i<argc; i++){

        char *token = argv[i];

        // Rainbow command
        if(!parse_state && kstrncmp("rainbow", token, 8) == 0){
            parse_state = true;
            prints("\n");
            command = &rainbow;
        }

        // Clear screen command
        if(!parse_state && kstrncmp("cls", token, 4) == 0){
            parse_state = true;
            prints("\n");
            command = &cls;
        }

        // Help command
        if(!parse_state && kstrncmp("help", token, 5) == 0){
            parse_state = true;
            prints("\n");
            command = &help;
        }

        // Echo command
        if(!parse_state && kstrncmp("echo", token, 5) == 0){
            parse_state = true;
            prints("\n");
            command = &echo;
        }

        if(!parse_state && kstrncmp("peek", token, 5) == 0){
            parse_state = true;
            prints("\n");
            command = &peek;
        }

        if(!parse_state && kstrncmp("poke", token, 5) == 0){
            parse_state = true;
            prints("\n");
            command = &poke;
        }

        // Sends an unknown command error
        if(!parse_state){
            error("\nCould not find command.");
            return -1;
        }
    }

    return (*command)(argc, argv);
}

uint8_t *parse_env_variable(char *token) {

    if(kstrncmp("$cursor_x", token, 10) == 0){
        return &console.cursor_x;
    }

    if(kstrncmp("$cursor_y", token, 10) == 0){
        return &console.cursor_y;
    }

    if(kstrncmp("$back_r", token, 8) == 0){
        return &console.back_colour.red;
    }

    if(kstrncmp("$back_g", token, 8) == 0){
        return &console.back_colour.green;
    }

    if(kstrncmp("$back_b", token, 8) == 0){
        return &console.back_colour.blue;
    }

    if(kstrncmp("$text_r", token, 8) == 0){
        return &console.text_colour.red;
    }

    if(kstrncmp("$text_g", token, 8) == 0){
        return &console.text_colour.green;
    }

    if(kstrncmp("$text_b", token, 8) == 0){
        return &console.text_colour.blue;
    }

    return 0;
}

int help(int argc, char **argv){

    // Prints it all
    prints("Commands:\n");
    prints("cls - Clears the console\n");
    prints("help [command] - Displays a help prompt, optionally about a command\n");
    prints("echo [text] - Echoes text to the console output\n");
    prints("peek [address] - Displays the byte held at an address in memory\n");
    prints("poke [address] [value] - Sets the byte at an address in memory\n");
    prints("rainbow - Prints a rainbow to the screen\n\n");
    prints("Environment variables:\n");
    prints("$cursor_x - X position of the cursor.\n");
    prints("$cursor_y - Y position of the cursor.\n");
    prints("$back_r - The red component of the background colour.\n");
    prints("$back_g - The green component of the background colour.\n");
    prints("$back_b - The blue component of the background colour.\n");
    prints("$text_r - The red component of the text colour.\n");
    prints("$text_g - The green component of the text colour.\n");
    prints("$text_b - The blue component of the text colour.\n\n");
    prints("Using environment variables:\n");
    prints("To use environment variables, substitute the name of the variable as an argument for a command. eg. echo $text_g will print the contents of the $text_g variable.\n");
    return 0;
}

int cls(int argc, char **argv) {

    // Clears the console and resets the cursor position
    console.cursor_x = 0;
    console.cursor_y = 0;
    return fill_screen(console.display, console.back_colour);
}

int rainbow(int argc, char **argv) {

    // Blends together the colours of the rainbow using special characters and background colours
    for(int i=0; i<7; i++){
        fill_colour(COLOUR_PALETTE[i+1]);
        text_colour(COLOUR_PALETTE[i]);
        prints("\x04\x03\x02\x01");
    }

    reset_colour();

    return 0;
}

int echo(int argc, char **argv) {

    // Print every argument passed to echo except the first
    for(int i=1; i<argc; i++) {

        // Environment variable
        if(argv[i][0] == '$') {
            uint8_t *var_value = parse_env_variable(argv[i]);
            printn(*var_value, 8);
            prints(" ");
        } else {
            prints(argv[i]);
            prints(" ");
        }
    }

    return 0;
}

int peek(int argc, char **argv) {

    uint64_t address = 0;

    if(argc != 2){
        error("Invalid operand(s) for peek instruction.");
        return -1;
    }

    if(argv[1][0] == '$') { // Handles environment variables
        printn(*parse_env_variable(argv[1]), 8);
    } else {                // Handles regular arguments
        address = katoi(argv[1]);
        printn(*(uint64_t*)address, 8);
    }

    return 0;
}

int poke(int argc, char **argv) {

    uint64_t address = 0;
    uint64_t value = 0;

    if(argc != 3){
        error("Invalid operand(s) for poke instruction.");
        return -1;
    }

    if(argv[1][0] == '$') { // Handles environment variables for address
        address = (uint64_t)parse_env_variable(argv[1]);
    } else {                // Handles regular arguments
        address = katoi(argv[1]);
    }

    if(argv[2][0] == '$') { // Handles environment variables for value
        value = (uint64_t)*parse_env_variable(argv[2]);
    } else {                // Handles regular arguments
        value = katoi(argv[2]);
    }

    // Set the value
    *(uint64_t*)address = value;

    return 0;
}

const int ARGV_SIZE = 33;
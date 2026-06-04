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
    // char *curr_command = NULL;

    for(int i=0; i<argc; i++){

        char *token = argv[i];

        // Rainbow command
        if(!parse_state && kstrncmp("rainbow", token, 8) == 0){
            parse_state = true;
            prints("\n");
            rainbow();
        }

        // Clear screen command
        if(!parse_state && kstrncmp("cls", token, 4) == 0){
            parse_state = true;
            prints("\n");
            cls();
        }

        // Help command
        if(!parse_state && kstrncmp("help", token, 5) == 0){
            parse_state = true;
            prints("\n");
            help();
        }

        // Echo command
        if(!parse_state && kstrncmp("echo", token, 5) == 0){
            parse_state = true;
            prints("\n");
            echo(argc, argv);
            break;
        }

        if(!parse_state && kstrncmp("debug", token, 6) == 0){
            parse_state = true;
            prints("\n");
            printn(katoi(argv[1]), 64);
            break;
        }

        if(!parse_state && kstrncmp("peek", token, 5) == 0){
            parse_state = true;
            prints("\n");
            peek(argc, argv);
            break;
        }

        if(!parse_state && kstrncmp("poke", token, 5) == 0){
            parse_state = true;
            prints("\n");
            poke(argc, argv);
            break;
        }

        // Sends an unknown command error
        if(!parse_state){
            error("\nCould not find command.");
            return -1;
        }
    }
    return 0;
}

int help(){

    // Prints it all
    prints("Commands:\n");
    prints("cls - Clears the console.\n");
    prints("help - Displays a help prompt.\n");
    prints("echo [input] - Echoes text to the console output\n");
    prints("peek [address] - Displays the byte held at an address\n");
    prints("poke [address] [value] - Sets the byte at an address\n");
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
    prints("To use environment variables, substitute the name of the variable as an argument for the command. eg. echo $text_g will print the contents of the $text_g variable.\n");
    return 0;
}

int cls() {

    // Clears the console and resets the cursor position
    console.cursor_x = 0;
    console.cursor_y = 0;
    return fill_screen(console.display, console.back_colour);
}

int rainbow() {

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
        prints(argv[i]);
        prints(" ");
    }

    return 0;
}

int peek(int argc, char **argv) {

    if(argc != 2){
        error("Invalid operand for peek instruction.");
        return -1;
    }

    uint64_t address = katoi(argv[1]);
    printn(*(uint64_t*)address, 8);

    return 0;
}

int poke(int argc, char **argv) {

    if(argc != 3){
        error("Invalid operand(s) for poke instruction.");
        return -1;
    }

    uint64_t address = katoi(argv[1]);
    uint64_t value = katoi(argv[2]);

    *(uint64_t*)address = value;

    return 0;
}

const int ARGV_SIZE = 33;
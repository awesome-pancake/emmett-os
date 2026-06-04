#ifndef KERNEL_ENTRY
    #include <start.h>
    #define KERNEL_ENTRY 1
#endif

#ifndef COMMAND
    #define COMMAND 1
#endif

// Parses a command from the command line.
int parse_command(int argc, char **argv);

/* Terminal Commands */

// Prints terminal instructions
int help();

// Clears the screen
int cls();

// Draws a rainbow
int rainbow();

// Echoes text to the console
int echo(int argc, char **argv);

// Displays the byte stored at a memory address
int peek(int argc, char **argv);

// Sets a byte at a memory address
int poke(int argc, char **argv);

// Maximum size of the arguments variable
extern const int ARGV_SIZE;
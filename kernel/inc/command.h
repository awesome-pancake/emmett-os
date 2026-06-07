#ifndef KERNEL_ENTRY
    #include <start.h>
    #define KERNEL_ENTRY 1
#endif

#ifndef COMMAND
    #define COMMAND 1
#endif

// Parses a command from the command line.
int parse_command(int argc, char **argv);

// Parses an environment variable
uint8_t *parse_env_variable(char *token);

/* Terminal Commands */

// Prints terminal instructions
int help(int argc, char **argv);

// Clears the screen
int cls(int argc, char **argv);

// Draws a rainbow
int rainbow(int argc, char **argv);

// Echoes text to the console
int echo(int argc, char **argv);

// Displays the byte stored at a memory address
int peek(int argc, char **argv);

// Sets a byte at a memory address
int poke(int argc, char **argv);

// Maximum size of the arguments variable
extern const int ARGV_SIZE;
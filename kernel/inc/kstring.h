#ifndef KERNEL_ENTRY
        #include <start.h>
#endif

#ifndef KSTRING
        #define KSTRING 1
#endif

// Below are a couple functions from string.h in the C library that I rewrote for this project.

// Finds the length of a given string not including the null terminator
int kstrlen(char *str);

// Compares two strings against one another
int kstrcmp(char *s1, char *s2);

// Compares two strings with a character limit
int kstrncmp(char *s1, char *s2, int n);

/*
Splits a string into tokens.

The first call takes the string as input and returns the first token. Subsequent calls will return the remaining tokens one at a time and take NULL as an input for the string.
*/
char *kstrtok_r(char *str, const char delim, char **saveptr);

// Determines if a character is in a string
char *kstrchr(char *str, char c);

// Concatenates src to the end of dest. The space allocated for dest must be at least equal to the sum of src and dest.
char *kstrcat(char *dest, char *src);

// Converts a character into uppercase, including some special keyboard characters like . to >
char ktoupper(uint8_t ch);

// Concatenates src to the end of dest with a character limit. The space allocated for dest must be at least equal to the sum of src and dest.
char *kstrncat(char *dest, char *src, int n);

// Checks if a character is a digit
bool kisdigit(char c);

// Converts a number stored in a string into a number
uint64_t katoi(const char *nstr);
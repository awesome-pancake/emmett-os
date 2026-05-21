#ifndef KERNEL_ENTRY
    #include <start.h>
#endif

// Below are a couple functions from string.h in the C library that I rewrote for this project.

// Finds the length of a given string not including the null terminator
int kstrlen(char *str);

// Compares two strings against one another
int kstrcmp(char *s1, char *s2);

// Splits a string into tokens
char *kstrtok_r(char *str, const char delim, char **saveptr);

// Determines if a character is in a string
char *kstrchr(char *str, char c);

// Concatenates src to the end of dest. The space allocated for dest must be at least equal to the sum of src and dest.
char *kstrcat(char *dest, char *src);
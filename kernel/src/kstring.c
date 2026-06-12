// String operations
// Emmett Hoffman
// June 12, 2026
// 
// Description:
// - This file contains many helpful string operations for parsing console commands.
// - All functions implemented here use C standard library function names with a k at the front.

#ifndef KSTRING
        #include <kstring.h>
        #define KSTRING 1
#endif

#ifndef CONSOLE
        #include <console.h>
        #define CONSOLE 1
#endif

int kstrlen(char *str) {

        int count;
        for(count=0; str[count] != '\0'; count++);
        return count;
}

int kstrcmp(char *s1, char *s2) {

        // Iterate through both strings, terminating if differences in either are detected or if both strings terminate
        for(int i=0; s1[i] != '\0' || s2[i] != '\0'; i++){
                if(s1[i] - s2[i] > 0){
                        return 1;
                } else if (s1[i] - s2[i] < 0){
                        return -1;
                }
        }
        return 0;
}

int kstrncmp(char *s1, char *s2, int n) {

        // Iterate through both strings, exiting if the limit is reached or if either has a null terminator
        for(int i=0; i<n && (s1[i] != '\0' || s2[i] != '\0'); i++){
                if(s1[i] - s2[i] > 0){
                        return 1;
                } else if (s1[i] - s2[i] < 0){
                        return -1;
                }
        }
        return 0;
}

char *kstrtok_r(char *str, const char delim, char **saveptr) {

        // Sets the initial state of the tokenizer
        if(str != NULL){
                *saveptr = str;
        }

        // Determine if the end of the string has been reached
        if(str == NULL && *saveptr == NULL){
                return NULL;
        }

        char *output = *saveptr;
        char *delim_location = kstrchr(*saveptr, delim);

        // Check if any more deliminator characters are present
        if(delim_location != NULL){
                *saveptr = (delim_location + 1);
                *delim_location = '\0';
        } else {
                *saveptr = NULL;
        }

        return output;
}

char *kstrchr(char *str, char c) {

        // Iterate through the string and check for the character
        for(int i=0; str[i]!='\0'; i++){
                if(str[i] == c){
                        return (str + i);
                }
        }
        return NULL;
}

char *kstrcat(char *dest, char *src) {

        int dest_length = kstrlen(dest);

        int i;
        for(i=0; src[i] != '\0'; i++){
                dest[dest_length + i] = src[i];
        }
        dest[dest_length+i] = '\0';

        return dest;
}

char ktoupper(uint8_t ch) {

        // Handles lowercase characters
        if(ch >= 97 && ch <= 122){
                return ch ^ 32;
        }

        // Handles some numbers
        if(ch == 49 || (ch >= 51 && ch <= 53)){
                return ch - 16;
        }

        // Some miscellaneous characters
        if(ch == '2'){ return '@';}
        if(ch == '.'){ return '>';}
        if(ch == ','){ return '<';}
        if(ch == '/'){ return '?';}
        if(ch == ';'){ return ':';}
        if(ch == '\''){ return '"';}
        if(ch == '-'){ return '_';}
        if(ch == '='){ return '+';}
        if(ch == '6'){ return '^';}
        if(ch == '7'){ return '&';}
        if(ch == '8'){ return '*';}
        if(ch == '9'){ return '(';}
        if(ch == '0'){ return ')';}
        if(ch == '`'){ return '~';}
        if(ch == '['){ return '{';}
        if(ch == ']'){ return '}';}
        if(ch == '\\'){ return '|';}

        // Returns the character if no matches were found
        return ch;
}

char *kstrncat(char *dest, char *src, int n) {

        int dest_length = kstrlen(dest);

        int i;
        for(i=0; i < n; i++){
                dest[dest_length + i] = src[i];
        }
        dest[dest_length+i] = '\0';

        return dest;
}

bool kisdigit(char c) {
        return (c <= 0x39 && c >= 0x30);
}

uint64_t katoi(const char *nstr) {

        uint64_t num = 0;

        // Parse hexadecimal
        if(nstr[0] == 'x') {
                
                for(int i=0; nstr[i] != '\0'; i++) {
                        num *= 16;

                        if(kisdigit(nstr[i])){  // Handles regular digits
                                num += (uint64_t)nstr[i] - 0x30;
                        } 
                        else if (kstrchr("abcdefABCDEF", nstr[i]) != NULL) {    // Handles hexadecimal digits
                                num += (uint64_t)ktoupper(nstr[i]) - 55;
                        }
                }
        }

        // Parse decimal numbers otherwise
        for(int i=0; nstr[i] != '\0' && kisdigit(nstr[i]); i++) {
                num *= 10;
                num += (uint64_t)nstr[i] - 0x30;
        }

        return num;
}
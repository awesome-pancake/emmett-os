#include <kstring.h>
#include <console.h>

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
#ifndef KERNEL_ENTRY
    #include <start.h>
#endif

typedef enum {
    E_SUCCESS,
    E_INVALID_PARAMETER = 0xFFFFFFFF,
} STATUS;
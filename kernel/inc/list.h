#ifndef LIST
    #define LIST 1
#endif


// Doubly linked list.
struct list {
    struct list     *fd;
    struct list     *bk;
};

// Adds a list node immediately following the list head
int list_add(struct list *new, struct list *head);

// Adds a list node at the end of a list
int list_add_tail(struct list *new, struct list *head);

// Returns the parent struct of a given list node.
// Output must be casted to a pointer to the parent type.
static inline void* container_of(struct list *head, int offset){
    return (void*)(head-offset);
};
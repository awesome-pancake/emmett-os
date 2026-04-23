
// Doubly linked list.
struct list {
    void            *container;     // Normally void pointers are a crime, but in this case the alternative is an evil macro.
    struct list     *fd;
    struct list     *bk;
};

// Adds a list node immediately following the list head
int list_add(struct list *new, struct list *head);

// Adds a list node at the end of a list
int list_add_tail(struct list *new, struct list *head);
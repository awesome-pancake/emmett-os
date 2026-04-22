
// Doubly linked list, similar to the linux kernel's implementation
struct list {
    struct list     *fd;
    struct list     *bk;
};

int comp_test();
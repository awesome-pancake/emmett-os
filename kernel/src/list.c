#include <list.h>

int list_add(struct list *new, struct list *head) {
    // Set new pointers
    new->bk = head;
    new->fd = head->fd;

    // Set head pointers
    head->fd->bk = new;
    head->fd = new;
    return 0;
}

int list_add_tail(struct list *new, struct list *head) {
    // Set new pointers
    new->fd = head;
    new->bk = head->bk;

    // Set head pointers
    head->bk->fd = new;
    head->bk = new;
    return 0;
}
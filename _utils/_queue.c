#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *item;

    unsigned int st;
    unsigned int en;
    unsigned int length;
} QueueTAD;

QueueTAD* queue_create(unsigned int len) {
    QueueTAD *queue = (QueueTAD *)malloc(sizeof(QueueTAD));
    queue->item = (int *)malloc(len * sizeof(int));

    queue->st = 0;
    queue->en = queue->st;
    queue->length = len;

    return(queue);
}

void queue_destroy(QueueTAD *queue) {
    free(queue->item);
    free(queue);
}

short queue_isempty(QueueTAD *queue) {
    return(queue->en == queue->st);
}

short queue_push(QueueTAD *queue, int item) {
    unsigned int next = (queue->en + 1) % queue->length;
    if(next == queue->st) return(0);

    queue->item[queue->en] = item;
    queue->en = next;
    return(1);
}

int queue_pop(QueueTAD *queue) {
    unsigned int slot = queue->st;
    queue->st = (queue->st + 1) % queue->length;

    return(queue->item[slot]);
}

int main() {
    QueueTAD *queue = queue_create(5);

    queue_push(queue, 777);
    int item = queue_pop(queue);
    printf("Got %d. Empty queue: %d", item, queue_isempty(queue));

    return 0;
}

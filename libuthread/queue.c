#include <stdlib.h>
#include "queue.h"

typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node_t;

struct queue {
    queue_node_t *head;
    queue_node_t *tail;
    int length;
};

queue_t queue_create(void) {
    queue_t q = malloc(sizeof(*q));
    if (!q) return NULL;
    q->head = q->tail = NULL;
    q->length = 0;
    return q;
}

int queue_destroy(queue_t queue) {
    if (!queue || queue->length != 0)
        return -1;
    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data) {
    if (!queue || !data) return -1;
    queue_node_t *node = malloc(sizeof(*node));
    if (!node) return -1;
    node->data = data;
    node->next = NULL;
    if (!queue->tail) {
        queue->head = queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
    queue->length++;
    return 0;
}

int queue_dequeue(queue_t queue, void **data) {
    if (!queue || queue->length == 0 || !data)
        return -1;
    queue_node_t *node = queue->head;
    *data = node->data;
    queue->head = node->next;
    if (!queue->head) queue->tail = NULL;
    free(node);
    queue->length--;
    return 0;
}

int queue_delete(queue_t queue, void *data) {
    if (!queue || queue->length == 0 || !data)
        return -1;
    queue_node_t *prev = NULL, *curr = queue->head;
    while (curr) {
        if (curr->data == data) {
            if (prev)
                prev->next = curr->next;
            else
                queue->head = curr->next;
            if (curr == queue->tail)
                queue->tail = prev;
            free(curr);
            queue->length--;
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }
    return -1;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg) {
    if (!queue || !func)
        return -1;
    queue_node_t *curr = queue->head;
    while (curr) {
        queue_node_t *next = curr->next;
        func(queue, curr->data, arg);
        curr = next;
    }
    return 0;
}

int queue_length(queue_t queue) {
    if (!queue)
        return -1;
    return queue->length;
}

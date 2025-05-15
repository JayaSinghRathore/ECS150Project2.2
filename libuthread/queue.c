#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

typedef struct queue_node {
    void *data;
    struct queue_node *prev;
    struct queue_node *next;
} queue_node_t;

struct queue {
    queue_node_t *head;
    queue_node_t *tail;
    int length;
};

queue_t queue_create(void)
{
    queue_t q = malloc(sizeof(struct queue));
    if (!q) return NULL;
    q->head = q->tail = NULL;
    q->length = 0;
    return q;
}

int queue_destroy(queue_t queue)
{
    if (!queue) return -1;
    if (queue->length != 0) return -1;
    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
    if (!queue || !data) return -1;
    queue_node_t *node = malloc(sizeof(queue_node_t));
    if (!node) return -1;
    node->data = data;
    node->next = NULL;
    node->prev = queue->tail;
    if (queue->tail)
        queue->tail->next = node;
    else
        queue->head = node;
    queue->tail = node;
    queue->length++;
    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
    if (!queue || !data) return -1;
    if (queue->length == 0) return -1;
    queue_node_t *node = queue->head;
    *data = node->data;
    queue->head = node->next;
    if (queue->head)
        queue->head->prev = NULL;
    else
        queue->tail = NULL;
    free(node);
    queue->length--;
    return 0;
}

int queue_delete(queue_t queue, void *data)
{
    if (!queue || !data) return -1;
    queue_node_t *curr = queue->head;
    while (curr) {
        if (curr->data == data) {
            if (curr->prev)
                curr->prev->next = curr->next;
            else
                queue->head = curr->next;
            if (curr->next)
                curr->next->prev = curr->prev;
            else
                queue->tail = curr->prev;
            free(curr);
            queue->length--;
            return 0;
        }
        curr = curr->next;
    }
    return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
    if (!queue || !func) return -1;
    queue_node_t *curr = queue->head;
    queue_node_t *next;
    while (curr) {
        next = curr->next;
        func(queue, curr->data);
        curr = next;
    }
    return 0;
}

int queue_length(queue_t queue)
{
    if (!queue) return -1;
    return queue->length;
}


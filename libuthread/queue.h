#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue* queue_t;
typedef void (*queue_func_t)(queue_t queue, void *data, void *arg);

queue_t queue_create(void);
int queue_destroy(queue_t queue);
int queue_enqueue(queue_t queue, void* data);
int queue_dequeue(queue_t queue, void** data);
int queue_delete(queue_t queue, void* data);
int queue_iterate(queue_t queue, queue_func_t func, void* arg);
int queue_length(queue_t queue);

#endif /* QUEUE_H */

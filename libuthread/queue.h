#ifndef _QUEUE_H
#define _QUEUE_H

typedef struct queue* queue_t;

queue_t queue_create(void);
int queue_destroy(queue_t queue);
int queue_enqueue(queue_t queue, void *data);
int queue_dequeue(queue_t queue, void **data);
int queue_delete(queue_t queue, void *data);
typedef void (*queue_func_t)(queue_t queue, void *data);
int queue_iterate(queue_t queue, queue_func_t func);
int queue_length(queue_t queue);

#endif /* _QUEUE_H */


#include "sem.h"
#include "uthread.h"
#include "queue.h"
#include <stdlib.h>

extern queue_t ready_queue;
extern uthread_tcb_t *current_thread;

sem_t *sem_create(int value) {
    sem_t *sem = malloc(sizeof(sem_t));
    if (!sem) return NULL;
    sem->value = value;
    sem->wait_queue = queue_create();
    return sem;
}

void sem_destroy(sem_t *sem) {
    queue_destroy(sem->wait_queue);
    free(sem);
}

int sem_wait(sem_t *sem) {
    sem->value--;
    if (sem->value < 0) {
        queue_enqueue(sem->wait_queue, current_thread);
        uthread_tcb_t *prev = current_thread;
        uthread_tcb_t *next = NULL;
        queue_dequeue(ready_queue, (void **)&next);
        current_thread = next;
        swapcontext(&prev->context, &next->context);
    }
    return 0;
}

int sem_signal(sem_t *sem) {
    sem->value++;
    if (sem->value <= 0) {
        uthread_tcb_t *thread = NULL;
        if (queue_dequeue(sem->wait_queue, (void **)&thread) == 0) {
            queue_enqueue(ready_queue, thread);
        }
    }
    return 0;
}

#include <stddef.h>
#include <stdlib.h>
#include "queue.h"
#include "private.h"
#include "sem.h"

struct semaphore {
    size_t count;
    queue_t waiters;
};

sem_t sem_create(size_t count)
{
    sem_t sem = malloc(sizeof(struct semaphore));
    if (!sem)
        return NULL;
    sem->count = count;
    sem->waiters = queue_create();
    if (!sem->waiters) {
        free(sem);
        return NULL;
    }
    return sem;
}

int sem_destroy(sem_t sem)
{
    if (!sem || queue_length(sem->waiters) != 0)
        return -1;
    queue_destroy(sem->waiters);
    free(sem);
    return 0;
}

int sem_down(sem_t sem)
{
    if (!sem)
        return -1;
    preempt_disable();
    while (sem->count == 0) {
        queue_enqueue(sem->waiters, uthread_current());
        uthread_block();
        preempt_enable();
        uthread_yield();
        preempt_disable();
    }
    sem->count--;
    preempt_enable();
    return 0;
}

int sem_up(sem_t sem)
{
    if (!sem)
        return -1;
    preempt_disable();
    if (queue_length(sem->waiters) > 0) {
        struct uthread_tcb *tcb;
        queue_dequeue(sem->waiters, (void**)&tcb);
        uthread_unblock(tcb);
    } else {
        sem->count++;
    }
    preempt_enable();
    return 0;
}


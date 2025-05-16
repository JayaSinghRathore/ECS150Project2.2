#include <stdlib.h>
#include "queue.h"
#include "sem.h"
#include "uthread.h"
#include "private.h"

struct sem {
    int count;
    queue_t blocked_queue;
};

sem_t sem_create(int count) {
    preempt_disable();
    sem_t sem = malloc(sizeof(struct sem));
    if (!sem) { preempt_enable(); return NULL; }
    sem->count = count;
    sem->blocked_queue = queue_create();
    if (!sem->blocked_queue) { free(sem); preempt_enable(); return NULL; }
    preempt_enable();
    return sem;
}

int sem_destroy(sem_t sem) {
    preempt_disable();
    if (!sem || queue_length(sem->blocked_queue) > 0) {
        preempt_enable();
        return -1;
    }
    queue_destroy(sem->blocked_queue);
    free(sem);
    preempt_enable();
    return 0;
}

int sem_down(sem_t sem) {
    if (!sem) return -1;
    preempt_disable();
    while (1) {
        if (sem->count > 0) {
            sem->count--;
            preempt_enable();
            return 0;
        }
        uthread_tcb_t *curr = uthread_current();
        curr->state = UTHREAD_BLOCKED;
        queue_enqueue(sem->blocked_queue, curr);
        preempt_enable();
        uthread_yield();
        preempt_disable();
    }
}

int sem_up(sem_t sem) {
    if (!sem) return -1;
    preempt_disable();
    sem->count++;
    if (queue_length(sem->blocked_queue) > 0) {
        uthread_tcb_t *thread_to_unblock;
        if (queue_dequeue(sem->blocked_queue, (void **)&thread_to_unblock) == 0) {
            thread_to_unblock->state = UTHREAD_READY;
            queue_enqueue(get_ready_queue(), thread_to_unblock);
        }
    }
    preempt_enable();
    return 0;
}

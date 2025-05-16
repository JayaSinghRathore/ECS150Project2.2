#ifndef SEM_H
#define SEM_H

#include "queue.h"

typedef struct sem {
    int value;
    queue_t wait_queue;
} sem_t;

sem_t *sem_create(int value);
void sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_signal(sem_t *sem);

#endif // SEM_H

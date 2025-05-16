#ifndef SEM_H
#define SEM_H

#include "uthread.h"

typedef struct sem {
    int count;
    // Add other fields as needed (e.g., waiting queue)
} sem_t;

sem_t sem_create(int value);
void sem_destroy(sem_t *sem);
void sem_down(sem_t *sem);
void sem_up(sem_t *sem);

#endif

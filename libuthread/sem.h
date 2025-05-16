#ifndef SEM_H
#define SEM_H

#include "uthread.h"

typedef struct sem *sem_t;

sem_t sem_create(int count);
int sem_destroy(sem_t sem);
int sem_down(sem_t sem);
int sem_up(sem_t sem);

#endif /* SEM_H */

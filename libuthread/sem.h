#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <sys/types.h>
typedef struct semaphore *sem_t;

sem_t sem_create(size_t count);
int sem_destroy(sem_t sem);
int sem_down(sem_t sem);
int sem_up(sem_t sem);

#endif /* _SEMAPHORE_H */


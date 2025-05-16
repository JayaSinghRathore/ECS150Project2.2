#include "sem.h"
#include <stdlib.h>
#include <stdio.h>
#include "uthread.h"

sem_t sem_create(int value) {
    sem_t sem;
    sem.count = value;
    // Initialize other fields as needed (e.g., waiting queue)
    return sem;
}

void sem_destroy(sem_t *sem) {
    // If you dynamically allocated anything inside sem, free it here
    // Otherwise, nothing needed for stack-allocated semaphores
    sem->count = 0;
}

void sem_down(sem_t *sem) {
    while (sem->count <= 0) {
        uthread_yield(); // Yield to other threads
    }
    sem->count--;
}

void sem_up(sem_t *sem) {
    sem->count++;
    // If you have a waiting queue, wake up a thread here
}


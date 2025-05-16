k#include "sem.h"
#include <stdio.h>

// Simple stub implementations for demonstration.
// Replace with your real logic as needed.

sem_t sem_create(int value) {
    sem_t sem;
    sem.count = value;
    return sem;
}

void sem_destroy(sem_t *sem) {
    // Clean up any dynamically allocated resources here if needed.
    sem->count = 0;
}

void sem_down(sem_t *sem) {
    while (sem->count <= 0) {
        uthread_yield();
    }
    sem->count--;
}

void sem_up(sem_t *sem) {
    sem->count++;
}


#include <stdio.h>
#include "uthread.h"
#include "sem.h"

#define NUM_THREADS 4

typedef struct {
    int id;
    sem_t *sem;
} thread_arg_t;

void thread_func(void *arg) {
    thread_arg_t *targ = (thread_arg_t *)arg;
    sem_down(targ->sem);
    printf("Thread %d in critical section\n", targ->id);
    sem_up(targ->sem);
}

int main(void) {
    sem_t sem = sem_create(0);

    thread_arg_t args[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].id = i;
        args[i].sem = &sem;
        uthread_create(thread_func, &args[i]);
    }

    // Kickstart the semaphore so one thread can proceed
    sem_up(&sem);

    // Start the scheduler
    uthread_run(0, NULL, NULL);

    sem_destroy(&sem);
    return 0;
}

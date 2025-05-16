#include <stdbool.h>
#include <stdio.h>
#include "uthread.h"
#include "sem.h"

struct thread_data {
    sem_t sem1;
    sem_t sem2;
    int count;
};

void thread2(void *arg) {
    struct thread_data *t = (struct thread_data *) arg;
    for (int i = 0; i < t->count; i++) {
        sem_up(&t->sem1);
        sem_down(&t->sem2);
    }
}

void thread1(void *arg) {
    struct thread_data *t = (struct thread_data *) arg;
    uthread_create(thread2, t);
    for (int i = 0; i < t->count; i++) {
        sem_down(&t->sem1);
        printf("Ping\n");
        sem_up(&t->sem2);
    }
}

void start(void *arg) {
    (void)arg;
    static struct thread_data t;
    t.count = 10;
    t.sem1 = sem_create(0);
    t.sem2 = sem_create(0);
    uthread_create(thread1, &t);
}

int main(void) {
    uthread_run(0, start, NULL);
    return 0;
}

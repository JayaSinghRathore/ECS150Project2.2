#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "uthread.h"
#include "sem.h"

#define BUFFER_SIZE 4

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
    sem_t mutex;
    sem_t empty;
    sem_t full;
} buffer_t;

void producer(void *arg) {
    buffer_t *t = (buffer_t *)arg;
    for (int i = 1; i <= 8; i++) {
        sem_down(&t->empty);
        sem_down(&t->mutex);
        t->buffer[t->in] = i;
        printf("Produced: %d\n", i);
        t->in = (t->in + 1) % BUFFER_SIZE;
        sem_up(&t->mutex);
        sem_up(&t->full);
    }
}

void consumer(void *arg) {
    buffer_t *t = (buffer_t *)arg;
    for (int i = 1; i <= 8; i++) {
        sem_down(&t->full);
        sem_down(&t->mutex);
        int item = t->buffer[t->out];
        printf("Consumed: %d\n", item);
        t->out = (t->out + 1) % BUFFER_SIZE;
        sem_up(&t->mutex);
        sem_up(&t->empty);
    }
}

void start(void *arg) {
    (void)arg;
    static buffer_t t;
    t.in = 0;
    t.out = 0;
    t.mutex = sem_create(1);
    t.empty = sem_create(BUFFER_SIZE);
    t.full = sem_create(0);

    uthread_create(producer, &t);
    uthread_create(consumer, &t);
}

int main(void) {
    uthread_run(0, start, NULL);
    return 0;
}

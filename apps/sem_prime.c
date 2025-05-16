#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "uthread.h"
#include "sem.h"

#define N 10

typedef struct pipe {
    int value;
    sem_t produce;
    sem_t consume;
} pipe_t;

void source(void *arg) {
    pipe_t *p = (pipe_t *)arg;
    for (int i = 2; i <= N; i++) {
        p->value = i;
        sem_up(&p->consume);
        sem_down(&p->produce);
    }
    p->value = 0; // End of stream
    sem_up(&p->consume);
}

void filter(void *arg) {
    pipe_t *left = (pipe_t *)arg;
    int prime;

    sem_down(&left->consume);
    prime = left->value;
    if (prime == 0) {
        sem_up(&left->produce);
        return;
    }
    printf("%d\n", prime);

    pipe_t *right = malloc(sizeof(pipe_t));
    right->produce = sem_create(0);
    right->consume = sem_create(0);

    uthread_create(filter, right);

    while (1) {
        sem_down(&left->consume);
        int val = left->value;
        sem_up(&left->produce);
        if (val == 0) {
            right->value = 0;
            sem_up(&right->consume);
            break;
        }
        if (val % prime != 0) {
            right->value = val;
            sem_up(&right->consume);
            sem_down(&right->produce);
        }
    }

    sem_destroy(&right->produce);
    sem_destroy(&right->consume);
    free(right);
}

void start(void *arg) {
    (void)arg;
    pipe_t *first = malloc(sizeof(pipe_t));
    first->produce = sem_create(0);
    first->consume = sem_create(0);

    uthread_create(filter, first);
    uthread_create(source, first);

    // Wait for the first filter to finish
    sem_down(&first->produce);

    sem_destroy(&first->produce);
    sem_destroy(&first->consume);
    free(first);
}

int main(void) {
    uthread_run(0, start, NULL);
    return 0;
}

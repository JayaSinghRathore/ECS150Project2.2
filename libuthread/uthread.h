#ifndef UTHREAD_H
#define UTHREAD_H

#include <ucontext.h>

#define UTHREAD_MAX_THREADS 128
#define UTHREAD_STACK_SIZE  32768

typedef struct uthread_tcb {
    ucontext_t context;
    void (*func)(void *);
    void *arg;
    void *stack;
    int finished;
} uthread_tcb_t;

void uthread_init(void);
int uthread_create(void (*func)(void *), void *arg);
void uthread_yield(void);
void uthread_exit(void);
void uthread_run(void);

#endif // UTHREAD_H

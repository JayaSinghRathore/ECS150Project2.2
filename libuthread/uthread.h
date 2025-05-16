#ifndef UTHREAD_H
#define UTHREAD_H

typedef struct uthread uthread_t;

void uthread_create(void (*func)(void *), void *arg);
void uthread_yield(void);
void uthread_run(int, void (*func)(void *), void *arg);

#endif


#ifndef _UTHREAD_H
#define _UTHREAD_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef int uthread_t;

uthread_t uthread_self(void);
uthread_t uthread_create(void (*func)(void *), void *arg);
void uthread_yield(void);
void uthread_exit(void *retval);
int uthread_join(uthread_t tid, void **retval);
int uthread_run(bool preempt, void (*start_func)(void *), void *arg);

#endif /* _UTHREAD_H */

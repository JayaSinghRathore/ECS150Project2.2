#ifndef _UTHREAD_H
#define _UTHREAD_H

#include <stdbool.h>
typedef void (*uthread_func_t)(void *arg);
int uthread_run(bool preempt, uthread_func_t func, void *arg);
int uthread_create(uthread_func_t func, void *arg);
void uthread_yield(void);
void uthread_exit(void);

#endif /* _THREAD_H */


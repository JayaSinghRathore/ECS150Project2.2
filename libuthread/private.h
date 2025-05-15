#ifndef _UTHREAD_PRIVATE_H
#define _UTHREAD_PRIVATE_H

#include <ucontext.h>
#include "uthread.h"

typedef ucontext_t uthread_ctx_t;
void uthread_ctx_switch(uthread_ctx_t *prev, uthread_ctx_t *next);
void *uthread_ctx_alloc_stack(void);
void uthread_ctx_destroy_stack(void *top_of_stack);
int uthread_ctx_init(uthread_ctx_t *uctx, void *top_of_stack,
                     uthread_func_t func, void *arg);

void preempt_start(bool preempt);
void preempt_stop(void);
void preempt_enable(void);
void preempt_disable(void);

struct uthread_tcb;
struct uthread_tcb *uthread_current(void);
void uthread_block(void);
void uthread_unblock(struct uthread_tcb *uthread);

#endif /* _UTHREAD_PRIVATE_H */


#ifndef _UTHREAD_PRIVATE_H
#define _UTHREAD_PRIVATE_H

#include <stdbool.h>
#include <ucontext.h>
#include "uthread.h"
#include "queue.h"

typedef ucontext_t uthread_ctx_t;

typedef enum {
    UTHREAD_READY,
    UTHREAD_RUNNING,
    UTHREAD_BLOCKED,
    UTHREAD_FINISHED
} uthread_state_t;

typedef struct uthread_tcb {
    uthread_t tid;
    uthread_ctx_t context;
    void *stack;
    void (*func)(void *);
    void *arg;
    void *retval;
    uthread_state_t state;
} uthread_tcb_t;

// Exported for sem.c and others
uthread_tcb_t *uthread_current(void);
queue_t get_ready_queue(void);

int uthread_ctx_switch(uthread_ctx_t *prev, uthread_ctx_t *next);
void *uthread_ctx_alloc_stack(void);
void uthread_ctx_destroy_stack(void *stack);
int uthread_ctx_init(uthread_ctx_t *uctx, void *stack, void (*func)(void *), void *arg);

void preempt_start(bool preempt);
void preempt_stop(void);
void preempt_enable(void);
void preempt_disable(void);

#endif /* _UTHREAD_PRIVATE_H */

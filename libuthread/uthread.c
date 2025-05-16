#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "private.h"
#include "uthread.h"
#include "queue.h"

// No need to redefine uthread_state_t or struct uthread_tcb here!

static queue_t ready_queue;
static uthread_tcb_t *current_thread = NULL;
static uthread_t next_tid = 1;

// Export for sem.c
uthread_tcb_t *uthread_current(void) { return current_thread; }
queue_t get_ready_queue(void) { return ready_queue; }

static void thread_wrapper(void *arg) {
    uthread_tcb_t *tcb = (uthread_tcb_t *)arg;
    tcb->func(tcb->arg);
    uthread_exit(NULL);
}

static void schedule(void) {
    uthread_tcb_t *prev = current_thread;
    uthread_tcb_t *next = NULL;

    while (queue_dequeue(ready_queue, (void **)&next) == 0) {
        if (next->state == UTHREAD_READY) break;
    }
    if (!next) exit(0);

    next->state = UTHREAD_RUNNING;
    current_thread = next;

    if (prev && prev->state == UTHREAD_FINISHED) {
        uthread_ctx_switch(&prev->context, &next->context);
        uthread_ctx_destroy_stack(prev->stack);
        free(prev);
    } else if (prev && prev != next) {
        queue_enqueue(ready_queue, prev);
        prev->state = UTHREAD_READY;
        uthread_ctx_switch(&prev->context, &next->context);
    } else if (!prev) {
        setcontext(&next->context);
    }
}

uthread_t uthread_create(void (*func)(void *), void *arg) {
    preempt_disable();
    uthread_tcb_t *tcb = malloc(sizeof(uthread_tcb_t));
    if (!tcb) { preempt_enable(); return -1; }
    tcb->tid = next_tid++;
    tcb->stack = uthread_ctx_alloc_stack();
    tcb->func = func;
    tcb->arg = arg;
    tcb->retval = NULL;
    tcb->state = UTHREAD_READY;
    if (uthread_ctx_init(&tcb->context, tcb->stack, thread_wrapper, tcb) == -1) {
        uthread_ctx_destroy_stack(tcb->stack);
        free(tcb);
        preempt_enable();
        return -1;
    }
    queue_enqueue(ready_queue, tcb);
    preempt_enable();
    return tcb->tid;
}

uthread_t uthread_self(void) {
    return current_thread ? current_thread->tid : -1;
}

void uthread_yield(void) {
    preempt_disable();
    if (current_thread && current_thread->state == UTHREAD_RUNNING)
        current_thread->state = UTHREAD_READY;
    schedule();
    preempt_enable();
}

void uthread_exit(void *retval) {
    preempt_disable();
    current_thread->retval = retval;
    current_thread->state = UTHREAD_FINISHED;
    schedule();
    fprintf(stderr, "uthread_exit: fatal error, returned from schedule\n");
    exit(1);
}

int uthread_join(uthread_t tid, void **retval) {
    (void)tid; (void)retval;
    return -1;
}

int uthread_run(bool preempt, void (*start_func)(void *), void *arg) {
    ready_queue = queue_create();
    if (!ready_queue) return -1;

    uthread_tcb_t main_tcb;
    main_tcb.tid = 0;
    main_tcb.stack = NULL;
    main_tcb.func = NULL;
    main_tcb.arg = NULL;
    main_tcb.retval = NULL;
    main_tcb.state = UTHREAD_RUNNING;
    current_thread = &main_tcb;

    if (uthread_create(start_func, arg) == -1) return -1;

    if (preempt) preempt_start(true);

    schedule();

    if (preempt) preempt_stop();

    return 0;
}

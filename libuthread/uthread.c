#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "private.h"
#include "uthread.h"
#include "queue.h"

enum thread_state {
    RUNNING,
    READY,
    BLOCKED,
    EXITED
};

struct uthread_tcb {
    uthread_ctx_t ctx;
    void *stack;
    enum thread_state state;
};

static queue_t ready_queue;
static queue_t blocked_queue;
static struct uthread_tcb *current_thread = NULL;
static struct uthread_tcb *idle_thread = NULL;

struct uthread_tcb *uthread_current(void)
{
    return current_thread;
}

static void schedule(void)
{
    struct uthread_tcb *prev = current_thread;
    struct uthread_tcb *next = NULL;
    if (queue_dequeue(ready_queue, (void**)&next) == 0) {
        current_thread = next;
        current_thread->state = RUNNING;
    } else {
        current_thread = idle_thread;
        current_thread->state = RUNNING;
    }
    if (prev != current_thread)
        uthread_ctx_switch(&prev->ctx, &current_thread->ctx);
}

void uthread_yield(void)
{
    preempt_disable();
    if (current_thread && current_thread->state == RUNNING && current_thread != idle_thread) {
        current_thread->state = READY;
        queue_enqueue(ready_queue, current_thread);
    }
    schedule();
    preempt_enable();
}

void uthread_exit(void)
{
    preempt_disable();
    current_thread->state = EXITED;
    free(current_thread->stack);
    free(current_thread);
    schedule();
    preempt_enable();
    // Should never return
    assert(0);
}

int uthread_create(uthread_func_t func, void *arg)
{
    struct uthread_tcb *tcb = malloc(sizeof(struct uthread_tcb));
    if (!tcb)
        return -1;
    tcb->stack = uthread_ctx_alloc_stack();
    if (!tcb->stack) {
        free(tcb);
        return -1;
    }
    if (uthread_ctx_init(&tcb->ctx, tcb->stack, func, arg) < 0) {
        uthread_ctx_destroy_stack(tcb->stack);
        free(tcb);
        return -1;
    }
    tcb->state = READY;
    preempt_disable();
    queue_enqueue(ready_queue, tcb);
    preempt_enable();
    return 0;
}

static void idle(void *arg)
{
    (void)arg;
    while (queue_length(ready_queue) > 0 || queue_length(blocked_queue) > 0)
        uthread_yield();
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
    ready_queue = queue_create();
    blocked_queue = queue_create();
    if (!ready_queue || !blocked_queue)
        return -1;

    preempt_start(preempt);

    idle_thread = malloc(sizeof(struct uthread_tcb));
    if (!idle_thread)
        return -1;
    idle_thread->stack = uthread_ctx_alloc_stack();
    if (!idle_thread->stack) {
        free(idle_thread);
        return -1;
    }
    uthread_ctx_init(&idle_thread->ctx, idle_thread->stack, idle, NULL);
    idle_thread->state = RUNNING;

    current_thread = idle_thread;

    if (uthread_create(func, arg) < 0)
        return -1;

    schedule();

    preempt_stop();

    uthread_ctx_destroy_stack(idle_thread->stack);
    free(idle_thread);
    queue_destroy(ready_queue);
    queue_destroy(blocked_queue);
    return 0;
}

void uthread_block(void)
{
    preempt_disable();
    current_thread->state = BLOCKED;
    queue_enqueue(blocked_queue, current_thread);
    schedule();
    preempt_enable();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
    preempt_disable();
    if (uthread->state == BLOCKED) {
        uthread->state = READY;
        queue_delete(blocked_queue, uthread);
        queue_enqueue(ready_queue, uthread);
    }
    preempt_enable();
}


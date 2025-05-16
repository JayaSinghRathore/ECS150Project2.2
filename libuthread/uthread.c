#include "uthread.h"
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

queue_t ready_queue = NULL;
uthread_tcb_t *current_thread = NULL;
static ucontext_t main_context;
static int initialized = 0;

static void thread_stub(void *arg) {
    (void)arg;  // Suppress unused parameter warning
    uthread_tcb_t *tcb = current_thread;
    tcb->func(tcb->arg);
    tcb->finished = 1;
    uthread_exit();
}

void uthread_init(void) {
    if (initialized) return;
    ready_queue = queue_create();
    initialized = 1;
}

int uthread_create(void (*func)(void *), void *arg) {
    uthread_init();
    uthread_tcb_t *tcb = malloc(sizeof(uthread_tcb_t));
    if (!tcb) return -1;

    tcb->func = func;
    tcb->arg = arg;
    tcb->finished = 0;
    tcb->stack = malloc(UTHREAD_STACK_SIZE);
    if (!tcb->stack) {
        free(tcb);
        return -1;
    }

    getcontext(&tcb->context);
    tcb->context.uc_stack.ss_sp = tcb->stack;
    tcb->context.uc_stack.ss_size = UTHREAD_STACK_SIZE;
    tcb->context.uc_link = &main_context;
    makecontext(&tcb->context, (void (*)())thread_stub, 1, arg);

    queue_enqueue(ready_queue, tcb);
    return 0;
}

void uthread_yield(void) {
    uthread_tcb_t *prev = current_thread;
    uthread_tcb_t *next = NULL;

    if (prev && !prev->finished)
        queue_enqueue(ready_queue, prev);

    if (queue_dequeue(ready_queue, (void **)&next) == 0) {
        current_thread = next;
        swapcontext(&prev->context, &next->context);
    }
}

void uthread_exit(void) {
    uthread_tcb_t *prev = current_thread;
    free(prev->stack);
    free(prev);
    current_thread = NULL;
    uthread_tcb_t *next = NULL;
    if (queue_dequeue(ready_queue, (void **)&next) == 0) {
        current_thread = next;
        setcontext(&next->context);
    } else {
        setcontext(&main_context);
    }
}

void uthread_run(void) {
    uthread_init();
    uthread_tcb_t *next = NULL;
    if (queue_dequeue(ready_queue, (void **)&next) == 0) {
        current_thread = next;
        swapcontext(&main_context, &next->context);
    }
}

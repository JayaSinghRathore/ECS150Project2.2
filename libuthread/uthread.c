#include "uthread.h"
#include "queue.h"
#include <stdbool.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE 32768

typedef struct uthread {
    ucontext_t context;
    void *stack;
    uthread_func_t func;
    void *arg;
} uthread_t;

static queue_t ready_queue = NULL;
static uthread_t *current_thread = NULL;
static ucontext_t main_context;

// Forward declaration
static void uthread_start(void);

int uthread_run(bool preempt, uthread_func_t func, void *arg) {
    (void)preempt; // Preemption not implemented in this minimal version

    ready_queue = queue_create();
    if (!ready_queue)
        return -1;

    // Create the first thread
    if (uthread_create(func, arg) < 0)
        return -1;

    // Save the main context
    getcontext(&main_context);

    // Run threads until queue is empty
    while (queue_length(ready_queue) > 0) {
        queue_dequeue(ready_queue, (void**)&current_thread);
        setcontext(&current_thread->context);
    }

    queue_destroy(ready_queue);
    ready_queue = NULL;
    return 0;
}

int uthread_create(uthread_func_t func, void *arg) {
    uthread_t *thread = malloc(sizeof(uthread_t));
    if (!thread)
        return -1;

    thread->stack = malloc(STACK_SIZE);
    if (!thread->stack) {
        free(thread);
        return -1;
    }

    thread->func = func;
    thread->arg = arg;

    getcontext(&thread->context);
    thread->context.uc_stack.ss_sp = thread->stack;
    thread->context.uc_stack.ss_size = STACK_SIZE;
    thread->context.uc_link = &main_context;
    makecontext(&thread->context, uthread_start, 0);

    queue_enqueue(ready_queue, thread);
    return 0;
}

void uthread_yield(void) {
    if (!current_thread)
        return;

    queue_enqueue(ready_queue, current_thread);
    uthread_t *prev_thread = current_thread;
    queue_dequeue(ready_queue, (void**)&current_thread);
    swapcontext(&prev_thread->context, &current_thread->context);
}

void uthread_exit(void) {
    if (!current_thread)
        return;

    free(current_thread->stack);
    free(current_thread);
    current_thread = NULL;

    // Switch back to main context, which will schedule the next thread
    setcontext(&main_context);
}

// Internal start function for threads
static void uthread_start(void) {
    if (current_thread && current_thread->func) {
        current_thread->func(current_thread->arg);
    }
    uthread_exit();
}


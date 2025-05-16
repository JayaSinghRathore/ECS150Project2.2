#include "uthread.h"
#include "private.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>

uthread_t *current = NULL;
static uthread_t *main_thread = NULL;
static uthread_t *ready_queue = NULL;

void enqueue(uthread_t *t) {
    t->next = NULL;
    if (!ready_queue) {
        ready_queue = t;
    } else {
        uthread_t *tmp = ready_queue;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = t;
    }
}

static uthread_t* dequeue(void) {
    if (!ready_queue)
        return NULL;
    uthread_t *t = ready_queue;
    ready_queue = ready_queue->next;
    t->next = NULL;
    return t;
}

void uthread_run(int preempt, void (*func)(void *), void *arg) {
    printf("[uthread_run] called\n");
    (void)preempt; // ignore preemption for now

    main_thread = malloc(sizeof(uthread_t));
    main_thread->is_main_thread = 1;
    main_thread->stack = NULL;
    main_thread->state = RUNNING;
    main_thread->next = NULL;
    getcontext(&main_thread->context);

    current = main_thread;

    uthread_create(func, arg);

    while (ready_queue != NULL) {
        uthread_yield();
    }

    free(main_thread);
    printf("[uthread_run] done\n");
}

int uthread_create(void (*func)(void *), void *arg) {
    printf("[uthread_create] called\n");
    uthread_t *t = malloc(sizeof(uthread_t));
    t->is_main_thread = 0;
    t->stack = malloc(UTHREAD_STACK_SIZE);
    t->state = READY;
    t->next = NULL;
    uthread_ctx_init(&t->context, t->stack, func, arg);
    enqueue(t);
    printf("[uthread_create] done\n");
    return 0;
}

void uthread_yield(void) {
    printf("[uthread_yield] called\n");
    uthread_t *prev = current;
    uthread_t *next = dequeue();
    if (next) {
        if (prev->state == RUNNING)
            prev->state = READY;
        // Only enqueue if the thread is READY (not BLOCKED)
        if (prev->state == READY)
            enqueue(prev);
        next->state = RUNNING;
        current = next;
        printf("[uthread_yield] switching context\n");
        swapcontext(&prev->context, &next->context);
    } else {
        printf("[uthread_yield] no thread to yield to\n");
    }
    printf("[uthread_yield] done\n");
}

void uthread_exit(void) {
    printf("[uthread_exit] called\n");
    uthread_t *prev = current;
    current = dequeue();
    if (!prev->is_main_thread && prev->stack) {
        free(prev->stack);
    }
    free(prev);
    if (current) {
        current->state = RUNNING;
        printf("[uthread_exit] switching context\n");
        setcontext(&current->context);
    } else {
        printf("[uthread_exit] no more threads, exiting\n");
        exit(0);
    }
    printf("[uthread_exit] done\n");
}

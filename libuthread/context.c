#include <stdio.h>
#include <stdlib.h>
#include "private.h"
#include "uthread.h"

#define UTHREAD_STACK_SIZE 32768

void uthread_ctx_switch(uthread_ctx_t *prev, uthread_ctx_t *next)
{
    if (swapcontext(prev, next)) {
        perror("swapcontext");
        exit(1);
    }
}

void *uthread_ctx_alloc_stack(void)
{
    return malloc(UTHREAD_STACK_SIZE);
}

void uthread_ctx_destroy_stack(void *top_of_stack)
{
    free(top_of_stack);
}

static void uthread_ctx_bootstrap(uthread_func_t func, void *arg)
{
    preempt_enable();
    func(arg);
    uthread_exit();
}

int uthread_ctx_init(uthread_ctx_t *uctx, void *top_of_stack,
                     uthread_func_t func, void *arg)
{
    if (getcontext(uctx))
        return -1;
    uctx->uc_stack.ss_sp = top_of_stack;
    uctx->uc_stack.ss_size = UTHREAD_STACK_SIZE;
    makecontext(uctx, (void (*)(void)) uthread_ctx_bootstrap, 2, func, arg);
    return 0;
}


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "private.h"
#include "uthread.h"

#define UTHREAD_STACK_SIZE 65536

int uthread_ctx_init(uthread_ctx_t *ctx, void *stack, void (*func)(void *), void *arg) {
    if (getcontext(ctx) == -1)
        return -1;
    ctx->uc_stack.ss_sp = stack;
    ctx->uc_stack.ss_size = UTHREAD_STACK_SIZE;
    ctx->uc_stack.ss_flags = 0;
    ctx->uc_link = NULL;
    makecontext(ctx, (void (*)(void))func, 1, arg);
    return 0;
}

int uthread_ctx_switch(uthread_ctx_t *prev, uthread_ctx_t *next) {
    if (swapcontext(prev, next) == -1)
        return -1;
    return 0;
}

void *uthread_ctx_alloc_stack(void) {
    void *stack = malloc(UTHREAD_STACK_SIZE);
    return stack;
}

void uthread_ctx_destroy_stack(void *stack) {
    free(stack);
}

#include "uthread.h"
#include <ucontext.h>
#include <stddef.h>
#include <stdlib.h>

// Trampoline function to ensure uthread_exit() is called after the user function
static void uthread_start(void (*func)(void *), void *arg) {
    func(arg);
    uthread_exit();
}

int uthread_ctx_init(ucontext_t *ctx, void *stack, void (*func)(void *), void *arg) {
    if (getcontext(ctx) == -1)
        return -1;

    ctx->uc_stack.ss_sp = stack;
    ctx->uc_stack.ss_size = UTHREAD_STACK_SIZE;
    ctx->uc_link = NULL;

    // Use the trampoline so uthread_exit is called when func returns
    makecontext(ctx, (void (*)(void))uthread_start, 2, func, arg);

    return 0;
}

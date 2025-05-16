#include <ucontext.h>
#include <stddef.h>      // <-- Add this line for NULL
#include "private.h"

// Initializes a context for a thread
int uthread_ctx_init(ucontext_t *ctx, void *stack, void (*func)(void *), void *arg) {
    getcontext(ctx);
    ctx->uc_stack.ss_sp = stack;
    ctx->uc_stack.ss_size = UTHREAD_STACK_SIZE;
    ctx->uc_link = NULL;
    makecontext(ctx, (void (*)())func, 1, arg);
    return 0;
}

// Switches from prev context to next context
int uthread_ctx_switch(ucontext_t *prev, ucontext_t *next) {
    return swapcontext(prev, next);
}

// preempt.c

#include <signal.h>
#include "preempt.h"

void preempt_disable(void) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &set, NULL);
}

void preempt_enable(void) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &set, NULL);
}

// If you have a preempt_start or similar, keep it as is, or update as needed.
// Example:
void preempt_start(void) {
    // Your timer setup code here, e.g., using setitimer and signal
}


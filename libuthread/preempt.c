#include <signal.h>
#include <stdbool.h>
#include <sys/time.h>
#include "private.h"
#include "uthread.h"

#define HZ 100

static struct sigaction old_action;
static struct itimerval old_timer;
static bool preemption_enabled = false;

static void preempt_handler(int sig) {
    (void)sig;
    if (preemption_enabled) {
        uthread_yield();
    }
}

void preempt_disable(void) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void preempt_enable(void) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void preempt_start(bool preempt) {
    if (preempt) {
        struct sigaction new_action;
        struct itimerval timer;
        sigemptyset(&new_action.sa_mask);
        new_action.sa_handler = preempt_handler;
        new_action.sa_flags = 0;
        sigaction(SIGVTALRM, &new_action, &old_action);
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 1000000 / HZ;
        timer.it_interval = timer.it_value;
        setitimer(ITIMER_VIRTUAL, &timer, &old_timer);
        preemption_enabled = true;
    }
}

void preempt_stop(void) {
    if (preemption_enabled) {
        sigaction(SIGVTALRM, &old_action, NULL);
        setitimer(ITIMER_VIRTUAL, &old_timer, NULL);
        preemption_enabled = false;
    }
}

#include <signal.h>
#include <stddef.h> // For NULL
#include <string.h>
#include <sys/time.h>
#include "private.h"
#include "uthread.h"

static struct sigaction old_action;
static sigset_t mask;

void preempt_disable(void) {
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void preempt_enable(void) {
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

static void alarm_handler(int signum) {
    (void)signum;
    uthread_yield();
}

void preempt_start(void) {
    struct sigaction sa;
    struct itimerval timer;

    // Set up signal mask for SIGVTALRM
    sigemptyset(&mask);
    sigaddset(&mask, SIGVTALRM);

    // Set up the signal handler
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGVTALRM, &sa, &old_action);

    // Set up the timer: 10 ms intervals
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 10000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 10000;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

void preempt_stop(void) {
    struct itimerval timer;

    // Disable the timer
    memset(&timer, 0, sizeof(timer));
    setitimer(ITIMER_VIRTUAL, &timer, NULL);

    // Restore the old signal handler
    sigaction(SIGVTALRM, &old_action, NULL);
}

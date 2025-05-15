#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "private.h"
#include "uthread.h"

#define HZ 100

static struct sigaction oldact;
static struct itimerval oldtimer;
static bool preemption_enabled = false;

static void sigvtalrm_handler(int signum)
{
    (void)signum;
    uthread_yield();
}

void preempt_disable(void)
{
    if (preemption_enabled)
        sigprocmask(SIG_BLOCK, &(sigset_t){SIGVTALRM}, NULL);
}

void preempt_enable(void)
{
    if (preemption_enabled)
        sigprocmask(SIG_UNBLOCK, &(sigset_t){SIGVTALRM}, NULL);
}

void preempt_start(bool preempt)
{
    if (!preempt)
        return;
    preemption_enabled = true;
    struct sigaction sa;
    sa.sa_handler = sigvtalrm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGVTALRM, &sa, &oldact);

    struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 1000000 / HZ;
    timer.it_value = timer.it_interval;
    setitimer(ITIMER_VIRTUAL, &timer, &oldtimer);
}

void preempt_stop(void)
{
    if (!preemption_enabled)
        return;
    sigaction(SIGVTALRM, &oldact, NULL);
    setitimer(ITIMER_VIRTUAL, &oldtimer, NULL);
    preemption_enabled = false;
}


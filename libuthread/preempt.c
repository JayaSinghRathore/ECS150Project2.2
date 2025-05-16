#include "private.h"
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define HZ 100
#define INTERVAL_USEC (1000000 / HZ)

static struct sigaction old_action;
static struct itimerval old_timer;
static bool preempt_on = false;

/* The signal handler for SIGVTALRM */
static void preempt_handler(int signum)
{
    (void)signum;
    uthread_yield();
}

void preempt_disable(void)
{
    if (preempt_on) {
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGVTALRM);
        sigprocmask(SIG_BLOCK, &set, NULL);
    }
}

void preempt_enable(void)
{
    if (preempt_on) {
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGVTALRM);
        sigprocmask(SIG_UNBLOCK, &set, NULL);
    }
}

void preempt_start(bool preempt)
{
    if (!preempt)
        return;

    preempt_on = true;

    // Set up the signal handler for SIGVTALRM
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = preempt_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGVTALRM, &sa, &old_action);

    // Set up the timer
    struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = INTERVAL_USEC;
    timer.it_value = timer.it_interval;
    setitimer(ITIMER_VIRTUAL, &timer, &old_timer);
}

void preempt_stop(void)
{
    if (!preempt_on)
        return;

    // Restore old signal handler
    sigaction(SIGVTALRM, &old_action, NULL);

    // Restore old timer
    setitimer(ITIMER_VIRTUAL, &old_timer, NULL);

    preempt_on = false;
}

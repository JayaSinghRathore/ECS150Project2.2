#include <signal.h>
#include <stddef.h>
#include "preempt.h"

// Example implementation (you may need to adjust for your project)

void preempt_disable(void)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &set, NULL);
}

void preempt_enable(void)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &set, NULL);
}

void preempt_start(void)
{
    // This is a placeholder; your real implementation may need to set up a timer and signal handler.
    // Example:
    // struct sigaction sa;
    // sa.sa_handler = your_handler;
    // sigemptyset(&sa.sa_mask);
    // sa.sa_flags = 0;
    // sigaction(SIGVTALRM, &sa, NULL);
    //
    // struct itimerval timer;
    // timer.it_value.tv_sec = 0;
    // timer.it_value.tv_usec = 10000; // 10 ms
    // timer.it_interval = timer.it_value;
    // setitimer(ITIMER_VIRTUAL, &timer, NULL);
}


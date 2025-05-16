#include <stdio.h>
#include "uthread.h"

void thread1(void *arg) {
    (void)arg;
    printf("thread1\n");
    uthread_yield();
}

void thread2(void *arg) {
    (void)arg;
    printf("thread2\n");
    uthread_yield();
}

void thread3(void *arg) {
    (void)arg;
    printf("thread3\n");
    // Do not call uthread_yield() here
}

void start(void *arg) {
    (void)arg;
    uthread_create(thread1, NULL);
    uthread_create(thread2, NULL);
    uthread_create(thread3, NULL);
}

int main(void) {
    uthread_run(0, start, NULL);
    return 0;
}

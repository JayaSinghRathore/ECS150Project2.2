#include <stdio.h>
#include "uthread.h"

void hello(void *arg) {
    (void)arg;
    printf("Hello world!\n");
}

void start(void *arg) {
    (void)arg;
    uthread_create(hello, NULL);
}

int main(void) {
    uthread_run(0, start, NULL);
    return 0;
}

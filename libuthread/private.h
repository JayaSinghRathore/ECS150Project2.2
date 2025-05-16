#ifndef PRIVATE_H
#define PRIVATE_H

#include "uthread.h"

extern uthread_t *current;
void enqueue(uthread_t *t);

#endif

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sem.h>
#include <uthread.h>

sem_t sem1;
sem_t sem2;
sem_t sem3;

static void thread3(void *arg)
{
	(void)arg;
	sem_down(sem3);
	printf("thread3\n");
	sem_up(sem2);
}

static void thread2(void *arg)
{
	(void)arg;
	sem_down(sem2);
	printf("thread2\n");
	sem_up(sem1);
}

static void thread1(void *arg)
{
	(void)arg;
	uthread_create(thread2, NULL);
	uthread_create(thread3, NULL);
	sem_up(sem3);
	sem_down(sem1);
	printf("thread1\n");
}

int main(void)
{
	sem1 = sem_create(0);
	sem2 = sem_create(0);
	sem3 = sem_create(0);
	uthread_run(false, thread1, NULL);
	sem_destroy(sem1);
	sem_destroy(sem2);
	sem_destroy(sem3);
	return 0;
}


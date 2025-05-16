CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -I../libuthread
LDFLAGS = -L../libuthread
LDLIBS = -luthread
PROGS = queue_tester_example sem_buffer sem_count sem_prime sem_simple uthread_hello uthread_yield

all: ../libuthread/libuthread.a $(PROGS)

../libuthread/libuthread.a:
	$(MAKE) -C ../libuthread

%: %.c ../libuthread/libuthread.a
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) *.o

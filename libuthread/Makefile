CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
AR = ar
ARFLAGS = rcs

TARGET = libuthread.a
SRCS = queue.c context.c uthread.c sem.c preempt.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

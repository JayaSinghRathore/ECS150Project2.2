CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -MMD
AR = ar
ARFLAGS = rcs

SRC = queue.c sem.c uthread.c context.c preempt.c
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

LIB = libuthread.a

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)

clean:
	rm -f $(OBJ) $(DEP) $(LIB)

.PHONY: all clean


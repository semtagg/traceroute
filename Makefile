CC = gcc
CFLAGS = -std=c99 -Wall -Wextra
LDFLAGS =
RM = rm -f
TARGET = traceroute

SRCS = icmp.c traceroute.c
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET}

$(TARGET): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

.PHONY: clean
clean:
	-${RM} -- ${TARGET} ${OBJS}

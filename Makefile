CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -pthread -Iinclude

SRCS = src/uart.c src/queue.c src/processor_interface.c #test/test.c
OBJS = $(SRCS:.c=.o)
OUTDIR = output
OBJS = $(SRCS:%=$(OUTDIR)/%.o)
TARGET = $(OUTDIR)/uart_test

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OUTDIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test: all
	./$(TARGET)

clean:
	rm -rf $(OUTDIR)

# Makefile created with gen AI
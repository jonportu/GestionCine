CC       = gcc
CFLAGS   = -Iinclude -Wall -Wextra -std=c11

ifeq ($(OS),Windows_NT)
    LDFLAGS = -lws2_32
else
    LDFLAGS =
endif

TARGET   = bin/servidor
SRCS     = src/main.c src/red.c
OBJS     = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o bin/*

.PHONY: all clean
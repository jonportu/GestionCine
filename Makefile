
CC       = gcc

CFLAGS   = -Iinclude -Wall -Wextra -std=c11

SQLITE_CFLAGS = -Iinclude -O2 \
                -DSQLITE_THREADSAFE=0 \
                -DSQLITE_OMIT_LOAD_EXTENSION \
                -DSQLITE_DEFAULT_MEMSTATUS=0 \
                -DSQLITE_ENABLE_FTS5=0

LDFLAGS  = -lm

TARGET   = bin/programa

SQLITE_SRC = src/sqlite3.c
SQLITE_OBJ = src/sqlite3.o

SRCS     = $(filter-out $(SQLITE_SRC), $(wildcard src/*.c))
OBJS     = $(SRCS:.c=.o)

ALL_OBJS = $(OBJS) $(SQLITE_OBJ)

all: $(TARGET)

$(TARGET): $(ALL_OBJS)
	@mkdir -p bin
	$(CC) $(ALL_OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SQLITE_OBJ): $(SQLITE_SRC)
	$(CC) $(SQLITE_CFLAGS) -c $< -o $@

clean:
	rm -rf src/*.o bin/*

.PHONY: all clean
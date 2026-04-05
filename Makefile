CC = gcc
CXX = g++
CFLAGS = -Iinclude -Wall
CXXFLAGS = -Iinclude -Wall

TARGET = bin/programa

SRCS = $(wildcard src/*.c) $(wildcard src/*.cpp)

OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf src/*.o bin/*
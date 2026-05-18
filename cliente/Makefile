CXX      = g++
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++11

ifeq ($(OS),Windows_NT)
    LDFLAGS = -lws2_32
else
    LDFLAGS =
endif

TARGET   = bin/cliente
SRCS     = src/main.cpp src/conexion.cpp
OBJS     = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o bin/*

.PHONY: all clean
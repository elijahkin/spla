CC = g++
CFLAGS = -std=c++23 -O2 -Wall -Wextra -Wpedantic -Werror

all: vector_test

vector_test: src/vector_test.cc
	$(CC) $(CFLAGS) -o bin/vector_test src/vector_test.cc

clean:
	rm -f bin/*

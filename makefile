CC = g++
CFLAGS = -std=c++2a -O2 -Wall -Wextra -Wpedantic -Werror

all: sparse_vector_test

sparse_vector_test: src/sparse_vector_test.cc
	$(CC) $(CFLAGS) -o bin/sparse_vector_test src/sparse_vector_test.cc

clean:
	rm -f bin/*

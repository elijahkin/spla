CC = g++-15
CFLAGS = -std=c++23 -O2 -Wall -Wextra -Wpedantic -Werror -fno-exceptions

all: tensor_test

tensor_test: src/tensor_test.cc
	$(CC) $(CFLAGS) -o bin/tensor_test src/tensor_test.cc

clean:
	rm -f bin/*

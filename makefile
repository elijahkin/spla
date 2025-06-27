CC = clang++
CFLAGS = -std=c++23 -O2 -Wall -Wextra -Wpedantic -Werror -fno-exceptions -fno-rtti -flto

all: tensor_test

tensor_test: src/tensor_test.cpp
	$(CC) $(CFLAGS) -o bin/tensor_test src/tensor_test.cpp

clean:
	rm -f bin/*

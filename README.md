### Introduction
This is a simple library for sparse linear algebra computations aimed toward numerical solutions of partial differential equations. The fundamental object of computation is the `spla::Tensor`. Currently, only computations for order-1 tensors, i.e. vectors, are implemented. However, we are actively working on generalizing this to tensors of arbitrary order. For example usage, see `tensor_test.cc`.

### Work in Progress
* Address `TODO` comments and thoroughly document each class and function.
* Add parallelism with OpenMP. To do this we will need the compiler flag `-fopenmp`.
* Enforce good C++ practices: `#define` guards, long member functions outside class definitions.
* Generalize to handle tensors of arbitrary rank. To accomplish this we should use the parameter pack template `size_t... Shape`. As of C++23, `operator[]` can be used for multidimensional subscripts.

### Introduction
This is a simple library for sparse linear algebra computations aimed toward numerical solutions of partial differential equations. The fundamental object of computation is the `spla::Tensor`, which generalizes the notions of scalars (order-0), vectors (order-1), matrices (order-2), and higher order tensors. For example usage, see `tensor_test.cc`.

### Work in Progress
* Address `TODO` comments and thoroughly document each class and function.
* Add parallelism with OpenMP. To do this we will need the compiler flag `-fopenmp`.
* Enforce good C++ practices: `#define` guards, long member functions outside class definitions.

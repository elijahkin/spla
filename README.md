### Introduction
This is a simple library for sparse linear algebra computations aimed toward numerical solutions of partial differential equations. Currently, only the `spla::Vector` class is implemented; we hope to eventually generalize to handle tensors of arbitrary order.

### Work in Progress
* Add `operator<=>` and update `EXPECT_NEAR` etc. to be able to accept vectors.
* Address `TODO` comments and thoroughly document each class and function.
* Add parallelism with OpenMP. To do this we will need the compiler flag `-fopenmp`.
* Enforce good C++ practices: `#define` guards, long member functions outside class definitions.
* Consider whether `shape_` would make more sense as a template parameter. We should then use CTAD on the factory constructors.
* Generalize to handle `spla::Tensor` of arbitrary rank. To accomplish this we should supply a `Shape` class, which is a wrapper around `std::vector<size_t>`. As of C++23, `operator[]` can be used for multidimensional subscripts.

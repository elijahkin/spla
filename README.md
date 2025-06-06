### Introduction
This is a simple library for sparse linear algebra computations aimed toward numerical solutions of partial differential equations. Currently, only the `spla::Vector` class is implemented; we hope to eventually generalize to handle tensors of arbitrary order.

### Work in Progress
* Ensure that binary operations can handle vectors of unlike types, assuming there is a known conversion between the types.
* Add parallelism with OpenMP.
* Generalize to handle `spla::Tensor` of arbitrary rank. To accomplish this we should supply a `Shape` class, which is a wrapper around `std::vector<size_t>`. As of C++23, `operator[]` can be used for multidimensional subscripts.
* If it doesn't require changing too much, also support dense tensors. However, this is very low priority and somewhat goes against the spirit of the library.

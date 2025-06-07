### Introduction
This is a simple library for sparse linear algebra computations aimed toward numerical solutions of partial differential equations. Currently, only the `spla::Vector` class is implemented; we hope to eventually generalize to handle tensors of arbitrary order.

### Work in Progress
* Define a version of `operator*=` for two instances of `spla::Vector`. This will help very much with code reuse; in the case where `rhs` is `T`, we can simply call the former version with `fill(lhs.shape, rhs)`.
* Do the same for `operator+=`. Then add nonmodifying versions, as well as `abs`, `operator==`, and perhaps `operator<=>`. Finally, update `EXPECT_NEAR` etc. to be able to accept vectors.
* Address `TODO` comments and write thorough documentation of each class and function.
* Ensure that binary operations can handle vectors of unlike types, assuming there is a known conversion between the types. This should probably be done using a conversion constructor.
* Add parallelism with OpenMP. We already are compiling with `-fopenmp`.
* Consider using good C++ practices: `#define` guards, move long member functions outside class definitions.
* Generalize to handle `spla::Tensor` of arbitrary rank. To accomplish this we should supply a `Shape` class, which is a wrapper around `std::vector<size_t>`. As of C++23, `operator[]` can be used for multidimensional subscripts.
* If it doesn't require changing too much, also support dense tensors. However, this is very low priority and somewhat goes against the spirit of the library.

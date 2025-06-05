### Introduction
This is a simple library for sparse linear algebra computations aimed toward numerical solutions of partial differential equations. Currently, only the `SparseVector` class is implemented; we hope to eventually generalize to handle tensors of arbitrary order.

### Work in Progress
* Add shape verification for binary operations, i.e. only vectors of the same length may be added.
* Refactor `operator[]` so that it returns a proxy object to avoid polluting `data` when called without lvalues.
* Ensure that binary operations can handle vectors of unlike types, assuming there is a known conversion between the types.
* Add a field for setting default values other than `T{}`. Make factory constructors such as `ones` and `full`.
* Use concepts to assert what we need of a type (`+`, `*`, `pow`, `abs`, etc.).
* Generalize to handle `SparseTensor` of arbitrary rank. To accomplish this we should supply a `Shape` class, which is a wrapper around `std::vector<size_t>`. As of C++23, `operator[]` can be used for multidimensional subscripts.
* If it doesn't require changing too much, also support dense tensors.

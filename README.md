## Introduction
This is a simple library for sparse linear algebra computations aimed toward numerical solutions of partial differential equations. The fundamental object of computation is the `spla::Tensor`, which generalizes the notions of scalars (order-0), vectors (order-1), matrices (order-2), and higher order tensors. For example usage, see `src/tensor_test.cc`.
## Work in Progress
The outstanding work is split broadly into three categories. **Functional** tasks pertain to the implementation and correctness of mathematical operations. **Qualitative** tasks pertain to code health, readability, and documentation. **Experimental** tasks pertain to implementation details not relevant to the user.
### Functional
* Diagnose and correct any failing tests.
* Add `eye` as a factory function.
* Ensure `operator[]` handles negative and out of bounds indices correctly.
* Investigate whether the factory functions can easily be made free.
* Implement any remaining [operator overloads](https://cppreference.com/w/cpp/language/operators.html) and any [element-wise operations](https://cppreference.com/w/cpp/header/cmath.html) from the `<cmath>` header that seem relevant.
* Refactor `reduce` so that it returns a tensor and require a list of reduction dimensions. This will allow for operations such as computing the row sums and column sums of matrices for example, rather than simply the sum of all entries.
* Consider whether supplying addition as the default reduction operation would make sense.
### Qualitative
* Can we avoid calling `decltype` for `apply_unary` and `apply_binary`?
* Can `apply_unary`, `apply_binary`, and `apply_binary_inplace` be unified at all?
* Ensure thorough documentation of each class and function except those which are self-explanatory.
* Enforce good C++ practices: `#define` guards, long member functions outside class definitions.
### Experimental
* Think about whether it makes sense to allow writing `default_value_` to `data_` explicitly.
* Add parallelism with OpenMP. To do this we will need the compiler flag `-fopenmp`.
* Declaring `data_` as `std::unordered_map` instead of `std::map` bombards me with compile errors. Diagnose why this is and whether it would be difficult to fix. If not, do so and compare performance.
* Ensure `reduce` is designed such that `any` and `all` exit early.

#include <cmath>
#include <cstddef>
#include <format>
#include <functional>
#include <ostream>
#include <unordered_map>

namespace spla {

// Defines the requirements to be the template type for a `Tensor`. In
// particular, it must support typical arithmetic operations such as addition,
// multiplication, and absolute value.
template <typename T>
concept Arithmetic = requires(T a, T b) {
  a += b;
  a -= b;
  a *= b;
  abs(a);
  exp(a);
  pow(a, b);
};

// Implements a sparse tensor represented internally by `std::unordered_map`. If
// an index is not present as a key in the map, that entry is implicitly
// `default_value_`.
template <Arithmetic T, size_t... shape> class Tensor {
public:
  // This is required by the conversion constructor.
  template <Arithmetic OtherT, size_t... other_shape> friend class Tensor;

  ///////////////////////
  // Factory Functions //
  ///////////////////////

  // TODO(elijakin) Can we make these free?
  static auto full(T default_value) {
    return Tensor<T, shape...>(default_value);
  }

  static auto zeros() { return full(0); }

  static auto ones() { return full(1); }

  //////////////////
  // Constructors //
  //////////////////

  Tensor(T default_value) : default_value_(default_value) {}

  // TODO(elijahkin) Could this be done with `apply_unary`?
  template <Arithmetic U>
  Tensor(const Tensor<U, shape...> &other)
      : default_value_(static_cast<T>(other.default_value_)) {
    for (const auto &[key, val] : other.data_) {
      data_[key] = static_cast<T>(val);
    }
  }

  /////////////////////////
  // Subscript Operation //
  /////////////////////////

  // Handles reading and writing of tensor entries. This is the object returned
  // by `operator[]`, which is necessary to ensure that default values are not
  // accidently written to `data_`.
  class SubscriptProxy {
  public:
    SubscriptProxy(Tensor<T, shape...> *vec, size_t i) : vec_(vec), i_(i) {}

    // Implements the behavior for `vec[i] = val`.
    // TODO(elijahkin) Is this the right return type for this?
    // TODO(elijahkin) Should we prevent writing default values?
    SubscriptProxy &operator=(const T &val) {
      vec_->data_[i_] = val;
      return *this;
    }

    // Casts a `SubscriptProxy` to the type `T`. If the index `i_` is present in
    // `vec_->data_`, it returns the value at that index; otherwise, it returns
    // the default value of `vec_`.
    operator T() const {
      if (auto it = vec_->data_.find(i_); it != vec_->data_.end()) {
        return it->second;
      }
      return vec_->default_value_;
    }

  private:
    Tensor<T, shape...> *vec_;
    size_t i_;
  };

  SubscriptProxy operator[](size_t i) {
    // TODO(elijahkin) Should we allow negative indices? If so, we will have to
    // slightly redesign given that size_t is unsigned.
    return SubscriptProxy(this, i);
  }

  ///////////////////
  // Miscellaneous //
  ///////////////////

  friend std::ostream &operator<<(std::ostream &os,
                                  const Tensor<T, shape...> &vec) {
    return os << std::format("{}", vec.data_);
  }

  size_t elements_in() const { return (1 * ... * shape); }

  size_t sparsity() const { return data_.size(); }

  // TODO(elijahkin) Consider whether supplying addition as a default operation
  // would make sense.
  // TODO(elijahkin) Reduce should probably return something like `Tensor<T, 1>`
  // instead of `T`; this will make generalization much easier.
  // TODO(elijahkin) Once working, restore `-Werror` as a compiler flag.
  friend auto reduce(const Tensor<T, shape...> &vec,
                     std::function<T(T, T)> op) {
    auto result = vec.default_value_ * (vec.elements_in() - vec.sparsity());
    for (const auto &[_, val] : vec.data_) {
      result = op(result, val);
    }
    return result;
  }

  ////////////////////////////
  // Elementwise Operations //
  ////////////////////////////

  auto &operator+=(const Tensor<T, shape...> &rhs) {
    return this->apply_binary_inplace([](T &a, const T &b) { a += b; }, rhs);
  }

  auto &operator-=(const Tensor<T, shape...> &rhs) {
    return this->apply_binary_inplace([](T &a, const T &b) { a -= b; }, rhs);
  }

  auto &operator*=(const Tensor<T, shape...> &rhs) {
    return this->apply_binary_inplace([](T &a, const T &b) { a *= b; }, rhs);
  }

  // TODO(elijahkin) If we introduce an `Operation` class and swap around the
  // argument order, we can merge these functions into a single variadic
  // `apply_elementwise` function.
  template <typename UnaryOp>
  friend auto apply_unary(UnaryOp op, const Tensor<T, shape...> &vec)
      -> Tensor<decltype(op(std::declval<T>())), shape...> {
    auto result = full(op(vec.default_value_));
    for (const auto &[key, val] : vec.data_) {
      result.data_[key] = op(val);
    }
    return result;
  }

  // TODO(elijahkin) This should be modified to call `apply_binary_inplace` and
  // subsequently be made private.
  template <typename BinaryOp>
  friend auto apply_binary(BinaryOp op, const Tensor<T, shape...> &lhs,
                           const Tensor<T, shape...> &rhs)
      -> Tensor<decltype(op(std::declval<T>(), std::declval<T>())), shape...> {
    auto result = full(op(lhs.default_value_, rhs.default_value_));
    for (const auto &[key, lhs_val] : lhs.data_) {
      if (auto it = rhs.data_.find(key); it != rhs.data_.end()) {
        // The key is in both lhs and rhs
        result.data_[key] = op(lhs_val, it->second);
      } else {
        // The key is only in lhs
        result.data_[key] = op(lhs_val, rhs.default_value_);
      }
    }
    for (const auto &[key, rhs_val] : rhs.data_) {
      if (auto it = lhs.data_.find(key); it == lhs.data_.end()) {
        // The key is only in rhs
        result.data_[key] = op(lhs.default_value_, rhs_val);
      }
    }
    return result;
  }

  friend Tensor<T, shape...> pow(const Tensor<T, shape...> &lhs,
                                 const Tensor<T, shape...> &rhs) {
    return apply_binary([](T a, T b) { return std::pow(a, b); }, lhs, rhs);
  }

private:
  std::unordered_map<size_t, T> data_;
  T default_value_;

  auto &apply_binary_inplace(std::function<void(T &, const T &)> op,
                             const Tensor<T, shape...> &rhs) {
    for (auto &[key, lhs_val] : data_) {
      if (auto it = rhs.data_.find(key); it == rhs.data_.end()) {
        // The key is only in lhs
        op(lhs_val, rhs.default_value_);
      }
    }
    for (const auto &[key, rhs_val] : rhs.data_) {
      if (auto it = data_.find(key); it == data_.end()) {
        // The key is only in rhs
        data_[key] = default_value_;
      }
      op(data_[key], rhs_val);
    }
    op(default_value_, rhs.default_value_);
    return *this;
  }
};

////////////////////////////
// Elementwise Operations //
////////////////////////////

template <Arithmetic T, size_t... shape>
auto abs(const Tensor<T, shape...> &vec) {
  return apply_unary([](T a) { return std::abs(a); }, vec);
}

template <Arithmetic T, size_t... shape>
auto exp(const Tensor<T, shape...> &vec) {
  return apply_unary([](T a) { return std::exp(a); }, vec);
}

template <Arithmetic T, size_t... shape>
auto operator+(const Tensor<T, shape...> &lhs, const Tensor<T, shape...> &rhs) {
  return apply_binary([](T a, T b) { return a + b; }, lhs, rhs);
}

template <Arithmetic T, size_t... shape>
auto operator-(const Tensor<T, shape...> &lhs, const Tensor<T, shape...> &rhs) {
  return apply_binary([](T a, T b) { return a - b; }, lhs, rhs);
}

template <Arithmetic T, size_t... shape>
auto operator*(const Tensor<T, shape...> &lhs, const Tensor<T, shape...> &rhs) {
  return apply_binary([](T a, T b) { return a * b; }, lhs, rhs);
}

template <Arithmetic T, size_t... shape>
auto operator<(const Tensor<T, shape...> &lhs, const Tensor<T, shape...> &rhs) {
  return apply_binary([](T a, T b) { return a < b; }, lhs, rhs);
}

// TODO(elijahkin) We can easily add the other comparators if needed.

template <Arithmetic T, size_t... shape>
auto operator==(const Tensor<T, shape...> &lhs,
                const Tensor<T, shape...> &rhs) {
  return apply_binary([](T a, T b) { return a == b; }, lhs, rhs);
}

///////////////////////
// Reduce Operations //
///////////////////////

// TODO(elijahkin) Eventually we should update `reduce` to ensure `all` and
// `any` exit early.
template <size_t... shape> bool all(const Tensor<bool, shape...> &vec) {
  return reduce(vec, [](bool a, bool b) { return a && b; });
}

template <size_t... shape> bool any(const Tensor<bool, shape...> &vec) {
  return reduce(vec, [](bool a, bool b) { return a || b; });
}

template <Arithmetic T, size_t... shape>
T dot(const Tensor<T, shape...> &lhs, const Tensor<T, shape...> &rhs) {
  return reduce(lhs * rhs, [](T a, T b) { return a + b; });
}

template <Arithmetic T, size_t... shape>
double norm(const Tensor<T, shape...> &vec, int ord) {
  return std::pow(reduce(pow(abs(vec), ord), [](T a, T b) { return a + b; }),
                  1.0 / ord);
}

} // namespace spla

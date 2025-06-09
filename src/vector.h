#include <cmath>
#include <format>
#include <unordered_map>

namespace spla {

// Defines the requirements to be the template type for a `Vector`. In
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

// Implements a sparse vector represented internally by `std::unordered_map`. If
// an index `i` between 0 and `Shape` is not present as a key in the map, that
// entry is implicitly `default_value_`.
template <Arithmetic T, size_t Shape> class Vector {
public:
  //////////////////
  // Constructors //
  //////////////////

  Vector<T, Shape>(T default_value) : default_value_(default_value) {}

  static Vector<T, Shape> full(T default_value) {
    return Vector<T, Shape>(default_value);
  }

  static Vector<T, Shape> zeros() { return full(0); }

  static Vector<T, Shape> ones() { return full(1); }

  /////////////////////
  // Type Conversion //
  /////////////////////

  // This is needed in order for the conversion constructor to access the
  // private fields of Vector<U>.
  template <Arithmetic U, size_t UShape> friend class Vector;

  // TODO(elijahkin) This can likely be done with `apply_unary`
  template <Arithmetic U>
  Vector(const Vector<U, Shape> &other)
      : default_value_(static_cast<T>(other.default_value_)) {
    for (const auto &[key, val] : other.data_) {
      data_[key] = static_cast<T>(val);
    }
  }

  ////////////////////////
  // Subscript Operator //
  ////////////////////////

  // Handles reading and writing of vector elements. This is the object returned
  // by `operator[]`, which is necessary to ensure that default values are not
  // accidently written to `data_`.
  class SubscriptProxy {
  public:
    SubscriptProxy(Vector<T, Shape> *vec, size_t i) : vec_(vec), i_(i) {}

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
    Vector<T, Shape> *vec_;
    size_t i_;
  };

  SubscriptProxy operator[](size_t i) {
    // TODO(elijahkin) Should we allow negative indices? If so, we will have to
    // slightly redesign given that size_t is unsigned.
    if (i >= Shape) {
      throw std::out_of_range("Index out of range");
    }
    return SubscriptProxy(this, i);
  }

  /////////////////////////
  // Modifying Operators //
  /////////////////////////

  Vector<T, Shape> &operator+=(const Vector<T, Shape> &rhs) {
    return this->apply_binary_inplace(rhs, [](T &a, const T &b) { a += b; });
  }

  Vector<T, Shape> &operator-=(const Vector<T, Shape> &rhs) {
    return this->apply_binary_inplace(rhs, [](T &a, const T &b) { a -= b; });
  }

  Vector<T, Shape> &operator*=(const Vector<T, Shape> &rhs) {
    return this->apply_binary_inplace(rhs, [](T &a, const T &b) { a *= b; });
  }

  ////////////////////////////
  // Nonmodifying Operators //
  ////////////////////////////

  friend auto exp(const Vector<T, Shape> &vec) {
    return apply_unary(vec, [](T a) { return exp(a); });
  }

  friend Vector<T, Shape> abs(const Vector<T, Shape> &vec) {
    return apply_unary(vec, [](T a) { return abs(a); });
  }

  friend Vector<T, Shape> pow(const Vector<T, Shape> &lhs,
                              const Vector<T, Shape> &rhs) {
    return apply_binary(lhs, rhs, [](T a, T b) { return pow(a, b); });
  }

  ///////////////////
  // Miscellaneous //
  ///////////////////

  friend std::ostream &operator<<(std::ostream &os,
                                  const Vector<T, Shape> &vec) {
    return os << std::format("{}", vec.data_);
  }

  size_t sparsity() const { return data_.size(); }

private:
  std::unordered_map<size_t, T> data_;
  T default_value_;

  // TODO(elijahkin) Should `op` be `std::function` instead? Moreover, consider
  // whether supplying addition as a default operation makes sense.
  friend T reduce(const Vector<T, Shape> &vec, T (*op)(T, T)) {
    T result = vec.default_value_ * (Shape - vec.data_.size());
    for (const auto &[_, val] : vec.data_) {
      result = op(result, val);
    }
    return result;
  }

  ////////////////////////////
  // Elementwise Operations //
  ////////////////////////////

  // TODO(elijahkin) Try to somehow marry this function with those below.
  template <typename BinaryOp>
  Vector<T, Shape> &apply_binary_inplace(const Vector<T, Shape> &rhs,
                                         BinaryOp op) {
    for (auto &[key, lhs_val] : data_) {
      if (auto it = rhs.data_.find(key); it == rhs.data_.end()) {
        op(lhs_val, rhs.default_value_);
      }
    }
    for (const auto &[key, rhs_val] : rhs.data_) {
      if (auto it = data_.find(key); it == data_.end()) {
        data_[key] = default_value_;
      }
      op(data_[key], rhs_val);
    }
    op(default_value_, rhs.default_value_);
    return *this;
  }

  // TODO(elijahkin) If we introduce an `Operation` class and swap around the
  // argument order, we can merge these functions into a single variadic
  // `apply_elementwise` function.
  template <typename UnaryOp>
  friend auto apply_unary(const Vector<T, Shape> &vec, UnaryOp op)
      -> Vector<decltype(op(std::declval<T>())), Shape> {
    using U = decltype(op(std::declval<T>()));
    Vector<U, Shape> result = zeros();
    for (const auto &[key, val] : vec.data_) {
      result.data_[key] = op(val);
    }
    result.default_value_ = op(vec.default_value_);
    return result;
  }

  template <typename BinaryOp>
  friend auto apply_binary(const Vector<T, Shape> &lhs,
                           const Vector<T, Shape> &rhs, BinaryOp op)
      -> Vector<decltype(op(std::declval<T>(), std::declval<T>())), Shape> {
    using U = decltype(op(std::declval<T>(), std::declval<T>()));
    Vector<U, Shape> result = zeros();
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
    result.default_value_ = op(lhs.default_value_, rhs.default_value_);
    return result;
  }
};

////////////////////////////
// Nonmodifying Operators //
////////////////////////////

template <Arithmetic T, size_t Shape>
Vector<T, Shape> operator+(const Vector<T, Shape> &lhs,
                           const Vector<T, Shape> &rhs) {
  return apply_binary(lhs, rhs, [](T a, T b) { return a + b; });
}

template <Arithmetic T, size_t Shape>
Vector<T, Shape> operator-(const Vector<T, Shape> &lhs,
                           const Vector<T, Shape> &rhs) {
  return apply_binary(lhs, rhs, [](T a, T b) { return a - b; });
}

template <Arithmetic T, size_t Shape>
Vector<T, Shape> operator*(const Vector<T, Shape> &lhs,
                           const Vector<T, Shape> &rhs) {
  return apply_binary(lhs, rhs, [](T a, T b) { return a * b; });
}

template <Arithmetic T, size_t Shape>
Vector<bool, Shape> operator==(const Vector<T, Shape> &lhs,
                               const Vector<T, Shape> &rhs) {
  return apply_binary(lhs, rhs, [](T a, T b) { return a == b; });
}

template <Arithmetic T, size_t Shape>
Vector<bool, Shape> operator<(const Vector<T, Shape> &lhs,
                              const Vector<T, Shape> &rhs) {
  return apply_binary(lhs, rhs, [](T a, T b) { return a < b; });
}

// TODO(elijahkin) We can easily add the other comparators if needed.

template <Arithmetic T, size_t Shape>
T dot(const Vector<T, Shape> &lhs, const Vector<T, Shape> &rhs) {
  return reduce(lhs * rhs, [](T a, T b) { return a + b; });
}

template <Arithmetic T, size_t Shape>
double norm(const Vector<T, Shape> &vec, int ord) {
  return pow(reduce(pow(abs(vec), ord), [](T a, T b) { return a + b; }),
             1.0 / ord);
}

// TODO(elijahkin) Eventually we should update `reduce` to ensure these exit
// early
template <size_t Shape> bool any(const Vector<bool, Shape> &vec) {
  return reduce(vec, [](bool a, bool b) { return a || b; });
}

template <size_t Shape> bool all(const Vector<bool, Shape> &vec) {
  return reduce(vec, [](bool a, bool b) { return a && b; });
}

} // namespace spla

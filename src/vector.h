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

typedef size_t Shape;

// Implements a sparse vector of length `shape_` represented internally as a
// map. If an index `i` between 0 and `shape_` is not present as a key in the
// map, that entry is implicitly `default_value_`.
template <Arithmetic T> class Vector {
public:
  static Vector<T> full(Shape shape, T default_value) {
    return Vector<T>(shape, default_value);
  }

  static Vector<T> zeros(Shape shape) { return full(shape, 0); }

  static Vector<T> ones(Shape shape) { return full(shape, 1); }

  // This is needed in order for the conversion constructor to access the
  // private fields of Vector<U>.
  template <Arithmetic U> friend class Vector;

  // TODO(elijahkin) This can likely be done with `apply_unary`
  template <Arithmetic U>
  Vector(const Vector<U> &other)
      : shape_(other.shape_),
        default_value_(static_cast<T>(other.default_value_)) {
    for (const auto &[key, val] : other.data_) {
      data_[key] = static_cast<T>(val);
    }
  }

  friend std::ostream &operator<<(std::ostream &os, const Vector<T> &vec) {
    return os << std::format("{}", vec.data_);
  }

  size_t sparsity() const { return data_.size(); }

  // Handles reading and writing of vector elements. This is the object returned
  // by `operator[]`, which is necessary to ensure that default values are not
  // accidently written to `data_`.
  class SubscriptProxy {
  public:
    SubscriptProxy(Vector<T> *vec, size_t i) : vec_(vec), i_(i) {}

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
    Vector<T> *vec_;
    size_t i_;
  };

  SubscriptProxy operator[](size_t i) {
    // TODO(elijahkin) Should we allow negative indices? If so, we will have to
    // slightly redesign given that size_t is unsigned.
    if (i >= shape_) {
      throw std::out_of_range("Index out of range");
    }
    return SubscriptProxy(this, i);
  }

  /////////////////////////
  // Modifying Operators //
  /////////////////////////

  Vector<T> &operator+=(const Vector<T> &rhs) {
    return this->apply_binary_inplace(
        rhs, [](T &a, const T &b) { a += b; }, "+=");
  }

  Vector<T> &operator-=(const Vector<T> &rhs) {
    return this->apply_binary_inplace(
        rhs, [](T &a, const T &b) { a -= b; }, "-=");
  }

  Vector<T> &operator*=(const Vector<T> &rhs) {
    return this->apply_binary_inplace(
        rhs, [](T &a, const T &b) { a *= b; }, "*=");
  }

  // TODO(elijahkin) We can eliminate these if shape is a template parameter.
  Vector<T> &operator+=(T rhs) { return *this += full(shape_, rhs); }

  Vector<T> &operator*=(T rhs) { return *this *= full(shape_, rhs); }

  ////////////////////////////
  // Nonmodifying Operators //
  ////////////////////////////

  friend Vector<T> abs(const Vector<T> &vec) {
    return apply_unary(vec, [](T a) { return abs(a); });
  }

  friend auto exp(const Vector<T> &vec) {
    return apply_unary(vec, [](T a) { return exp(a); });
  }

  friend Vector<T> pow(const Vector<T> &lhs, const Vector<T> &rhs) {
    return apply_binary(lhs, rhs, [](T a, T b) { return pow(a, b); }, "pow");
  }

  // TODO(elijahkin) Once shape is a template parameter, this can be made free
  friend double norm(const Vector<T> &vec, int ord) {
    auto exp = full(vec.shape_, ord);
    auto sum = reduce(pow(abs(vec), exp), [](T a, T b) { return a + b; });
    return pow(sum, 1.0 / ord);
  }

private:
  std::unordered_map<size_t, T> data_;
  Shape shape_;
  T default_value_;

  Vector(Shape shape, T default_value)
      : shape_(shape), default_value_(default_value) {}

  // TODO(elijahkin) Should `op` be `std::function` instead? Moreover, consider
  // whether supplying addition as a default operation makes sense.
  friend T reduce(const Vector<T> &vec, T (*op)(T, T)) {
    T result = vec.default_value_ * (vec.shape_ - vec.data_.size());
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
  Vector<T> &apply_binary_inplace(const Vector<T> &rhs, BinaryOp op,
                                  const std::string &op_name) {
    if (shape_ != rhs.shape_) {
      throw std::invalid_argument(op_name +
                                  " expects operands of the same shape.");
    }
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
  friend auto apply_unary(const Vector<T> &vec, UnaryOp op)
      -> Vector<decltype(op(std::declval<T>()))> {
    using U = decltype(op(std::declval<T>()));
    Vector<U> result = zeros(vec.shape_);
    for (const auto &[key, val] : vec.data_) {
      result.data_[key] = op(val);
    }
    result.default_value_ = op(vec.default_value_);
    return result;
  }

  template <typename BinaryOp>
  friend auto apply_binary(const Vector<T> &lhs, const Vector<T> &rhs,
                           BinaryOp op, const std::string &op_name)
      -> Vector<decltype(op(std::declval<T>(), std::declval<T>()))> {
    if (lhs.shape_ != rhs.shape_) {
      throw std::invalid_argument(op_name +
                                  " expects operands of the same shape.");
    }
    using U = decltype(op(std::declval<T>(), std::declval<T>()));
    Vector<U> result = zeros(lhs.shape_);
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

template <Arithmetic T>
Vector<T> operator+(const Vector<T> &lhs, const Vector<T> &rhs) {
  return apply_binary(lhs, rhs, [](T a, T b) { return a + b; }, "+");
}

template <Arithmetic T>
Vector<T> operator-(const Vector<T> &lhs, const Vector<T> &rhs) {
  return apply_binary(lhs, rhs, [](T a, T b) { return a - b; }, "-");
}

template <Arithmetic T>
Vector<T> operator*(const Vector<T> &lhs, const Vector<T> &rhs) {
  return apply_binary(lhs, rhs, [](T a, T b) { return a * b; }, "*");
}

template <Arithmetic T>
Vector<bool> operator==(const Vector<T> &lhs, const Vector<T> &rhs) {
  return apply_binary(lhs, rhs, [](T a, T b) { return a == b; }, "==");
}

template <Arithmetic T>
Vector<bool> operator<(const Vector<T> &lhs, const Vector<T> &rhs) {
  return apply_binary(lhs, rhs, [](T a, T b) { return a < b; }, "<");
}

// TODO(elijahkin) We can easily add the other comparators if needed.

template <Arithmetic T> T dot(const Vector<T> &lhs, const Vector<T> &rhs) {
  return reduce(lhs * rhs, [](T a, T b) { return a + b; });
}

// TODO(elijahkin) Eventually we should update `reduce` to ensure these exit
// early
bool any(const Vector<bool> &vec) {
  return reduce(vec, [](bool a, bool b) { return a || b; });
}

bool all(const Vector<bool> &vec) {
  return reduce(vec, [](bool a, bool b) { return a && b; });
}

} // namespace spla

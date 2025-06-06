#include <cmath>
#include <cstddef>
#include <format>
#include <ostream>
#include <unordered_map>

namespace spla {

// Defines the requirements to be the template type for a `Vector`. In
// particular, it must support addition, multiplication, and absolute value.
template <typename T>
concept Scalar = requires(T a, T b) {
  a += b;
  a *= b;
  abs(a);
};

// Implements a sparse vector of length `shape_` represented internally as a
// map. If an index `i` between 0 and `shape_` is not present as a key in the
// map, that entry is implicitly `default_value_`.
template <Scalar T> class Vector {
public:
  static Vector<T> fill(size_t shape, T default_value) {
    return Vector<T>(shape, default_value);
  }

  static Vector<T> zeros(size_t shape) { return fill(shape, 0); }

  static Vector<T> ones(size_t shape) { return fill(shape, 1); }

  Vector<T> &operator*=(T rhs) {
    default_value_ *= rhs;
    if (rhs == T{}) {
      data_.clear();
    } else {
      for (auto &[_, val] : data_) {
        val *= rhs;
      }
    }
    return *this;
  }

  Vector<T> &operator+=(const Vector &rhs) {
    if (shape_ != rhs.shape_) {
      throw std::invalid_argument(
          "Addition expects operands of the same shape.");
    }
    default_value_ += rhs.default_value_;
    for (const auto &[key, rhs_val] : rhs.data_) {
      auto &val = data_[key];
      val += rhs_val;
      if (val == default_value_) {
        data_.erase(key);
      }
    }
    return *this;
  }

  friend T inner(const Vector &lhs, const Vector &rhs) {
    if (lhs.shape_ != rhs.shape_) {
      throw std::invalid_argument(
          "Inner product expects operands of the same shape");
    }
    // TODO(elijahkin) Canonicalize to always iterate over the smaller map
    T dot = T{};
    for (const auto &[key, lhs_val] : lhs.data_) {
      if (auto it = rhs.data_.find(key); it != rhs.data_.end()) {
        const T rhs_val = it->second;
        dot += lhs_val * rhs_val;
      }
    }
    return dot;
  }

  friend std::ostream &operator<<(std::ostream &os, const Vector &vec) {
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

  double norm(int ord) const {
    double sum = (shape_ - data_.size()) * pow(abs(default_value_), ord);
    for (const auto &[_, val] : data_) {
      sum += pow(abs(val), ord);
    }
    return pow(sum, 1.0 / ord);
  }

private:
  std::unordered_map<size_t, T> data_;
  size_t shape_;
  T default_value_;

  Vector(size_t shape, T default_value)
      : shape_(shape), default_value_(default_value) {}
};

} // namespace spla

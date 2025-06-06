#include <cmath>
#include <cstddef>
#include <format>
#include <ostream>
#include <unordered_map>

namespace spla {

// Defines the requirements to be the template type for a `Vector`. In
// particular, it must support addition, multiplication, and absolute value.
template <typename T>
concept Arithmetic = requires(T a, T b) {
  a += b;
  a *= b;
  abs(a);
};

typedef size_t Shape;

// Implements a sparse vector of length `shape_` represented internally as a
// map. If an index `i` between 0 and `shape_` is not present as a key in the
// map, that entry is implicitly `default_value_`.
template <Arithmetic T> class Vector {
public:
  static Vector<T> fill(Shape shape, T default_value) {
    return Vector<T>(shape, default_value);
  }

  static Vector<T> zeros(Shape shape) { return fill(shape, 0); }

  static Vector<T> ones(Shape shape) { return fill(shape, 1); }

  // This is needed in order for the conversion constructor to access the
  // private fields of Vector<U>.
  template <Arithmetic U> friend class Vector;

  template <Arithmetic U>
  explicit Vector(const Vector<U> &other)
      : shape_(other.shape_),
        default_value_(static_cast<T>(other.default_value_)) {
    for (const auto &[key, val] : other.data_) {
      data_[key] = static_cast<T>(val);
    }
  }

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

  Vector<T> &operator+=(T rhs) { return *this += fill(shape_, rhs); }

  // Implements the behavior for `vec1 == vec2`, which are considered equal if
  // they have the same shape, default value, and data. The behavior for `vec1
  // != vec2` is also inferred from this.
  friend bool operator==(const Vector &lhs, const Vector &rhs) {
    return lhs.shape_ == rhs.shape_ &&
           lhs.default_value_ == rhs.default_value_ && lhs.data_ == rhs.data_;
  }

  friend Vector operator+(Vector lhs, const Vector &rhs) { return lhs += rhs; }

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

  // TODO(elijahkin) Should we implement a spaceship operator?

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
  Shape shape_;
  T default_value_;

  Vector(Shape shape, T default_value)
      : shape_(shape), default_value_(default_value) {}
};

} // namespace spla

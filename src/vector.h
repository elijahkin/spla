#include <cmath>
#include <cstddef>
#include <format>
#include <ostream>
#include <unordered_map>

namespace spla {

// Defines the requirements to be the template type for a `Vector`. In
// particular, it must support addition, multiplication, and absolute value.
template <typename T> concept Scalar = requires(T a, T b) {
  a += b;
  a *= b;
  abs(a);
};

// Implements a sparse vector of length `shape_` represented internally as a
// map. If an index `i` between 0 and `shape_` is not present as a key in the
// map, that entry is implicitly 0.
template <Scalar T> class Vector {
public:
  static Vector<T> zeros(size_t shape) { return Vector<T>(shape); }

  Vector &operator*=(T rhs) {
    // TODO(elijahkin) Must change this to support types with zero divisors
    if (rhs == T{}) {
      // Maintain sparsity in the event we multiply by zero
      data_.clear();
    } else {
      for (auto &[_, val] : data_) {
        val *= rhs;
      }
    }
    return *this;
  }

  Vector &operator+=(const Vector &rhs) {
    for (const auto &[key, val] : rhs.data_) {
      data_[key] += val;
      if (data_[key] == T{}) {
        // Maintain sparsity in the event we added additive inverse
        data_.erase(key);
      }
    }
    return *this;
  }

  friend T inner(const Vector &lhs, const Vector &rhs) {
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

  T &operator[](size_t i) { return data_[i]; }

  double norm(int ord) const {
    double sum = 0;
    for (const auto &[_, val] : data_) {
      sum += pow(abs(val), ord);
    }
    return pow(sum, 1.0 / ord);
  }

private:
  std::unordered_map<size_t, T> data_;
  size_t shape_;

  Vector(size_t shape) : shape_(shape) {}
};

} // namespace spla

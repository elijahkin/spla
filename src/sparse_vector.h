#include <cmath>
#include <format>
#include <unordered_map>

template <typename T> class SparseVector {
private:
  std::unordered_map<size_t, T> data;

public:
  SparseVector() {}

  // Scalar multiplication
  SparseVector &operator*=(T rhs) {
    // TODO We'll have to change this to support types with zero divisors
    if (rhs == T{}) {
      // Maintain sparsity in the event we multiply by zero
      data.clear();
    } else {
      for (auto &[_, val] : data) {
        val *= rhs;
      }
    }
    return *this;
  }

  // Addition
  SparseVector &operator+=(const SparseVector &rhs) {
    for (const auto &[key, val] : rhs.data) {
      data[key] += val;
      if (data[key] == T{}) {
        // Maintain sparsity in the event we added additive inverse
        data.erase(key);
      }
    }
    return *this;
  }

  // Inner product
  friend T inner(const SparseVector &lhs, const SparseVector &rhs) {
    // TODO Canonicalize so that lhs is guaranteed to be the smaller map

    T dot = T{};
    for (const auto &[key, lhs_val] : lhs.data) {
      auto it = rhs.data.find(key);
      if (it != rhs.data.end()) {
        const T rhs_val = it->second;
        dot += lhs_val * rhs_val;
      }
    }
    return dot;
  }

  // Printing (in the style of Python dictionary)
  friend std::ostream &operator<<(std::ostream &os, const SparseVector &vec) {
    return os << std::format("{}", vec.data);
  }

  // Subscript
  T &operator[](size_t i) { return data[i]; }

  // Norm
  float norm(int ord) const {
    float sum = 0;
    for (const auto &[_, val] : data) {
      sum += pow(abs(val), ord);
    }
    return pow(sum, 1.0 / ord);
  }
};

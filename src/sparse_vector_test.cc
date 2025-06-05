#include <cassert>
#include <iostream>

#include "sparse_vector.h"

static void basic_sparse_vector() {
  auto v = SparseVector<int>::zeros(10);
  assert(std::abs(v.norm(2) - 0) < 1e-6);

  v[1] = -7;
  v[3] = 4;
  v *= 2;
  assert(v[1] == -14);
  assert(v[3] == 8);

  auto w = SparseVector<int>::zeros(20);
  w[3] = 2;
  w[5] = 1;
  assert(w[3] == 2);
  assert(w[5] == 1);

  v += w;
  assert(v[1] == -14);
  assert(v[3] == 10);
  assert(v[5] == 1);

  assert(std::abs(v.norm(1) - 25) < 1e-6);
  assert(std::abs(v.norm(2) - sqrt(297)) < 1e-6);

  assert(inner(v, w) == 21);

  std::cout << v << std::endl;
  std::cout << w << std::endl;
}

int main() {
  basic_sparse_vector();
  return 0;
}

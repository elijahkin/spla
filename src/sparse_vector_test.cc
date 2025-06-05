#include "sparse_vector.h"
#include <cassert>
#include <iostream>

int main() {
  SparseVector<int> v;
  assert(v.norm(2) == 0);

  v[1] = -7;
  v[3] = 4;
  v *= 2;
  assert(v[1] = -14);
  assert(v[3] = 8);

  SparseVector<int> w;
  w[3] = 2;
  w[5] = 1;
  assert(w[3] == 2);
  assert(w[5] == 1);

  v += w;
  assert(v[1] = -14);
  assert(v[3] = 10);
  assert(v[5] == 1);

  assert(v.norm(1) == 25);
  assert(abs(v.norm(2) - sqrt(297)) < 1e-4);

  assert(inner(v, w) == 21);

  std::cout << v.to_string() << std::endl;
  std::cout << w.to_string() << std::endl;
  return 0;
}

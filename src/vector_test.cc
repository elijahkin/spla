#include "test.h"
#include "vector.h"

TEST(VectorBasics) {
  auto v = spla::Vector<int>::zeros(10);
  ASSERT_CLOSE(v.norm(2), 0);

  v[1] = -7;
  v[3] = 4;
  v *= 2;
  ASSERT_EQ(v[1], -14);
  ASSERT_EQ(v[3], 8);

  auto w = spla::Vector<int>::zeros(20);
  w[3] = 2;
  w[5] = 1;
  ASSERT_EQ(w[3], 2);
  ASSERT_EQ(w[5], 1);

  v += w;
  ASSERT_EQ(v[1], -14);
  ASSERT_EQ(v[3], 10);
  ASSERT_EQ(v[5], 1);

  ASSERT_CLOSE(v.norm(1), 25);
  ASSERT_CLOSE(v.norm(2), sqrt(297));
  ASSERT_EQ(inner(v, w), 21);
}

TEST(SubscriptSparsity) {
  auto v = spla::Vector<double>::zeros(5);
  v[0] = 1.0;
  v[2] = 1.0;
  v[3] = 1.0;
  ASSERT_EQ(v.sparsity(), 3);

  ASSERT_CLOSE(v[0], 1);
  ASSERT_CLOSE(v[1], 0);
  ASSERT_EQ(v.sparsity(), 3);
}

TEST(NonzeroDefaultValues) {
  auto v = spla::Vector<double>::ones(3);
  ASSERT_CLOSE(v[0], 1);
  ASSERT_CLOSE(v.norm(2), sqrt(3));

  auto w = spla::Vector<double>::fill(3, 2);
  ASSERT_CLOSE(w[0], 2);
  ASSERT_CLOSE(w.norm(2), sqrt(12));

  v += w;
  ASSERT_CLOSE(v[0], 3);
  ASSERT_CLOSE(v.norm(2), sqrt(27));
}

int main() {
  VectorBasics();
  SubscriptSparsity();
  NonzeroDefaultValues();
  return 0;
}

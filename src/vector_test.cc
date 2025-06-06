#include "test.h"
#include "vector.h"

TEST(VectorBasics) {
  auto v = spla::Vector<int>::zeros(10);
  ASSERT_ALMOST_EQ(v.norm(2), 0);

  v[1] = -7;
  v[3] = 4;
  v *= 2;
  ASSERT_EQ(v[1], -14);
  ASSERT_EQ(v[3], 8);

  auto w = spla::Vector<int>::zeros(10);
  w[3] = 2;
  w[5] = 1;
  ASSERT_EQ(w[3], 2);
  ASSERT_EQ(w[5], 1);

  v += w;
  ASSERT_EQ(v[1], -14);
  ASSERT_EQ(v[3], 10);
  ASSERT_EQ(v[5], 1);

  ASSERT_ALMOST_EQ(v.norm(1), 25);
  ASSERT_ALMOST_EQ(v.norm(2), sqrt(297));
  ASSERT_EQ(inner(v, w), 21);
}

TEST(SubscriptSparsity) {
  auto v = spla::Vector<double>::zeros(5);
  v[0] = 1.0;
  v[2] = 1.0;
  v[3] = 1.0;
  ASSERT_EQ(v.sparsity(), 3);

  ASSERT_ALMOST_EQ(v[0], 1);
  ASSERT_ALMOST_EQ(v[1], 0);
  ASSERT_EQ(v.sparsity(), 3);
}

TEST(NonzeroDefaultValues) {
  auto v = spla::Vector<double>::ones(3);
  ASSERT_ALMOST_EQ(v[0], 1);
  ASSERT_ALMOST_EQ(v.norm(2), sqrt(3));

  auto w = spla::Vector<double>::fill(3, 2);
  ASSERT_ALMOST_EQ(w[0], 2);
  ASSERT_ALMOST_EQ(w.norm(2), sqrt(12));

  v += w;
  ASSERT_ALMOST_EQ(v[0], 3);
  ASSERT_ALMOST_EQ(v.norm(2), sqrt(27));
}

TEST(ShapeExceptions) {
  auto v1 = spla::Vector<int>::zeros(5);
  auto v2 = spla::Vector<int>::zeros(3);
  auto v3 = spla::Vector<int>::zeros(5);

  EXPECT_NOT_OK(v1 += v2);
  EXPECT_OK(v1 += v3);

  EXPECT_NOT_OK(inner(v2, v1));
  EXPECT_OK(inner(v3, v1));

  EXPECT_OK(v1[0]);
  EXPECT_NOT_OK(v1[-1]);

  EXPECT_OK(v2[2]);
  EXPECT_NOT_OK(v2[3]);
}

int main() {
  // TODO(elijahkin) Build this up automatically from the TEST macro
  RunAll({{"VectorBasics", VectorBasics},
          {"SubscriptSparsity", SubscriptSparsity},
          {"NonzeroDefaultValues", NonzeroDefaultValues},
          {"ShapeExceptions", ShapeExceptions}});
  return 0;
}

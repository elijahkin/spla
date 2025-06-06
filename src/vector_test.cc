#include "test.h"
#include "vector.h"

TEST(VectorBasics) {
  auto v = spla::Vector<int>::zeros(10);
  EXPECT_NEAR(v.norm(2), 0, 1e-6);

  v[1] = -7;
  v[3] = 4;
  v *= 2;
  EXPECT_EQ(v[1], -14);
  EXPECT_EQ(v[3], 8);

  auto w = spla::Vector<int>::zeros(10);
  w[3] = 2;
  w[5] = 1;
  EXPECT_EQ(w[3], 2);
  EXPECT_EQ(w[5], 1);

  v += w;
  EXPECT_EQ(v[1], -14);
  EXPECT_EQ(v[3], 10);
  EXPECT_EQ(v[5], 1);

  EXPECT_NEAR(v.norm(1), 25, 1e-6);
  EXPECT_NEAR(v.norm(2), sqrt(297), 1e-6);
  EXPECT_EQ(inner(v, w), 21);
}

TEST(SubscriptSparsity) {
  auto v = spla::Vector<double>::zeros(5);
  v[0] = 1.0;
  v[2] = 1.0;
  v[3] = 1.0;
  EXPECT_EQ(v.sparsity(), 3);

  EXPECT_NEAR(v[0], 1, 1e-6);
  EXPECT_NEAR(v[1], 0, 1e-6);
  EXPECT_EQ(v.sparsity(), 3);
}

TEST(NonzeroDefaultValues) {
  auto v = spla::Vector<double>::ones(3);
  EXPECT_NEAR(v[0], 1, 1e-6);
  EXPECT_NEAR(v.norm(2), sqrt(3), 1e-6);

  auto w = spla::Vector<double>::fill(3, 2);
  EXPECT_NEAR(w[0], 2, 1e-6);
  EXPECT_NEAR(w.norm(2), sqrt(12), 1e-6);

  v += w;
  EXPECT_NEAR(v[0], 3, 1e-6);
  EXPECT_NEAR(v.norm(2), sqrt(27), 1e-6);
}

TEST(ShapeExceptions) {
  auto v1 = spla::Vector<int>::zeros(5);
  auto v2 = spla::Vector<int>::zeros(3);
  auto v3 = spla::Vector<int>::zeros(5);

  EXPECT_THROW(v1 += v2, std::invalid_argument);
  EXPECT_NO_THROW(v1 += v3);

  EXPECT_THROW(inner(v2, v1), std::invalid_argument);
  EXPECT_NO_THROW(inner(v3, v1));

  EXPECT_NO_THROW(v1[0]);
  EXPECT_THROW(v1[-1], std::out_of_range);

  EXPECT_NO_THROW(v2[2]);
  EXPECT_THROW(v2[3], std::out_of_range);
}

TEST(Equality) {
  auto v1 = spla::Vector<int>::zeros(5);
  auto v2 = spla::Vector<int>::zeros(5);
  auto v3 = spla::Vector<int>::ones(5);

  EXPECT_EQ(v1, v2);
  EXPECT_NE(v1, v3);
  EXPECT_NE(v2, v3);

  v2[0] = 1;
  EXPECT_NE(v1, v2);
}

TEST(NonmodifyingAddition) {
  auto one = spla::Vector<int>::ones(3);
  auto two = spla::Vector<int>::fill(3, 2);
  auto three = spla::Vector<int>::fill(3, 3);

  auto v = one + two;
  EXPECT_EQ(v, three);
}

TEST(TypeConversion) {
  auto one_int = spla::Vector<int>::ones(3);
  auto one_double = spla::Vector<double>::ones(3);
  auto two_double = spla::Vector<double>::fill(3, 2);

  auto v = static_cast<spla::Vector<double>>(one_int);
  EXPECT_EQ(v, one_double);
  EXPECT_NE(v, two_double);
}

TEST(AdditionWithScalar) {
  auto v = spla::Vector<int>::zeros(3);
  auto one = spla::Vector<int>::ones(3);
  auto two = spla::Vector<int>::fill(3, 2);

  EXPECT_NE(v, one);
  EXPECT_NE(v, two);

  v += 1;
  EXPECT_EQ(v, one);
  EXPECT_NE(v, two);

  v += 1;
  EXPECT_NE(v, one);
  EXPECT_EQ(v, two);
}

TEST(AssignmentAndEquality) {
  auto v = spla::Vector<int>::ones(3);
  auto one = v;
  auto two = spla::Vector<int>::fill(3, 2);
  EXPECT_EQ(v, one);
  EXPECT_NE(v, two);

  v *= 2;
  EXPECT_NE(v, one);
  EXPECT_EQ(v, two);

  // v -= one;
  EXPECT_EQ(v, one);
  EXPECT_NE(v, two);
}

int main() {
  TestSuite::RunAll();
  return 0;
}

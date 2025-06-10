#include "tensor.h"
#include "test.h"

#include <numbers>

TEST(VectorAbs) {
  auto v = spla::Tensor<int, 3>::zeros();
  v[0] = -3;
  v[1] = 4;
  v[2] = -2;

  auto abs_v = abs(v);
  EXPECT_EQ(abs_v[0], 3);
  EXPECT_EQ(abs_v[1], 4);
  EXPECT_EQ(abs_v[2], 2);

  EXPECT_EQ(v[0], -3);
  EXPECT_EQ(v[1], 4);
  EXPECT_EQ(v[2], -2);
}

TEST(VectorAdd) {
  auto v = spla::Tensor<int, 4>::ones();
  v[0] = 0;
  v[2] = 0;
  EXPECT_EQ(v.sparsity(), 2);

  auto w = spla::Tensor<int, 4>::full(2);
  w[0] = 0;
  w[1] = 0;
  EXPECT_EQ(w.sparsity(), 2);

  auto u = v + w;
  EXPECT_EQ(u[0], 0);
  EXPECT_EQ(u[1], 1);
  EXPECT_EQ(u[2], 2);
  EXPECT_EQ(u[3], 3);
  EXPECT_EQ(u.sparsity(), 3);

  u += 2;
  EXPECT_EQ(u[0], 2);
  EXPECT_EQ(u[1], 3);
  EXPECT_EQ(u[2], 4);
  EXPECT_EQ(u[3], 5);
  EXPECT_EQ(u.sparsity(), 3);
}

TEST(VectorAll) {
  auto v = spla::Tensor<int, 3>::ones();
  const auto one = v;
  const auto two = spla::Tensor<int, 3>::full(2);
  EXPECT_TRUE(spla::all(v == one));
  EXPECT_FALSE(spla::all(v == two));

  v *= 2;
  EXPECT_FALSE(spla::all(v == one));
  EXPECT_TRUE(spla::all(v == two));

  v -= one;
  EXPECT_TRUE(spla::all(v == one));
  EXPECT_FALSE(spla::all(v == two));
}

TEST(VectorConversion) {
  const auto one_int = spla::Tensor<int, 3>::ones();
  const auto one_double = spla::Tensor<double, 3>::ones();
  const auto two_double = spla::Tensor<double, 3>::full(2);

  auto v = static_cast<spla::Tensor<double, 3>>(one_int);
  EXPECT_TRUE(spla::all(v == one_double));
  EXPECT_FALSE(spla::all(v == two_double));
}

TEST(VectorDot) {
  const auto one = spla::Tensor<int, 5>::ones();
  EXPECT_EQ(dot(one, one), 5);
}

TEST(VectorEq) {
  const auto zero = spla::Tensor<int, 5>::zeros();
  const auto one = spla::Tensor<int, 5>::ones();
  auto v = spla::Tensor<int, 5>::zeros();

  EXPECT_TRUE(spla::all(zero == v));
  EXPECT_TRUE(!spla::any(zero == one));
  EXPECT_TRUE(!spla::all(v == one));

  v[0] = 1;
  EXPECT_TRUE(spla::any(v == one));
}

TEST(VectorExp) {
  auto v = spla::Tensor<double, 2>::zeros();
  v[1] = 1;

  auto exp_v = exp(v);
  EXPECT_NEAR(exp_v[0], 1, 1e-6);
  EXPECT_NEAR(exp_v[1], std::numbers::e_v<double>, 1e-6);

  EXPECT_EQ(v[0], 0);
  EXPECT_EQ(v[1], 1);
}

TEST(VectorNorm) {
  const auto one = spla::Tensor<double, 3>::ones();
  const auto two = spla::Tensor<double, 3>::full(2);
  const auto three = spla::Tensor<double, 3>::full(3);

  auto v = one;
  EXPECT_NEAR(v[0], 1, 1e-6);
  EXPECT_NEAR(norm(v, 2), sqrt(3), 1e-6);

  auto w = two;
  EXPECT_NEAR(w[0], 2, 1e-6);
  EXPECT_NEAR(norm(w, 2), sqrt(12), 1e-6);

  v = v + w;
  EXPECT_NEAR(v[0], 3, 1e-6);
  EXPECT_NEAR(norm(v, 2), sqrt(27), 1e-6);
  EXPECT_TRUE(spla::all(v == three));
}

TEST(VectorPow) {
  auto v = spla::Tensor<int, 4>::full(2);
  v[0] = 1;
  v[1] = 2;
  v[2] = 3;

  auto v_pow_v = pow(v, v);
  EXPECT_EQ(v_pow_v[0], 1);
  EXPECT_EQ(v_pow_v[1], 4);
  EXPECT_EQ(v_pow_v[2], 27);
  EXPECT_EQ(v_pow_v[3], 4);

  EXPECT_EQ(v[0], 1);
  EXPECT_EQ(v[1], 2);
  EXPECT_EQ(v[2], 3);
  EXPECT_EQ(v[3], 2);
}

TEST(VectorReduce) {
  auto v = spla::Tensor<int, 5>::ones();
  v[2] = 7;
  v[3] = -1;
  EXPECT_EQ(reduce(v, [](int a, int b) { return a + b; }), 9);
}

TEST(VectorSubscript) {
  auto v = spla::Tensor<int, 10>::zeros();
  EXPECT_NEAR(norm(v, 2), 0, 1e-6);
  EXPECT_EQ(v.sparsity(), 0);

  v[1] = -7;
  v[3] = 4;
  v *= 2;
  EXPECT_EQ(v[1], -14);
  EXPECT_EQ(v[3], 8);
  EXPECT_EQ(v.sparsity(), 2);

  auto w = spla::Tensor<int, 10>::zeros();
  w[3] = 2;
  w[5] = 1;
  EXPECT_EQ(w[3], 2);
  EXPECT_EQ(w[5], 1);
  EXPECT_EQ(w.sparsity(), 2);

  v += w;
  EXPECT_EQ(v[1], -14);
  EXPECT_EQ(v[3], 10);
  EXPECT_EQ(v[5], 1);
  EXPECT_EQ(v.sparsity(), 3);

  EXPECT_NEAR(norm(v, 1), 25, 1e-6);
  EXPECT_NEAR(norm(v, 2), sqrt(297), 1e-6);
  EXPECT_EQ(dot(v, w), 21);
}

TEST(MatrixReduce) {
  auto A = spla::Tensor<int, 5, 5>::ones();
  const int sum = reduce(A, [](int a, int b) { return a + b; });
  EXPECT_EQ(sum, 25);
}

TEST(MatrixSubscript) {
  auto A = spla::Tensor<float, 2, 2>::zeros();
  A[0, 0] = 1;
  A[1, 1] = 1;

  auto B = spla::Tensor<float, 2, 2>::ones();
  B += A;
  EXPECT_EQ(B[0, 0], 2);
  EXPECT_EQ(B[0, 1], 1);
  EXPECT_EQ(B[1, 0], 1);
  EXPECT_EQ(B[1, 1], 2);
}

int main() {
  TestSuite::RunAll();
  return 0;
}

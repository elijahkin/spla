#include <cassert>
#include <cmath>
#include <iostream>
#include <unordered_map>

#define EXPECT_OK(expr)                                                        \
  try {                                                                        \
    expr;                                                                      \
  } catch (const std::exception &e) {                                          \
    assert(false);                                                             \
  }

#define EXPECT_NOT_OK(expr)                                                    \
  try {                                                                        \
    expr;                                                                      \
    assert(false);                                                             \
  } catch (const std::exception &e) {                                          \
  }

#define ASSERT_EQ(a, b) assert((a) == (b))

#define ASSERT_ALMOST_EQ(a, b) assert(std::abs((a) - (b)) < 1e-6)

#define TEST(name) static void name()

// Human readable names for ANSI escape codes
const std::string kResetText = "\033[0m";
const std::string kBoldText = "\033[1m";
const std::string kRedText = "\033[31m";
const std::string kGreenText = "\033[32m";

void RunAll(std::unordered_map<std::string, void (*)()> tests) {
  for (const auto &[name, ptr] : tests) {
    std::cout << kBoldText;
    try {
      ptr();
      std::cout << kGreenText << "PASSED: ";
    } catch (const std::exception &e) {
      std::cout << kRedText << "FAILED (" << e.what() << "): ";
    }
    std::cout << kResetText << name << std::endl;
  }
}

#include <cassert>
#include <cmath>
#include <iostream>
#include <unordered_map>

// Defines simple macros for testing in C++, heavily inspired by Google Test.

// Asserts that two values are exactly equal.
#define EXPECT_EQ(val1, val2) assert((val1) == (val2))

// Asserts that the absolute difference of two floating-point values does not
// exceed `abs_err`.
#define EXPECT_NEAR(val1, val2, abs_err)                                       \
  assert(std::abs((val1) - (val2)) < abs_err)

// Asserts that `stmt` does not throw any exceptions.
#define EXPECT_NO_THROW(stmt)                                                  \
  try {                                                                        \
    stmt;                                                                      \
  } catch (const std::exception &e) {                                          \
    assert(false);                                                             \
  }

// Asserts that `stmt` throws an exception of type `exc_type`.
#define EXPECT_THROW(stmt, exc_type)                                           \
  try {                                                                        \
    stmt;                                                                      \
    assert(false);                                                             \
  } catch (const exc_type &e) {                                                \
  }

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
      std::cout << kRedText << "FAILED: ";
    }
    std::cout << kResetText << name << std::endl;
  }
}

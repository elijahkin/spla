#include <cassert>
#include <cmath>
#include <iostream>
#include <unordered_map>

// Defines simple macros for testing in C++, heavily inspired by Google Test.

// Asserts that two values are exactly equal.
#define EXPECT_EQ(val1, val2) assert((val1) == (val2))

// Asserts that two values are not exactly equal.
#define EXPECT_NE(val1, val2) assert((val1) != (val2))

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

// Human readable names for ANSI escape codes
const char kResetText[] = "\033[0m";
const char kBoldText[] = "\033[1m";
const char kRedText[] = "\033[31m";
const char kGreenText[] = "\033[32m";

class TestSuite {
public:
  static void Register(const std::string &name, void (*test_func)()) {
    tests[name] = test_func;
  }

  static void RunAll() {
    for (const auto &[name, ptr] : tests) {
      bool passed = true;
      try {
        ptr();
      } catch (const std::exception &e) {
        passed = false;
      }
      std::cout << std::format("{}{}{}{}: {}", kBoldText,
                               (passed ? kGreenText : kRedText),
                               (passed ? "PASSED" : "FAILED"), kResetText, name)
                << std::endl;
    }
  }

private:
  static inline std::unordered_map<std::string, void (*)()> tests;
};

#define TEST(test_name)                                                        \
  void test_name();                                                            \
  static struct Register_##test_name {                                         \
    Register_##test_name() { TestSuite::Register(#test_name, test_name); }     \
  } register_##test_name;                                                      \
  void test_name()

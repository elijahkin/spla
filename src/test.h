#include "log.h"

#include <iostream>
#include <vector>

// This file is a simple testing framework for C++, taking heavy inspiration
// from Google Test. We first define several macros for making assertions within
// tests, and then the `TestSuite` class itself.

#define EXPECT_TRUE(condition) EXPECT_FALSE(!(condition))

#define EXPECT_FALSE(condition)                                                \
  if (condition)                                                               \
    throw std::runtime_error("EXPECT_FALSE failed");

#define EXPECT_EQ(val1, val2) EXPECT_TRUE((val1) == (val2))

#define EXPECT_NE(val1, val2) EXPECT_FALSE((val1) == (val2))

#define EXPECT_NEAR(val1, val2, abs_err)                                       \
  EXPECT_TRUE(abs((val1) - (val2)) < abs_err)

#define EXPECT_THROW(stmt, exc_type)                                           \
  {                                                                            \
    bool caught = false;                                                       \
    try {                                                                      \
      stmt;                                                                    \
    } catch (const exc_type &e) {                                              \
      caught = true;                                                           \
    }                                                                          \
    EXPECT_TRUE(caught)                                                        \
  }

#define EXPECT_NO_THROW(stmt)                                                  \
  try {                                                                        \
    stmt;                                                                      \
  } catch (const std::exception &e) {                                          \
    EXPECT_FALSE(true)                                                         \
  }

#define TEST(test_name)                                                        \
  void test_name();                                                            \
  static struct Register_##test_name {                                         \
    Register_##test_name() { TestSuite::Register(#test_name, test_name); }     \
  } register_##test_name;                                                      \
  void test_name()

// Manages a collection of tests, allowing them to be registered and run. The
// `RunAll` method executes all registered tests and prints their results to the
// console, indicating whether each test passed or failed.
class TestSuite {
public:
  static void Register(const std::string &name, void (*test_func)()) {
    tests.push_back({name, test_func});
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
  static inline std::vector<std::pair<std::string, void (*)()>> tests;
};

#include "log.h"

#include <vector>

// This file is a simple testing framework for C++, taking heavy inspiration
// from Google Test. We first define several macros for making assertions within
// tests, and then the `TestSuite` class itself.

#define EXPECT_EQ(val1, val2)                                                  \
  if (!((val1) == (val2))) {                                                   \
    throw std::runtime_error("EXPECT_EQ failed");                              \
  }

#define EXPECT_NE(val1, val2)                                                  \
  if (!((val1) != (val2))) {                                                   \
    throw std::runtime_error("EXPECT_NE failed");                              \
  }

#define EXPECT_NEAR(val1, val2, abs_err)                                       \
  if (abs((val1) - (val2)) > abs_err) {                                        \
    throw std::runtime_error("EXPECT_NEAR failed");                            \
  }

#define EXPECT_NO_THROW(stmt)                                                  \
  try {                                                                        \
    stmt;                                                                      \
  } catch (const std::exception &e) {                                          \
    throw std::runtime_error("EXPECT_NO_THROW failed");                        \
  }

#define EXPECT_THROW(stmt, exc_type)                                           \
  {                                                                            \
    bool thrown = false;                                                       \
    try {                                                                      \
      stmt;                                                                    \
    } catch (const exc_type &e) {                                              \
      thrown = true;                                                           \
    }                                                                          \
    if (!thrown) {                                                             \
      throw std::runtime_error("EXPECT_THROW failed");                         \
    }                                                                          \
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

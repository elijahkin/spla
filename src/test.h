#include "log.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// This file is a simple testing framework for C++, taking heavy inspiration
// from Google Test. We first define the `TestSuite` class itself, and then
// several functions for making assertions within tests.

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
    tests_.push_back({name, test_func});
  }

  static void RunAll() {
    for (const auto &[name, ptr] : tests_) {
      bool passed = true;
      auto begin = std::chrono::steady_clock::now();
      try {
        ptr();
      } catch (const std::exception &e) {
        passed = false;
      }
      auto end = std::chrono::steady_clock::now();
      std::cout << std::format(
                       "{:<20.20} {}{}{}{} in {}µs", name, kBoldText,
                       (passed ? kGreenText : kRedText),
                       (passed ? "PASSED" : "FAILED"), kResetText,
                       std::chrono::duration_cast<std::chrono::microseconds>(
                           end - begin)
                           .count())
                << std::endl;
    }
  }

private:
  static inline std::vector<std::pair<std::string, void (*)()>> tests_;
};

inline void EXPECT_TRUE(bool condition) {
  if (!condition)
    throw std::runtime_error("EXPECT_TRUE failed");
}

inline void EXPECT_FALSE(bool condition) { EXPECT_TRUE(!condition); }

// TODO(elijahkin) Can we introduce a Comparable concept to clean these up?

template <typename T1, typename T2> inline void EXPECT_EQ(T1 val1, T2 val2) {
  EXPECT_TRUE(val1 == val2);
}

template <typename T1, typename T2> inline void EXPECT_NE(T1 val1, T2 val2) {
  EXPECT_FALSE(val1 == val2);
}

template <typename T1, typename T2, typename T3>
inline void EXPECT_NEAR(T1 val1, T2 val2, T3 abs_err) {
  EXPECT_TRUE(abs(val1 - val2) < abs_err);
}

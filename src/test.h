#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "log.h"

// This file is a simple testing framework for C++, taking heavy inspiration
// from Google Test. We first define the `TestSuite` class itself, and then
// several functions for making assertions within tests.

#define TEST(name)                      \
  void name();                          \
  static struct Register_##name {       \
    Register_##name() {                 \
      TestSuite::Register(#name, name); \
    }                                   \
  } register_##name;                    \
  void name()

bool passed;

// Manages a collection of tests, allowing them to be registered and run. The
// `RunAll` method executes all registered tests and prints their results to the
// console, indicating whether each test passed or failed.
class TestSuite {
 public:
  static void Register(const std::string& name, void (*func)()) {
    tests_.emplace_back(name, func);
  }

  static void RunAll() {
    for (const auto& [name, func] : tests_) {
      passed = true;
      auto begin = std::chrono::steady_clock::now();
      func();
      auto end = std::chrono::steady_clock::now();
      std::cout << std::format(
          "{:<20.20} {}{}{}{} in {}µs\n", name, kBoldText,
          (passed ? kGreenText : kRedText), (passed ? "PASSED" : "FAILED"),
          kResetText,
          std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
              .count());
    }
  }

 private:
  static inline std::vector<std::pair<std::string, void (*)()>> tests_;
};

inline void EXPECT_TRUE(bool condition) {
  passed &= condition;
}

inline void EXPECT_FALSE(bool condition) {
  passed &= !condition;
}

template <typename T1, typename T2>
inline void EXPECT_EQ(T1 val1, T2 val2) {
  EXPECT_TRUE(val1 == val2);
}

template <typename T1, typename T2>
inline void EXPECT_NE(T1 val1, T2 val2) {
  EXPECT_FALSE(val1 == val2);
}

template <typename T1, typename T2, typename T3>
inline void EXPECT_NEAR(T1 val1, T2 val2, T3 abs_err) {
  EXPECT_TRUE(abs(val1 - val2) < abs_err);
}

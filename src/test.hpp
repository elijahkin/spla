#include <chrono>
#include <print>
#include <string>
#include <utility>
#include <vector>

// Provides a conevient way to register tests into the suite.
#define TEST(name)                                          \
  void name();                                              \
  static struct Register_##name {                           \
    Register_##name() { TestSuite::Register(#name, name); } \
  } register_##name;                                        \
  void name()

static bool passed;

// Manages a collection of tests, allowing them to be registered and run. The
// `RunAll` method executes all registered tests and prints their results to the
// console, indicating whether each test passed or failed.
class TestSuite {
 public:
  static void Register(const std::string &name, void (*func)()) {
    tests_.emplace_back(name, func);
  }

  static void RunAll() {
    for (const auto &[name, func] : tests_) {
      passed = true;
      auto begin = std::chrono::steady_clock::now();
      func();
      auto end = std::chrono::steady_clock::now();
      auto micros =
          std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
              .count();
      // Stylizes the text using ANSI escape codes.
      std::println("{:<20.20} \033[1m{}\033[0m in {}µs", name,
                   (passed ? "\033[32mPASSED" : "\033[31mFAILED"), micros);
    }
  }

 private:
  static inline std::vector<std::pair<std::string, void (*)()>>
      tests_;  // NOLINT
};

// Make assertions within tests, taking heavy inspiration from Google Test.
inline void EXPECT_TRUE(bool condition) { passed &= condition; }

inline void EXPECT_FALSE(bool condition) { passed &= !condition; }

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
  EXPECT_TRUE(std::abs(val1 - val2) < abs_err);
}

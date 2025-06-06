#include <cassert>
#include <cmath>

#define ASSERT_EQ(a, b) assert((a) == (b))

#define ASSERT_CLOSE(a, b) assert(std::abs((a) - (b)) < 1e-6)

#define TEST(name) static void name()

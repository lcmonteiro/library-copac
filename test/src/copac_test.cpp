#include <gtest/gtest.h>

#include "copac.hpp"

using namespace copac;

/// Test CodecEnvironment
TEST(copac, overview_test) {
    auto a = var(1);
    auto b = var(1);
    auto c = a + b;

    std::cout << c << std::endl;
}

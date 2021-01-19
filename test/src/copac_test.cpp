#include <gtest/gtest.h>

#include "copac.hpp"
#include "copac/math.hpp"
#include "copac/stream.hpp"


using namespace copac;

/// Test CodecEnvironment
TEST(copac, overview_test) {
    auto a = var{
        var{
            std::pair{1, var(1)},
            std::pair{2, var(1)},
        },
        var{
            std::pair{1, var(1)},
            std::pair{2, var(1)},
        }
    };
    auto b = var(1) + var(2);

    std::cout << a << std::endl;
    std::cout << b << std::endl;
}

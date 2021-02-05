#include <regex>
#include <gtest/gtest.h>

#include "copac.hpp"
#include "copac/math.hpp"
#include "copac/stream.hpp"


using namespace copac;

/// Test CodecEnvironment
TEST(copac_stream, overview_test) {

    auto in = std::string("[[{\"a\":111,\"b\":2},{\"a\":111,\"b\":2}],[{\"a\":111,\"b\":2},{\"a\":111,\"b\":2}]]");
    auto is = std::istringstream(in);
    auto os = std::ostringstream();

    var v;
    is >> v;
    os << v;

    EXPECT_EQ(in, os.str());
}

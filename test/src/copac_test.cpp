#include <regex>
#include <gtest/gtest.h>

#include "copac.hpp"
#include "copac/math.hpp"
#include "copac/stream.hpp"
#include "copac/pipe.hpp"


using namespace copac;

/// Test CodecEnvironment
TEST(copac, overview_test) {
    

    var a = {{
        {1, 1},
        {2, 1},
    },{
        std::pair{1, var(1)},
        std::pair{2, var(1)},
    }};

    //auto b = var::link(soft(a)); 
    //a | pipe::find("1") = 1;
    a | pipe::find("1/1") = 10;
    a | pipe::find("1/2") | pipe::push(1) | pipe::push(10);

    std::cout << std::setfill(' ') << a << std::endl;
    //std::cout << b << std::endl;
    // a >> edit::filter() >> edit::foreach([](auto v){
    //     v >> edit::find("test.1") >> edit::asign(1);
    // }) >> ;  
    //std::map<int, int> map;
    //std::cout << map[1] << map.size() << std::endl;
    //std::cout << a << std::endl;
    //std::cout << b << std::endl;
}

/// ===============================================================================================
/// @file      : copac_operator.hpp                                 __|   _ \   __ \    _` |   __|
/// @copyright : 2019 LCMonteiro                                   (     (   |  |   |  (   |  (
///                                                               \___| \___/   .__/  \__,_| \___|
/// @author    : Luis Monteiro                                                 _|
/// ===============================================================================================

#pragma once

#include <iostream>

namespace copac {

    template<typename...Ts>
    var<Ts...> operator+(const var<Ts...>& a, const var<Ts...>& b) {
        using integer_t  = typename var<Ts...>::integer_t;
        using floating_t = typename var<Ts...>::floating_t;
        return var<Ts...>::visit(select {
            [](auto       a, auto b){ return var();  },
            [](integer_t  a, auto b){ return var(a + cast::to<integer_t>(b  ));},
            [](floating_t a, auto b){ return var(a + cast::to<floating_t>(b));}
        } , a, b);
    }

    template<typename...Ts>
    std::ostream& operator<<(std::ostream &os, const var<Ts...>& a) {
        using integer_t  = typename var<Ts...>::integer_t;
        using floating_t = typename var<Ts...>::floating_t;
        var<Ts...>::visit(select {
            [&os](auto       a){   },
            [&os](integer_t  a){ os << a;},
            [&os](floating_t a){ os << a;}
        }, a);
        return os;
    }
}

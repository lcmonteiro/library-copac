/// ===============================================================================================
/// @file      : copac/math.hpp                                     __|   _ \   __ \    _` |   __|
/// @copyright : 2019 LCMonteiro                                   (     (   |  |   |  (   |  (
///                                                               \___| \___/   .__/  \__,_| \___|
/// @author    : Luis Monteiro                                                 _|
/// ===============================================================================================

#pragma once

#include <iostream>

#include "variable.hpp"

namespace copac {

    template<typename...Ts>
    var<Ts...> operator+(const var<Ts...>& a, const var<Ts...>& b) {
        using integer_t  = typename var<Ts...>::integer_t;
        using floating_t = typename var<Ts...>::floating_t;
        using string_t   = typename var<Ts...>::string_t;
        return var<Ts...>::visit(select {
            [](const auto&       a, const auto& b){return var(cast<string_t>(a) + cast<string_t>(b));},
            [](const string_t&   a, const auto& b){return var(a + cast<string_t>(b));},
            [](const integer_t&  a, const auto& b){return var(a + cast<integer_t>(b));},
            [](const floating_t& a, const auto& b){return var(a + cast<floating_t>(b));}
        } , a, b);
    }


}

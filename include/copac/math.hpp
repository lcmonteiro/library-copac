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
    basic_var<Ts...> operator+(const basic_var<Ts...>& a, const basic_var<Ts...>& b){
        using integer_t  = typename basic_var<Ts...>::integer_t;
        using floating_t = typename basic_var<Ts...>::floating_t;
        using string_t   = typename basic_var<Ts...>::string_t;

        return basic_var<Ts...>::visit(select{
            [](const auto& a, const auto& b){
                return basic_var<Ts...>(cast<string_t>(a) + cast<string_t>(b)); },
            [](const string_t& a, const auto& b){
                return basic_var<Ts...>(a + cast<string_t>(b)); },
            [](const integer_t& a, const auto& b){
                return basic_var<Ts...>(a + cast<integer_t>(b)); },
            [](const floating_t& a, const auto& b){
                return basic_var<Ts...>(a + cast<floating_t>(b)); }
        } , a, b);
    }


}

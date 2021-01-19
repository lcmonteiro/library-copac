/// ===============================================================================================
/// @file      : copac/stream.hpp                                   __|   _ \   __ \    _` |   __|
/// @copyright : 2019 LCMonteiro                                   (     (   |  |   |  (   |  (
///                                                               \___| \___/   .__/  \__,_| \___|
/// @author    : Luis Monteiro                                                 _|
/// ===============================================================================================

#pragma once

#include <iostream>

#include "variable.hpp" 

namespace copac {

    template<typename...Ts>
    std::ostream& operator<<(std::ostream &os, const var<Ts...>& a) {
        using map_t    = typename var<Ts...>::map_t;
        using list_t   = typename var<Ts...>::list_t;
        using buffer_t = typename var<Ts...>::buffer_t;
        using string_t = typename var<Ts...>::string_t;
        var<Ts...>::visit(select {
            [&os](const auto&     a){ os << a; },
            [&os](const map_t&    a){
                for(auto&[k, v]: a) 
                    os << k << ":" << v << std::endl;
            },
            [&os](const list_t&   a){
                for(auto& v: a) 
                    os << "-" << v;
            },
            [&os](const buffer_t& a){ }
        }, a);
        return os;
    }
}

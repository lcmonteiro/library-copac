/// ===============================================================================================
/// @file      : copac/math.hpp                                     __|   _ \   __ \    _` |   __|
/// @copyright : 2019 LCMonteiro                                   (     (   |  |   |  (   |  (
///                                                               \___| \___/   .__/  \__,_| \___|
/// @author    : Luis Monteiro                                                 _|
/// ===============================================================================================

#pragma once

#include <iostream>
#include <numeric>

#include "variable.hpp"

namespace copac::pipe {

    /// find operation
    /// @brief
    /// find returns a reference to a var by given a path (path/elem)      
    struct find {
        find(std::vector<std::string> path): path_{std::move(path)}{}
        find(std::string path) {
            auto iss = std::istringstream(std::move(path));
            auto key = std::string();
            while (std::getline(iss, key, '/'))
                path_.emplace_back(std::move(key));
        }
        template<typename...Ts>
        friend basic_var<Ts...>& operator|(basic_var<Ts...>& v, const find& f) {
            using map_t  = typename basic_var<Ts...>::map_t;
            using list_t = typename basic_var<Ts...>::list_t;     
            
            return std::accumulate(std::cbegin(f.path_), std::cend(f.path_), std::ref(v), [](auto v , auto k){
                return basic_var<Ts...>::visit(copac::select{ 
                    [&v](auto&   a) { throw  std::out_of_range("find: wrong path"); return v; },
                    [&k](map_t&  m) { return std::ref(m[k]); },
                    [&k](list_t& l) { return std::ref(l.at(cast<size_t>(k))); }
                }, v.get());
             }).get();
        }
    private:
        std::vector<std::string> path_;
    };
    

}

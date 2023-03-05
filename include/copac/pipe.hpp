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
    /// path
    /// @brief
    /// find returns a reference to a var by given a path (path/elem)     
    struct path : std::vector<std::variant<std::string_view, size_t>> {
        path(std::initializer_list<std::variant<std::string_view, size_t>> l){
            for(const auto& v : l)
                emplace_back(std::move(v));
        }
        path(std::string p) {
            auto iss = std::istringstream(std::move(p));
            auto key = std::string();
            while (std::getline(iss, key, '/'))
                emplace_back(std::move(key));
        }
    };

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
            using var_t  = basic_var<Ts...>;
            using map_t  = typename var_t::map_t;
            using list_t = typename var_t::list_t;
            using link_t = typename var_t::link_t;
            using key_t  = typename map_t::key_type;     
            
            return std::accumulate(std::cbegin(f.path_), std::cend(f.path_), std::ref(v), [](auto v , auto k){
                return basic_var<Ts...>::visit(copac::select{ 
                    [&v](auto&   a) { throw  std::out_of_range("find: wrong path"); return v; },
                    [&k](map_t&  m) { return std::ref(m[cast<key_t>(k)]); },
                    [&k](list_t& l) { return std::ref(l.at(cast<size_t>(k))); }
                }, v.get());
             }).get();
        }
    private:
        std::vector<std::string> path_;
    };

    /// push operation
    /// @brief  
    template<typename Type>
    struct push {
        push(Type&& value): value_{std::forward<Type>(value)}{}
        template<typename...Ts>
        friend basic_var<Ts...>& operator|(basic_var<Ts...>& v, const push& f) {
            using link_t = typename basic_var<Ts...>::link_t;
            using list_t = typename basic_var<Ts...>::list_t; 

            basic_var<Ts...>::visit(copac::select{ 
                [&](auto&   a) { v = basic_var<Ts...>{a, std::move(f.value_)}; },
                [&](link_t& l) { 
                    v = std::visit(copac::select{
                            [](std::shared_ptr<basic_var<Ts...>>& p) { return !p; },
                            [](std::weak_ptr<basic_var<Ts...>>&   p) { return p.expired();}}, l)
                        ? basic_var<Ts...>{std::move(f.value_)}
                        : basic_var<Ts...>{l, std::move(f.value_)};
                },
                [&](list_t& l) { l.emplace_back(std::move(f.value_)); }
            }, v);
            return v;
        }
    private:
        mutable Type value_;
    };

}

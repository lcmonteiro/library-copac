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
    /// ===========================================================================================
    /// serialize
    /// ===========================================================================================
    template<typename...Ts>
    std::ostream& operator<<(std::ostream &os, const basic_var<Ts...>& a) {
        using map_t       = typename basic_var<Ts...>::map_t;
        using list_t      = typename basic_var<Ts...>::list_t;
        using string_t    = typename basic_var<Ts...>::string_t;
        using buffer_t    = typename basic_var<Ts...>::buffer_t;
        using integer_t   = typename basic_var<Ts...>::integer_t;
        using floating_t  = typename basic_var<Ts...>::floating_t;
        using link_t      = typename basic_var<Ts...>::link_t;
        
        basic_var<Ts...>::visit(select{
            [&os](const map_t& a){
                os << '{';
                if(auto it = std::begin(a); it != std::end(a)) {
                    os << '"' << std::get<0>(*it) << '"'; 
                    os << ':' << std::get<1>(*it);
                    for(++it; it != std::end(a); ++it){  
                        os << ',' << '"' << std::get<0>(*it); 
                        os << '"' << ':' << std::get<1>(*it);
                    }
                }
                os << '}';
            },
            [&os](const list_t& a){
                os << '[';
                if(auto it = std::begin(a); it != std::end(a)) {
                    os << *it;
                    for(++it; it != std::end(a); ++it) 
                        os <<',' << *it;
                }
                os << ']';
            },
            [&os](const string_t&   a){ 
                os << '"';
                os << std::regex_replace(a, std::regex("\n"), "\\n"); 
                os << '"'; 
            },
            [&os](const buffer_t&   a){ 
                os << '"';
                os << cast<string_t>(a); 
                os << '"';
            },
            [&os](const integer_t&   a){ os << cast<string_t>(a); },
            [&os](const floating_t&  a){ os << cast<string_t>(a); },
            [&os](const link_t& a){  }
        }, a);
        return os;
    }

    /// ===========================================================================================
    /// unserialize
    /// ===========================================================================================
    namespace helper {

        static bool match(char ch, const std::string& del) {
            return std::find(std::begin(del), std::end(del), ch) != std::end(del);
        }
        static bool match(char ch, const std::initializer_list<char>& del) {
            return std::find(std::begin(del), std::end(del), ch) != std::end(del);
        }

        static char read_some(std::istream& is, std::string& str) {
            while (is.good()) {
                switch (auto opt = is.get(); opt) {
                    case '['://str.push_back(opt); return opt;
                    case ']'://str.push_back(opt); return opt;
                    case '{'://str.push_back(opt); return opt;
                    case '}'://str.push_back(opt); return opt;
                    case '"'://str.push_back(opt); return opt;
                    case ','://str.push_back(opt); return opt;
                    case ':'://str.push_back(opt); return opt;
                    case 'n'://str.push_back(opt); return opt;
                    case '\'': str.push_back(opt); return opt;
                    case '0'://str.push_back(opt); return '0'; 
                    case '1'://str.push_back(opt); return '0';
                    case '2'://str.push_back(opt); return '0';
                    case '3'://str.push_back(opt); return '0';
                    case '4'://str.push_back(opt); return '0';
                    case '5'://str.push_back(opt); return '0';
                    case '6'://str.push_back(opt); return '0';
                    case '7'://str.push_back(opt); return '0';
                    case '8'://str.push_back(opt); return '0';
                    case '9':  str.push_back(opt); return '0';
                    default :  str.push_back(opt);
                }
            }
            return 0;
        }

        static char read_until(std::istream& is, std::string& str, std::initializer_list<char> del) {
            auto ch = read_some(is, str);
            while(!match(ch, del)) 
                ch = read_some(is, str);
            return ch;
        }

        static std::string& clear (std::string& str) { str.clear();    return str; }
        static std::string& pop   (std::string& str) { str.pop_back(); return str; }
        static std::string& squash(std::string& str) {
            auto ch = str.back();
            str.clear();
            str.push_back(ch);
            return str;
        }
    }

    template<typename...Ts>
    std::istream& operator>>(std::istream &is, basic_var<Ts...>& v) {    
        using map_t      = typename basic_var<Ts...>::map_t;
        using list_t     = typename basic_var<Ts...>::list_t;
        using integer_t  = typename basic_var<Ts...>::integer_t;
        using floating_t = typename basic_var<Ts...>::floating_t;

        auto cache = std::string();
        auto parse = std::function<char()>([&]() {
            auto o = helper::read_some(is, helper::clear(cache));
            switch (o) {
                case '[': {
                    auto lst = list_t{};
                    for(o = parse(); helper::match(o, {']', ','}); o = parse()){
                        lst.emplace_back(std::move(v));
                        if(o == ']') {
                            v = std::move(lst);
                            return parse();
                        }
                    }
                    throw std::runtime_error("parse error (list)");
                }
                case '{': {
                    auto map = map_t{};
                    for(o = parse(); helper::match(o, {':'}); o = parse()) {
                        basic_var<Ts...>::visit([&](auto k) {
                            if(o = parse(); !helper::match(o, {'}', ','}))
                                throw std::runtime_error("parse error (map)");
                            map.emplace(cast<typename map_t::key_type>(k), std::move(v));
                        }, v);
                        if(o == '}') {
                            v = std::move(map);
                            return parse();
                        }
                    }                 
                    throw std::runtime_error("parse error (map)");
                }  
                case '"':
                case '\'': {
                    o = helper::read_until(is, helper::clear(cache), {o});
                    v = std::move(helper::pop(cache));
                    return parse();
                }
                case 'n': {
                    o = helper::read_some(is, helper::squash(cache));
                    break;
                }
                case '0': {
                    o = helper::read_until(is, helper::squash(cache), {']', '}', ','});
                    if(helper::match('.', cache))
                        v = cast<floating_t>(helper::pop(cache));
                    else
                        v = cast<integer_t>(helper::pop(cache));
                    break;
                }
            }
            return o;
        });   
        parse();
        return is;
    }
}

/// ===============================================================================================
/// @file      : copac/cast.hpp                                     __|   _ \   __ \    _` |   __|
/// @copyright : 2019 LCMonteiro                                   (     (   |  |   |  (   |  (
///                                                               \___| \___/   .__/  \__,_| \___|
/// @author    : Luis Monteiro                                                 _|
/// ===============================================================================================

#pragma once

#include <charconv>
#include <string>
#include <stdexcept>
#include <type_traits>

/// ===============================================================================================
/// cast
/// @brief
/// standards types converters
/// ===============================================================================================
namespace copac{    
    struct cast_error: std::runtime_error{
        using std::runtime_error::runtime_error;
    };

    namespace detail{
        template<typename From, typename To, typename = void>
        struct cast{
            template<typename F=From, typename T=To>
            static constexpr std::enable_if_t<!std::is_convertible_v<F, T>, To>
            value(const From& val){ throw cast_error(
                std::string(typeid(F).name()) + " --> " + typeid(T).name()); 
            }

            template<typename F=From, typename T=To>
            static constexpr std::enable_if_t<std::is_convertible_v<F, T>, To>
            value(const From& val){ return val; }
        }; 

        template<typename T>
        struct cast<T, T>{
            static constexpr const T& value(const T& val){ return val; }
            static constexpr       T& value(      T& val){ return val; }
        }; 

        template<typename To>
        struct cast<std::shared_ptr<To>, To>{
            static constexpr To& value(const std::shared_ptr<To>& ptr){ return *ptr; }
        };

        template<typename To>
        struct cast<std::unique_ptr<To>, To>{
            static constexpr To& value(const std::unique_ptr<To>& ptr){ return *ptr; }
        };

        template<typename To>
        struct cast<std::weak_ptr<To>, To>{
            static constexpr To& value(const std::weak_ptr<To>& ptr){
                if(auto p = ptr.lock())
                    return *p;
                throw cast_error("obj not found");
            }
        };

        template<typename... Ts, typename To>
        struct cast<std::variant<Ts...>, To>{
            static constexpr To value(const std::variant<Ts...>& var){
                return std::visit([](const auto& v){ 
                    return cast<decltype(v), std::decay_t<To>>::value(v); 
                }, var);
            }
        };

        template<typename From>
        struct cast<From, std::string, std::enable_if_t<
            std::is_arithmetic_v<From>>>{
            static std::string value(const From& val){ 
                return std::to_string(val); 
            }
        };

        template<typename From, typename To>
        struct cast<From, To, std::enable_if_t<
            std::is_convertible_v<From, std::string_view> &&
            std::is_arithmetic_v<To>>>{
            static To value(const std::string_view& val){
                To output{};
                std::from_chars(std::cbegin(val), std::end(val), output);
                return output;
            }
        }; 
    } 

    template<typename To, typename From>
    To cast(const From& val) {
        return detail::cast<From, std::decay_t<To>>::value(val);
    }      
    template<typename To, typename From>
    To cast(From& val) {
        return detail::cast<From, std::decay_t<To>>::value(val);
    }      
}

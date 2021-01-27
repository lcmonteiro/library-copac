/// ===============================================================================================
/// @file      : copac/variable.hpp                                 __|   _ \   __ \    _` |   __|
/// @copyright : 2019 LCMonteiro                                   (     (   |  |   |  (   |  (
///                                                               \___| \___/   .__/  \__,_| \___|
/// @author    : Luis Monteiro                                                 _|
/// ===============================================================================================

#pragma once

#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace copac {

    /// ===========================================================================================
    /// Helpers
    /// ===========================================================================================
    /// helper for the visitor
    template<typename... Ts> struct select : Ts... { using Ts::operator()...; };
    template<typename... Ts> select(Ts...) -> select<Ts...>;

    // helper for detecting
    template< typename, typename = void >
    struct detected : std::false_type { };
    template< typename T >
    struct detected<T, std::void_t<T>> : std::true_type { };
    template< class T >
    inline constexpr bool detected_v = detected<T>::value;


    /// ===========================================================================================
    /// Type Concepts
    /// ===========================================================================================
    namespace concepts {
        template <template <typename ...> typename Base, typename Key>
        struct map {
            // requirements
            static_assert(detected_v<decltype(Base<Key,Key>().emplace(Key(), Key()))>);
            
            template <typename Connector>
            using type = Base<Key, Connector>;
        };
        template <template <typename, typename ...> typename Base>
        struct list {
            // requirements
            static_assert(detected_v<decltype(Base<int>().emplace_back(int()))>);

            template <typename Connector>
            using type = Base<Connector>;
        };
        template <typename Base>
        struct string {
            using type = Base;
        };
        template <typename Base>
        struct buffer {
            using type = Base;
        };
        template <typename Base>
        struct integer {
            using type = Base;
        };
        template <typename Base>
        struct floating {
            using type = Base;
        };
    }

    /// ===========================================================================================
    /// basic_var
    /// @brief
    /// ===========================================================================================
    template <
        typename Map      = concepts::map<std::map, std::string>,
        typename List     = concepts::list<std::vector>,
        typename String   = concepts::string<std::string>,
        typename Buffer   = concepts::buffer<std::vector<uint8_t>>,
        typename Integer  = concepts::integer<int>,
        typename Floating = concepts::floating<double>>
    class basic_var {
        template <typename Type>
        using connector_t = std::shared_ptr<Type>;

        using container_t = std::variant<
            typename Map::template  type<basic_var>,
            typename List::template type<basic_var>,
            typename String::type,
            typename Integer::type,
            typename Floating::type>;
        struct object : public container_t {
            using container_t::container_t;
            using container_t::operator=;
            template <typename Callable, typename...Objs>
            static constexpr decltype(auto) visit(Callable&& fn, Objs&&... objs){
                return std::visit(
                    std::forward<Callable>(fn), 
                    static_cast<const container_t&>(objs) ...);
            }
        };

    public:
        /// types
        using map_t      = typename Map::template  type<basic_var>;
        using list_t     = typename List::template type<basic_var>;
        using string_t   = typename String::type;
        using buffer_t   = typename Buffer::type;
        using integer_t  = typename Integer::type;
        using floating_t = typename Floating::type;

        /// constructors
        template <typename... Args>
        constexpr basic_var(Args... args): obj_(std::forward<Args>(args)...) {}

        template<typename Key>
        constexpr basic_var(std::initializer_list<std::pair<Key,basic_var>> l): obj_([&]{
              auto map = map_t();
              for(auto&[k, v] : l) 
                map.emplace(cast<typename map_t::key_type>(k), std::move(v));
              return map;
          }()){}

        constexpr basic_var(std::initializer_list<basic_var> l): obj_([&]{
              auto lst = list_t();
              for(auto& v : l) 
                lst.emplace_back(std::move(v));
              return lst;
          }()){}

        /// visit 
        template <typename Callable, typename...Vars>
        static constexpr auto visit(Callable&& fn, Vars&&... vs){
            return object::visit(std::forward<Callable>(fn), vs.obj_... ); 
        }

    private:
        object obj_;
    };

    // default variable type
    typedef basic_var<> var;   
}

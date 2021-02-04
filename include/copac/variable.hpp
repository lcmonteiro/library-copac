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
    /// decoratores
    /// @brief
    /// ===========================================================================================
    template <typename Type>
    struct hard:std::reference_wrapper<Type> {
        hard(Type& a):std::reference_wrapper<Type>(a){}
    };
    template <typename Type>
    struct soft:std::reference_wrapper<Type> {
        soft(Type& a):std::reference_wrapper<Type>(a){}
    };
    
    /// ===========================================================================================
    /// basic_var
    /// @brief
    /// ===========================================================================================
    template <
        typename Map      , // = concepts::map<map_t, key_t>,
        typename List     , // = concepts::list<list_t>,
        typename String   , // = concepts::string<string_t>,
        typename Buffer   , // = concepts::buffer<buufer_t>,
        typename Integer  , // = concepts::integer<int_t>,
        typename Floating   // = concepts::floating<float_t>
    >
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
        constexpr basic_var(Args... args): conn_(std::forward<Args>(args)...) {}

        template<typename Key>
        constexpr basic_var(std::initializer_list<std::pair<Key,basic_var>> l): conn_([&] {
              auto map = map_t();
              for(auto&[k, v] : l) 
                map.emplace(cast<typename map_t::key_type>(k), std::move(v));
              return map;
          }()){}

        constexpr basic_var(std::initializer_list<basic_var> l): conn_([&] {
              auto lst = list_t();
              for(auto& v : l) 
                lst.emplace_back(std::move(v));
              return lst;
          }()){}

        /// visit 
        template <typename Callable, typename...Vars>
        static constexpr auto visit(Callable&& fn, Vars&&... vs) {
            return std::visit([&](auto&&...c){ 
                return object::visit(std::forward<Callable>(fn), cast<object>(c)... ); 
            }, vs.conn_ ...);
        }

        /// links
        static constexpr auto link(soft<basic_var> a) {
            return std::visit(select{
                [&](object& o) { 
                    auto conn     = std::make_shared<object>(std::move(o));   
                    a.get().conn_ = conn;
                    return basic_var(std::weak_ptr(conn)); 
                },
                [](std::shared_ptr<object>& o) { return basic_var(std::weak_ptr(o)); },
                [](std::weak_ptr<object>&   o) { return basic_var(o); }
            }, a.get().conn_);
        }

        static constexpr auto link(hard<basic_var> a) {
            return std::visit(select{
                [&](object& o) { 
                    auto conn     = std::make_shared<object>(std::move(o));   
                    a.get().conn_ = conn;
                    return basic_var(conn); 
                },
                [](std::shared_ptr<object>& o) { return basic_var(o); },
                [](std::weak_ptr<object>&   o) { return basic_var(o.lock()); } 
            }, a.get().conn_);
        }

    private:
        std::variant<object, std::shared_ptr<object>, std::weak_ptr<object>> conn_;
    };

    /// default variable type
    typedef basic_var<
        concepts::map<std::map, std::string>,
        concepts::list<std::vector>,
        concepts::string<std::string>,
        concepts::buffer<std::vector<uint8_t>>,
        concepts::integer<int>,
        concepts::floating<double>> var;   
}
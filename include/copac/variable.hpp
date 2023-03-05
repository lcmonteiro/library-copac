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

        template <typename Base>
        struct boolean {
            using type = Base;
        };
    }

    /// ===========================================================================================
    /// Decoratores
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
        typename Boolean  , // = concepts::boolean<boolean_t>
        typename Integer  , // = concepts::integer<int_t>,
        typename Floating   // = concepts::floating<float_t>
    >
    class basic_var{
    public:
        /// types
        using link_t      = std::variant<std::shared_ptr<basic_var>, std::weak_ptr<basic_var>>;
        using map_t       = typename Map::template  type<basic_var>;
        using list_t      = typename List::template type<basic_var>;
        using string_t    = typename String::type;
        using buffer_t    = typename Buffer::type;
        using boolean_t   = typename Boolean::type;
        using integer_t   = typename Integer::type;
        using floating_t  = typename Floating::type;
    public:
        /// constructors
        template <typename... Args>
        constexpr basic_var(Args... args): obj_(std::forward<Args>(args)...){}

        template<typename Key>
        constexpr basic_var(std::initializer_list<std::pair<Key, basic_var>> l): obj_([&]{
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
        static constexpr auto visit(Callable&& fn, const Vars&... vs){
            return object::visit(std::forward<Callable>(fn), vs.obj_ ... );
        }
        template <typename Callable, typename...Vars>
        static auto visit(Callable&& fn, Vars&... vs){
            return object::visit(std::forward<Callable>(fn), vs.obj_ ... );
        }

        /// links
        static constexpr auto link(soft<basic_var> a){
            return visit(select{
                [&](const auto&){
                    auto conn = std::make_shared<basic_var>(std::move(a.get()));
                    a.get()   = conn;
                    return basic_var{std::weak_ptr{conn}};
                },
                [&](const link_t& l){
                    return std::visit([](auto& p){ 
                        return basic_var{std::weak_ptr(p)}; 
                    }, l);
                }
            }, a.get());
        }
        static constexpr auto link(hard<basic_var> a){
            return visit(select{
                [&](const auto&){
                    auto conn = std::make_shared<basic_var>(std::move(a.get()));
                    a.get()   = conn;
                    return basic_var{conn};
                },
                [&](const link_t& l){
                    return std::visit(select{
                        [](std::shared_ptr<basic_var>& p) { return basic_var{p}; }, 
                        [](std::shared_ptr<basic_var>& p) { return basic_var{p.lock()}; } 
                    }, l);
                }
            }, a.get());
        }

    private:
        /// internal container to handle all type of objects
        using container_t = std::variant<
            link_t, map_t, list_t, string_t, buffer_t, boolean_t, integer_t, floating_t>;
        struct object : public container_t {
            using container_t::container_t;
            using container_t::operator=;
            template <typename Callable, typename...Objs>
            static constexpr auto visit(Callable&& fn, Objs&... objs){
                return std::visit(
                    std::forward<Callable>(fn),
                    static_cast<container_t&>(objs) ...);
            }
            template <typename Callable, typename...Objs>
            static constexpr auto visit(Callable&& fn, const Objs&... objs){
                return std::visit(
                    std::forward<Callable>(fn), 
                    static_cast<const container_t&>(objs) ...);
            }
        } obj_;
    };

    /// default variable type
    typedef basic_var<
        concepts::map<std::map, std::string>,
        concepts::list<std::vector>,
        concepts::string<std::string>,
        concepts::buffer<std::vector<uint8_t>>,
        concepts::boolean<bool>,
        concepts::integer<int>,
        concepts::floating<double>> var;
}

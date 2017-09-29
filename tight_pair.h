/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Morwenn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef CRUFT_TIGHT_PAIR_H_
#define CRUFT_TIGHT_PAIR_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <tuple>
#include <type_traits>
#include <utility>

namespace cruft
{
    ////////////////////////////////////////////////////////////
    // Forward declarations of main components

    template<typename T1, typename T2>
    struct tight_pair;

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2>&) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>>&;

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2> const&) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>> const&;

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2>&&) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>>&&;

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2> const&&) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>> const&&;
}

namespace std
{
    ////////////////////////////////////////////////////////////
    // Specializations needed for decomposition declarations

    template<typename T, typename U>
    struct tuple_size<cruft::tight_pair<T, U>>:
        std::integral_constant<std::size_t, 2>
    {};

    template<typename T, typename U>
    struct tuple_element<0, cruft::tight_pair<T, U>>
    {
        using type = T;
    };

    template<typename T, typename U>
    struct tuple_element<1, cruft::tight_pair<T, U>>
    {
        using type = U;
    };
}

namespace cruft
{
    namespace detail
    {
        ////////////////////////////////////////////////////////////
        // Whether a type can benefit from the empty base class
        // optimization

        template<typename T>
        struct is_ebco_eligible:
            std::conjunction<
                std::is_empty<T>,
                std::negation<std::is_final<T>>
            >
        {};

        template<typename T>
        constexpr bool is_ebco_eligible_v = is_ebco_eligible<T>::value;

        ////////////////////////////////////////////////////////////
        // Bits from libc++ <__tuple> header and more

        struct nat
        {
            nat() = delete;
            nat(const nat&) = delete;
            nat& operator=(const nat&) = delete;
            ~nat() = delete;
        };

        struct check_tuple_constructor_fail
        {
            template<typename...>
            static constexpr bool enable_default = false;

            template<typename...>
            static constexpr bool enable_explicit = false;

            template<typename...>
            static constexpr bool enable_implicit = false;

            template<typename...>
            static constexpr bool enable_assign = false;
        };

        template<typename T> struct pair_like: std::false_type {};
        template<typename T> struct pair_like<const T>: pair_like<T> {};
        template<typename T> struct pair_like<volatile T>: pair_like<T> {};
        template<typename T> struct pair_like<const volatile T>: pair_like<T> {};

        template<typename T1, typename T2> struct pair_like<std::pair<T1, T2>>: std::true_type {};
        template<typename T1, typename T2> struct pair_like<std::tuple<T1, T2>>: std::true_type {};
        template<typename T> struct pair_like<std::array<T, 2>>: std::true_type {};

        ////////////////////////////////////////////////////////////
        // Type used for the storage of a single pair member: the
        // template integer parameter is used to disambiguate the
        // types when both members the same underlying types

        template<std::size_t N, typename T, bool=is_ebco_eligible_v<T>>
        struct tight_pair_element;

        template<std::size_t N, typename T>
        struct tight_pair_element<N, T, false>
        {
            private:

                T value;

            public:

                ////////////////////////////////////////////////////////////
                // Construction

                constexpr tight_pair_element()
                    noexcept(std::is_nothrow_default_constructible<T>::value):
                    value()
                {
                    static_assert(not std::is_reference<T>::value,
                                  "attempted to default construct a reference element");
                }

                template<typename U>
                constexpr explicit tight_pair_element(U&& other)
                    noexcept(std::is_nothrow_constructible<T, U>::value):
                    value(std::forward<U>(other))
                {}

                template<typename... Args>
                constexpr tight_pair_element(std::piecewise_construct_t,
                                             std::tuple<Args...> args):
                    value(std::make_from_tuple<T>(args))
                {}

                constexpr tight_pair_element(tight_pair_element const&) = default;
                constexpr tight_pair_element(tight_pair_element&&) = default;

                template<typename U>
                constexpr auto operator=(U&& other)
                    noexcept(std::is_nothrow_assignable<T&, U>::value)
                    -> tight_pair_element&
                {
                    value = std::forward<U>(other);
                    return *this;
                }

                tight_pair_element& operator=(tight_pair_element const&) = default;

                ////////////////////////////////////////////////////////////
                // Element access

                constexpr auto get() & noexcept
                    -> T&
                {
                    return static_cast<T&>(value);
                }

                constexpr auto get() const& noexcept
                    -> T const&
                {
                    return static_cast<T const&>(value);
                }

                constexpr auto get() && noexcept
                    -> T&&
                {
                    return static_cast<T&&>(value);
                }

                constexpr auto get() const&& noexcept
                    -> T const&&
                {
                    return static_cast<T const&&>(value);
                }
        };

        template<std::size_t N, typename T>
        struct tight_pair_element<N, T, true>:
            private T
        {
            tight_pair_element& operator=(tight_pair_element const&) = delete;

            ////////////////////////////////////////////////////////////
            // Construction

            constexpr tight_pair_element()
                noexcept(std::is_nothrow_default_constructible<T>::value)
            {}

            template<typename U>
            constexpr explicit tight_pair_element(U&& other)
                noexcept(std::is_nothrow_constructible<T, U>::value):
                T(std::forward<U>(other))
            {}

            template<typename... Args>
            constexpr tight_pair_element(std::piecewise_construct_t,
                                         std::tuple<Args...> args):
                T(std::make_from_tuple<T>(args))
            {}

            tight_pair_element(tight_pair_element const&) = default;
            tight_pair_element(tight_pair_element &&) = default;

            template<typename U>
            auto operator=(U&& other)
                noexcept(std::is_nothrow_assignable<T&, U>::value)
                -> tight_pair_element&
            {
                T::operator=(std::forward<U>(other));
                return *this;
            }

            ////////////////////////////////////////////////////////////
            // Element access

            constexpr auto get() & noexcept
                -> T&
            {
                return static_cast<T&>(*this);
            }

            constexpr auto get() const& noexcept
                -> T const&
            {
                return static_cast<T const&>(*this);
            }

            constexpr auto get() && noexcept
                -> T&&
            {
                return static_cast<T&&>(*this);
            }

            constexpr auto get() const&& noexcept
                -> T const&&
            {
                return static_cast<T const&&>(*this);
            }
        };

        ////////////////////////////////////////////////////////////
        // Type used for the storage of the pair member: the
        // template integer parameter is used to disambiguate the
        // types when both have the same underlying types

        template<typename T1, typename T2, bool B=is_ebco_eligible_v<T1>>
        struct tight_pair_storage:
            tight_pair_element<0, T1>,
            tight_pair_element<1, T2>
        {
            ////////////////////////////////////////////////////////////
            // Construction

            constexpr tight_pair_storage():
                tight_pair_element<0, T1>(),
                tight_pair_element<1, T2>()
            {}

            template<typename U1, typename U2>
            constexpr tight_pair_storage(U1&& first, U2&& second):
                tight_pair_element<0, T1>(std::forward<U1>(first)),
                tight_pair_element<1, T2>(std::forward<U2>(second))
            {}

            template<typename... Args1, typename... Args2>
            constexpr tight_pair_storage(std::piecewise_construct_t pc,
                                         std::tuple<Args1...>&& first_args,
                                         std::tuple<Args2...>&& second_args):
                tight_pair_element<0, T1>(pc, std::move(first_args)),
                tight_pair_element<1, T2>(pc, std::move(second_args))
            {}

            ////////////////////////////////////////////////////////////
            // Element access

            template<std::size_t N>
            constexpr auto get() &
                -> decltype(auto)
            {
                if constexpr (N == 0) {
                    return static_cast<tight_pair_element<0, T1>&>(*this).get();
                } else if constexpr (N == 1) {
                    return static_cast<tight_pair_element<1, T2>&>(*this).get();
                }
            }

            template<std::size_t N>
            constexpr auto get() const&
                -> decltype(auto)
            {
                if constexpr (N == 0) {
                    return static_cast<tight_pair_element<0, T1> const&>(*this).get();
                } else if constexpr (N == 1) {
                    return static_cast<tight_pair_element<1, T2> const&>(*this).get();
                }
            }

            template<std::size_t N>
            constexpr auto get() &&
                -> decltype(auto)
            {
                if constexpr (N == 0) {
                    return static_cast<tight_pair_element<0, T1>&&>(*this).get();
                } else if constexpr (N == 1) {
                    return static_cast<tight_pair_element<1, T2>&&>(*this).get();
                }
            }

            template<std::size_t N>
            constexpr auto get() const&&
                -> decltype(auto)
            {
                if constexpr (N == 0) {
                    return static_cast<tight_pair_element<0, T1> const&&>(*this).get();
                } else if constexpr (N == 1) {
                    return static_cast<tight_pair_element<1, T2> const&&>(*this).get();
                }
            }
        };

        template<typename T>
        struct tight_pair_storage<T, T, true>:
            // Inherit to compress
            tight_pair_element<0, T>,
            tight_pair_element<1, T>
        {
            ////////////////////////////////////////////////////////////
            // Construction

            constexpr tight_pair_storage():
                tight_pair_element<0, T>(),
                tight_pair_element<1, T>()
            {}

            template<typename U1, typename U2>
            constexpr tight_pair_storage(U1&& first, U2&& second):
                tight_pair_element<0, T>(std::forward<U1>(first)),
                tight_pair_element<1, T>(std::forward<U2>(second))
            {}

            template<typename... Args1, typename... Args2>
            constexpr tight_pair_storage(std::piecewise_construct_t pc,
                                         std::tuple<Args1...> first_args,
                                         std::tuple<Args2...> second_args):
                tight_pair_element<0, T>(pc, first_args),
                tight_pair_element<1, T>(pc, second_args)
            {}

            ////////////////////////////////////////////////////////////
            // Element access

            template<std::size_t N>
            constexpr auto get() &
                -> decltype(auto)
            {
                return static_cast<tight_pair_element<N, T>&>(*this).get();
            }

            template<std::size_t N>
            constexpr auto get() const&
                -> decltype(auto)
            {
                return static_cast<tight_pair_element<N, T> const&>(*this).get();
            }

            template<std::size_t N>
            constexpr auto get() &&
                -> decltype(auto)
            {
                return static_cast<tight_pair_element<N, T>&&>(*this).get();
            }

            template<std::size_t N>
            constexpr auto get() const&&
                -> decltype(auto)
            {
                return static_cast<tight_pair_element<N, T> const&&>(*this).get();
            }
        };

        template<typename T>
        struct tight_pair_storage<T, T, false>
        {
            // Store elements contiguously, avoid padding between elements
            T elements[2];

            ////////////////////////////////////////////////////////////
            // Construction

            constexpr tight_pair_storage():
                elements()
            {}

            template<typename U1, typename U2>
            constexpr tight_pair_storage(U1&& first, U2&& second):
               elements{T(std::forward<U1>(first)), T(std::forward<U2>(second))}
            {}

            template<typename... Args1, typename... Args2>
            constexpr tight_pair_storage(std::piecewise_construct_t,
                                         std::tuple<Args1...>&& first_args,
                                         std::tuple<Args2...>&& second_args):
                elements{std::make_from_tuple<T>(std::move(first_args)),
                         std::make_from_tuple<T>(std::move(second_args))}
            {}

            ////////////////////////////////////////////////////////////
            // Element access

            template<std::size_t N>
            constexpr auto get() &
                -> decltype(auto)
            {
                return static_cast<T&>(elements[N]);
            }

            template<std::size_t N>
            constexpr auto get() const&
                -> decltype(auto)
            {
                return static_cast<T const&>(elements[N]);
            }

            template<std::size_t N>
            constexpr auto get() &&
                -> decltype(auto)
            {
                return static_cast<T&&>(elements[N]);
            }

            template<std::size_t N>
            constexpr auto get() const&&
                -> decltype(auto)
            {
                return static_cast<T const&&>(elements[N]);
            }
        };
    }

    ////////////////////////////////////////////////////////////
    // Main class

    template<typename T1, typename T2>
    struct tight_pair
    {
        private:

            // Actual storage of the values
            detail::tight_pair_storage<T1, T2> storage;

            struct check_args
            {
                template<typename U1, typename U2>
                static constexpr bool enable_default()
                {
                    return std::is_default_constructible<U1>::value
                        && std::is_default_constructible<U2>::value;
                }

                template<typename U1, typename U2>
                static constexpr bool enable_explicit()
                {
                    return std::is_constructible<T1, U1>::value
                        && std::is_constructible<T2, U2>::value
                        && (not std::is_convertible<U1, T1>::value ||
                            not std::is_convertible<U2, T2>::value);
                }

                template<typename U1, typename U2>
                static constexpr bool enable_implicit()
                {
                    return std::is_constructible<T1, U1>::value
                        && std::is_constructible<T2, U2>::value
                        && std::is_convertible<U1, T1>::value
                        && std::is_convertible<U2, T2>::value;
                }
            };

            /*struct check_tuple_like_constructor
            {
                template<typename Tuple>
                static constexpr bool enable_implicit()
                {
                    return __tuple_convertible<Tuple, tight_pair>::value;
                }

                template<typename Tuple>
                static constexpr bool enable_explicit()
                {
                    return __tuple_constructible<Tuple, tight_pair>::value
                        && not __tuple_convertible<Tuple, tight_pair>::value;
                }

                template<typename Tuple>
                static constexpr bool enable_assign()
                {
                    return __tuple_assignable<Tuple, tight_pair>::value;
                }
            };*/

            /*template<typename T>
            using check_pair_like = std::conditional_t<
                detail::pair_like<T>::value && not std::is_same<std::decay_t<T>, tight_pair>::value,
                check_tuple_like_constructor,
                detail::check_tuple_constructor_fail
            >;*/

        public:

            ////////////////////////////////////////////////////////////
            // Construction

            tight_pair(tight_pair const&) = default;
            tight_pair(tight_pair&&) = default;

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    check_args::template enable_default<U1, U2>(),
                    bool
                > = false
            >
            constexpr tight_pair():
                storage()
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    check_args::template enable_explicit<U1 const&, U2 const&>(),
                    bool
                > = false
            >
            constexpr explicit tight_pair(T1 const& first, T2 const& second):
                storage(first, second)
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    check_args::template enable_implicit<U1 const&, U2 const&>(),
                    bool
                > = false
            >
            constexpr tight_pair(T1 const& first, T2 const& second):
                storage(first, second)
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    check_args::template enable_explicit<U1, U2>(),
                    bool
                > = false
            >
            constexpr explicit tight_pair(U1&& first, U2&& second):
                storage(std::forward<U1>(first), std::forward<U2>(second))
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    check_args::template enable_implicit<U1, U2>(),
                    bool
                > = false
            >
            constexpr tight_pair(U1&& first, U2&& second):
                storage(std::forward<U1>(first), std::forward<U2>(second))
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    check_args::template enable_explicit<U1 const&, U2 const&>(),
                    bool
                > = false
            >
            constexpr explicit tight_pair(tight_pair<U1, U2> const& pair):
                storage(get<0>(pair), get<1>(pair))
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    check_args::template enable_implicit<U1 const&, U2 const&>(),
                    bool
                > = false
            >
            constexpr tight_pair(tight_pair<U1, U2> const& pair):
                storage(get<0>(pair), get<1>(pair))
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    check_args::template enable_explicit<U1, U2>(),
                    bool
                > = false
            >
            constexpr explicit tight_pair(tight_pair<U1, U2>&& pair):
                storage(std::forward<U1>(get<0>(pair)), std::forward<U2>(get<1>(pair)))
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    check_args::template enable_implicit<U1, U2>(),
                    bool
                > = false
            >
            constexpr tight_pair(tight_pair<U1, U2>&& pair):
                storage(std::forward<U1>(get<0>(pair)), std::forward<U2>(get<1>(pair)))
            {}

            /*template<
                typename Tuple,
                std::enable_if_t<
                    check_pair_like<Tuple>::template enable_explicit<Tuple>()
                >
            >
            constexpr explicit tight_pair(Tuple&& tuple):
                storage(
                    std::forward<U1>(get<0>(std::forward<Tuple>(tuple))),
                    std::forward<U2>(get<1>(std::forward<Tuple>(tuple)))
                )
            {}

            template<
                typename Tuple,
                std::enable_if_t<
                    check_pair_like<Tuple>::template enable_implicit<Tuple>()
                >
            >
            constexpr tight_pair(Tuple&& tuple):
                storage(
                    std::forward<U1>(get<0>(std::forward<Tuple>(tuple))),
                    std::forward<U2>(get<1>(std::forward<Tuple>(tuple)))
                )
            {}*/

            template<typename... Args1, typename... Args2>
            constexpr tight_pair(std::piecewise_construct_t pc,
                                 std::tuple<Args1...> first_args,
                                 std::tuple<Args2...> second_args):
                storage(pc, std::move(first_args), std::move(second_args))
            {}

            ////////////////////////////////////////////////////////////
            // Assignment operator

            constexpr auto operator=(std::conditional_t<
                                std::is_copy_assignable<T1>::value &&
                                std::is_copy_assignable<T2>::value,
                            tight_pair, detail::nat> const& pair)
                noexcept(noexcept(std::is_nothrow_copy_assignable<T1>::value &&
                                  std::is_nothrow_copy_assignable<T2>::value))
                -> tight_pair&
            {
                storage.template get<0>() = pair.storage.template get<0>();
                storage.template get<1>() = pair.storage.template get<1>();
                return *this;
            }

            constexpr auto operator=(std::conditional_t<
                                std::is_move_assignable<T1>::value &&
                                std::is_move_assignable<T2>::value,
                            tight_pair, detail::nat>&& pair)
                noexcept(noexcept(std::is_nothrow_move_assignable<T1>::value &&
                                  std::is_nothrow_move_assignable<T2>::value))
                -> tight_pair&
            {
                storage.template get<0>() = std::forward<T1>(pair.storage.template get<0>());
                storage.template get<1>() = std::forward<T2>(pair.storage.template get<1>());
                return *this;
            }

            /*template<typename Tuple, std::enable_if_t<
                check_pair_like<Tuple>::template enable_assign<Tuple>()
             >>
            constexpr auto operator=(Tuple&& pair)
                -> tight_pair&
            {
                using detail::get;
                using std::get;
                first = get<0>(std::forward<Tuple>(pair));
                second = get<1>(std::forward<Tuple>(pair));
                return *this;
            }*/

            ////////////////////////////////////////////////////////////
            // Swap

            constexpr auto swap(tight_pair& other)
                noexcept(std::is_nothrow_swappable_v<T1> &&
                         std::is_nothrow_swappable_v<T2>)
                -> void
            {
                using std::swap;
                swap(storage.template get<0>(), other.storage.template get<0>());
                swap(storage.template get<1>(), other.storage.template get<1>());
            }

            ////////////////////////////////////////////////////////////
            // Befriend get<> functions to let them access the storage

            template<std::size_t N, typename U1, typename U2>
            friend constexpr auto get(tight_pair<U1, U2>&) noexcept
                -> std::tuple_element_t<N, tight_pair<U1, U2>>&;

            template<std::size_t N, typename U1, typename U2>
            friend constexpr auto get(tight_pair<U1, U2> const&) noexcept
                -> std::tuple_element_t<N, tight_pair<U1, U2>> const&;

            template<std::size_t N, typename U1, typename U2>
            friend constexpr auto get(tight_pair<U1, U2>&&) noexcept
                -> std::tuple_element_t<N, tight_pair<U1, U2>>&&;

            template<std::size_t N, typename U1, typename U2>
            friend constexpr auto get(tight_pair<U1, U2> const&&) noexcept
                -> std::tuple_element_t<N, tight_pair<U1, U2>> const&&;
    };

    ////////////////////////////////////////////////////////////
    // Deduction guide

    template<typename T1, typename T2>
    tight_pair(T1, T2)
        -> tight_pair<T1, T2>;

    ////////////////////////////////////////////////////////////
    // Free swap function

    template<
        typename T1,
        typename T2,
        typename = std::enable_if_t<
            std::is_swappable_v<T1> && std::is_swappable_v<T2>
        >
    >
    auto swap(tight_pair<T1, T2>& lhs, tight_pair<T1, T2>& rhs)
        noexcept(noexcept(lhs.swap(rhs)))
        -> void
    {
        lhs.swap(rhs);
    }

    ////////////////////////////////////////////////////////////
    // get functions for element access and decomposition
    // declarations

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2>& pair) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>>&
    {
        return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>>&>(
            pair.storage.template get<N>()
        );
    }

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2> const& pair) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>> const&
    {
        return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>> const&>(
            pair.storage.template get<N>()
        );
    }

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2>&& pair) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>>&&
    {
        return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>>&&>(
            pair.storage.template get<N>()
        );
    }

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2> const&& pair) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>> const&&
    {
        return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>> const&&>(
            pair.storage.template get<N>()
        );
    }

    ////////////////////////////////////////////////////////////
    // Generic comparison and relational operators

    template<typename T, typename U>
    constexpr auto operator==(tight_pair<T, U> const& lhs, tight_pair<T, U> const& rhs)
        -> bool
    {
        return get<0>(lhs) == get<0>(rhs)
            && get<1>(lhs) == get<1>(rhs);
    }

    template<typename T, typename U>
    constexpr auto operator!=(tight_pair<T, U> const& lhs, tight_pair<T, U> const& rhs)
        -> bool
    {
        return get<0>(lhs) != get<0>(rhs)
            || get<1>(lhs) != get<1>(rhs);
    }

    template<typename T, typename U>
    constexpr auto operator<(tight_pair<T, U> const& lhs, tight_pair<T, U> const& rhs)
        -> bool
    {
        if (get<0>(lhs) < get<0>(rhs)) {
            return true;
        }
        if (get<0>(rhs) < get<0>(lhs)) {
            return false;
        }
        return get<1>(lhs) < get<1>(rhs);
    }

    template<typename T, typename U>
    constexpr auto operator<=(tight_pair<T, U> const& lhs, tight_pair<T, U> const& rhs)
        -> bool
    {
        return not(rhs < lhs);
    }

    template<typename T, typename U>
    constexpr auto operator>(tight_pair<T, U> const& lhs, tight_pair<T, U> const& rhs)
        -> bool
    {
        return rhs < lhs;
    }

    template<typename T, typename U>
    constexpr auto operator>=(tight_pair<T, U> const& lhs, tight_pair<T, U> const& rhs)
        -> bool
    {
        return not(lhs < rhs);
    }

    ////////////////////////////////////////////////////////////
    // Comparison and relational operators optimized to be
    // branchless when possible

    // TODO: pack small integer comparison functions
}

#endif // CRUFT_TIGHT_PAIR_H_

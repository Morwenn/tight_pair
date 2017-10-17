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
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
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

    template<typename T1, typename T2>
    struct tuple_size<cruft::tight_pair<T1, T2>>:
        std::integral_constant<std::size_t, 2>
    {};

    template<typename T1, typename T2>
    struct tuple_element<0, cruft::tight_pair<T1, T2>>
    {
        using type = T1;
    };

    template<typename T1, typename T2>
    struct tuple_element<1, cruft::tight_pair<T1, T2>>
    {
        using type = T2;
    };

    template<std::size_t N, typename T1, typename T2>
    struct tuple_element<N, cruft::tight_pair<T1, T2>>
    {
        static_assert(N < 2, "out of bounds index for tuple_element<N, cruft::tight_pair<T1, T2>>");
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
        // Detect whether an integer has padding bits

        // Partly taken from WG14 N1899, also handles unsigned
        // integer types such as unsigned __int128 that don't have
        // an std::numeric_limits specialization
        template<typename UnsignedInteger>
        constexpr auto has_padding_bits()
            -> bool
        {
            std::size_t precision = 0;
            auto num = static_cast<UnsignedInteger>(-1);
            while (num != 0) {
                if (num % 2 == 1) {
                    ++precision;
                }
                num >>= 1;
            }
            return precision != sizeof(UnsignedInteger) * CHAR_BIT;
        }

        ////////////////////////////////////////////////////////////
        // Detect endianness at compile-time, code taken from P0463

        enum class endian
        {
#ifdef _WIN32
            little = 0,
            big    = 1,
            native = little
#else
            little = __ORDER_LITTLE_ENDIAN__,
            big    = __ORDER_BIG_ENDIAN__,
            native = __BYTE_ORDER__
#endif
        };

        // TODO: this shouldn't be true, we should just disallow memcpy tricks
        static_assert(endian::native == endian::little || endian::native == endian::big,
                      "platforms with exotic endianness are not supported");

        ////////////////////////////////////////////////////////////
        // Find an unsigned integer type twice as big as the given
        // one, ensure all bits are used

        template<typename UInt>
        constexpr auto twice_as_big()
            -> decltype(auto)
        {
            if constexpr (has_padding_bits<UInt>()) {
                return;
            }

            else if constexpr(sizeof(unsigned short) == 2 * sizeof(UInt) &&
                         not has_padding_bits<unsigned short>()) {
                return static_cast<unsigned short>(0);
            }

            else if constexpr(sizeof(unsigned int) == 2 * sizeof(UInt) &&
                         not has_padding_bits<unsigned int>()) {
                return static_cast<unsigned int>(0);
            }

            else if constexpr(sizeof(unsigned long) == 2 * sizeof(UInt) &&
                         not has_padding_bits<unsigned long>()) {
                return static_cast<unsigned long>(0);
            }

            else if constexpr(sizeof(unsigned long long) == 2 * sizeof(UInt) &&
                         not has_padding_bits<unsigned long long>()) {
                return static_cast<unsigned long long>(0);
            }

            else if constexpr(sizeof(std::uintmax_t) == 2 * sizeof(UInt) &&
                         not has_padding_bits<std::uintmax_t>()) {
                return static_cast<std::uintmax_t>(0);
            }

#if defined(__clang__) && defined(__SIZEOF_INT128__)
            // Don't define it for GCC: then current codegen currently
            // generates branches, which defeats the purpose of our
            // dedicated optimizations
            else if constexpr(sizeof(unsigned __int128) == 2 * sizeof(UInt) &&
                         not has_padding_bits<unsigned __int128>()) {
                return static_cast<unsigned __int128>(0);
            }
#endif
        }

        template<typename T>
        struct has_twice_as_big:
            std::negation<std::is_void<
                decltype(twice_as_big<T>())
            >>
        {};

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

        ////////////////////////////////////////////////////////////
        // Utilities to work with pair-like types: types for which
        // std::tuple_size_v<T> == 2 and get<0>/get<1> work

        namespace adl_hook
        {
            using std::get;

            template<typename T, typename Enable>
            struct pair_like_impl:
                std::false_type
            {};

            template<typename T>
            struct pair_like_impl<
                T,
                std::void_t<
                    decltype(get<0>(std::declval<T&>())),
                    decltype(get<1>(std::declval<T&>()))
                >
            >:
                std::bool_constant<std::tuple_size_v<T> == 2>
            {};
        }

        template<typename T> struct pair_like:
            adl_hook::pair_like_impl<T, void>
        {};
        template<typename T> struct pair_like<const T>: pair_like<T> {};
        template<typename T> struct pair_like<volatile T>: pair_like<T> {};
        template<typename T> struct pair_like<const volatile T>: pair_like<T> {};

        // Safeguard to avoid ambiguous conversions
        template<typename T1, typename T2>
        struct pair_like<cruft::tight_pair<T1, T2>>:
            std::false_type
        {};

        // pair_assignable, pair_constructible, pair_convertible

        namespace adl_hook
        {
            // Import a template get for ADL
            using std::get;

            template<typename To, typename From>
            struct pair_assignable:
                std::bool_constant<
                    std::is_assignable_v<
                        decltype(get<0>(std::declval<To>())),
                        decltype(get<0>(std::declval<From>()))
                    > &&
                    std::is_assignable_v<
                        decltype(get<1>(std::declval<To>())),
                        decltype(get<1>(std::declval<From>()))
                    >
                >
            {};

            template<typename To, typename From>
            struct pair_constructible:
                std::bool_constant<
                    std::is_constructible_v<
                        decltype(get<0>(std::declval<To>())),
                        decltype(get<0>(std::declval<From>()))
                    > &&
                    std::is_constructible_v<
                        decltype(get<1>(std::declval<To>())),
                        decltype(get<1>(std::declval<From>()))
                    >
                >
            {};

            template<typename From, typename To>
            struct pair_convertible:
                std::bool_constant<
                    std::is_convertible_v<
                        decltype(get<0>(std::declval<From>())),
                        decltype(get<0>(std::declval<To>()))
                    > &&
                    std::is_assignable_v<
                        decltype(get<1>(std::declval<From>())),
                        decltype(get<1>(std::declval<To>()))
                    >
                >
            {};
        }

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

                tight_pair_element(tight_pair_element const&) = default;
                tight_pair_element(tight_pair_element&&) = default;

                constexpr tight_pair_element()
                    noexcept(std::is_nothrow_default_constructible<T>::value):
                    value()
                {
                    static_assert(not std::is_reference<T>::value,
                                  "attempted to default construct a reference element");
                }

                template<
                    typename U,
                    typename = std::enable_if_t<
                        std::is_constructible_v<T, U>
                    >
                >
                constexpr explicit tight_pair_element(U&& other)
                    noexcept(std::is_nothrow_constructible_v<T, U>):
                    value(std::forward<U>(other))
                {}

                template<typename... Args>
                constexpr tight_pair_element(std::piecewise_construct_t,
                                             std::tuple<Args...> args):
                    value(std::make_from_tuple<T>(args))
                {}

                template<typename U>
                constexpr auto operator=(U&& other)
                    noexcept(std::is_nothrow_assignable<T&, U>::value)
                    -> tight_pair_element&
                {
                    value = std::forward<U>(other);
                    return *this;
                }

                ////////////////////////////////////////////////////////////
                // Element access

                constexpr auto do_get() & noexcept
                    -> T&
                {
                    return static_cast<T&>(value);
                }

                constexpr auto do_get() const& noexcept
                    -> T const&
                {
                    return static_cast<T const&>(value);
                }

                constexpr auto do_get() && noexcept
                    -> T&&
                {
                    return static_cast<T&&>(value);
                }

                constexpr auto do_get() const&& noexcept
                    -> T const&&
                {
                    return static_cast<T const&&>(value);
                }
        };

        template<std::size_t N, typename T>
        struct tight_pair_element<N, T, true>:
            private T
        {
            ////////////////////////////////////////////////////////////
            // Construction

            tight_pair_element(tight_pair_element const&) = default;
            tight_pair_element(tight_pair_element&&) = default;

            constexpr tight_pair_element()
                noexcept(std::is_nothrow_default_constructible<T>::value)
            {}

            template<
                typename U,
                typename = std::enable_if_t<
                    std::is_constructible_v<T, U>
                >
            >
            constexpr explicit tight_pair_element(U&& other)
                noexcept(std::is_nothrow_constructible_v<T, U>):
                T(std::forward<U>(other))
            {}

            template<typename... Args>
            constexpr tight_pair_element(std::piecewise_construct_t,
                                         std::tuple<Args...> args):
                T(std::make_from_tuple<T>(args))
            {}

            ////////////////////////////////////////////////////////////
            // Element access

            constexpr auto do_get() & noexcept
                -> T&
            {
                return static_cast<T&>(*this);
            }

            constexpr auto do_get() const& noexcept
                -> T const&
            {
                return static_cast<T const&>(*this);
            }

            constexpr auto do_get() && noexcept
                -> T&&
            {
                return static_cast<T&&>(*this);
            }

            constexpr auto do_get() const&& noexcept
                -> T const&&
            {
                return static_cast<T const&&>(*this);
            }
        };

        ////////////////////////////////////////////////////////////
        // Whether the array elements need to be swapped

        // Additional notes: in the case of little endian architectures
        // the fields of the array will be reversed so that the array
        // can be reinterpreted as an integer twice as big when storing
        // integer data

        template<typename T>
        struct needs_reordering:
            std::bool_constant<
                endian::native == endian::little &&
                std::conjunction<
                    std::is_unsigned<T>,
                    has_twice_as_big<T>
                >::value
            >
        {};

        template<typename T> struct needs_reordering<const T>: needs_reordering<T> {};
        template<typename T> struct needs_reordering<volatile T>: needs_reordering<T> {};
        template<typename T> struct needs_reordering<const volatile T>: needs_reordering<T> {};

        ////////////////////////////////////////////////////////////
        // Type used for the storage of the pair member: the
        // template integer parameter is used to disambiguate the
        // types when both have the same underlying types

        template<
            typename T1,
            typename T2,
            bool RegularStorage = is_ebco_eligible_v<T1> || std::is_reference_v<T1>,
            bool NeedsReodering = needs_reordering<T1>::value
        >
        struct tight_pair_storage:
            tight_pair_element<0, T1>,
            tight_pair_element<1, T2>
        {
            ////////////////////////////////////////////////////////////
            // Construction

            tight_pair_storage(tight_pair_storage const&) = default;
            tight_pair_storage(tight_pair_storage&&) = default;

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
            constexpr auto do_get() &
                -> decltype(auto)
            {
                if constexpr (N == 0) {
                    return static_cast<tight_pair_element<0, T1>&>(*this).do_get();
                } else if constexpr (N == 1) {
                    return static_cast<tight_pair_element<1, T2>&>(*this).do_get();
                }
            }

            template<std::size_t N>
            constexpr auto do_get() const&
                -> decltype(auto)
            {
                if constexpr (N == 0) {
                    return static_cast<tight_pair_element<0, T1> const&>(*this).do_get();
                } else if constexpr (N == 1) {
                    return static_cast<tight_pair_element<1, T2> const&>(*this).do_get();
                }
            }

            template<std::size_t N>
            constexpr auto do_get() &&
                -> decltype(auto)
            {
                if constexpr (N == 0) {
                    return static_cast<tight_pair_element<0, T1>&&>(*this).do_get();
                } else if constexpr (N == 1) {
                    return static_cast<tight_pair_element<1, T2>&&>(*this).do_get();
                }
            }

            template<std::size_t N>
            constexpr auto do_get() const&&
                -> decltype(auto)
            {
                if constexpr (N == 0) {
                    return static_cast<tight_pair_element<0, T1> const&&>(*this).do_get();
                } else if constexpr (N == 1) {
                    return static_cast<tight_pair_element<1, T2> const&&>(*this).do_get();
                }
            }
        };

        template<typename T>
        struct tight_pair_storage<T, T, false, false>
        {
            // Store elements contiguously, avoid padding between elements
            T elements[2];

            ////////////////////////////////////////////////////////////
            // Construction

            tight_pair_storage(tight_pair_storage const&) = default;
            tight_pair_storage(tight_pair_storage&&) = default;

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
            constexpr auto do_get() &
                -> decltype(auto)
            {
                return static_cast<T&>(elements[N]);
            }

            template<std::size_t N>
            constexpr auto do_get() const&
                -> decltype(auto)
            {
                return static_cast<T const&>(elements[N]);
            }

            template<std::size_t N>
            constexpr auto do_get() &&
                -> decltype(auto)
            {
                return static_cast<T&&>(elements[N]);
            }

            template<std::size_t N>
            constexpr auto do_get() const&&
                -> decltype(auto)
            {
                return static_cast<T const&&>(elements[N]);
            }
        };

        template<typename T>
        struct tight_pair_storage<T, T, false, true>
        {
            // Store elements contiguously, avoid padding between elements,
            // reorder for more efficient bit tricks
            T elements[2];

            ////////////////////////////////////////////////////////////
            // Construction

            tight_pair_storage(tight_pair_storage const&) = default;
            tight_pair_storage(tight_pair_storage&&) = default;

            constexpr tight_pair_storage():
                elements()
            {}

            template<typename U1, typename U2>
            constexpr tight_pair_storage(U1&& second, U2&& first):
               elements{T(std::forward<U2>(first)), T(std::forward<U1>(second))}
            {}

            template<typename... Args1, typename... Args2>
            constexpr tight_pair_storage(std::piecewise_construct_t,
                                         std::tuple<Args1...>&& second_args,
                                         std::tuple<Args2...>&& first_args):
                elements{std::make_from_tuple<T>(std::move(first_args)),
                         std::make_from_tuple<T>(std::move(second_args))}
            {}

            ////////////////////////////////////////////////////////////
            // Element access

            template<std::size_t N>
            constexpr auto do_get() &
                -> decltype(auto)
            {
                if constexpr (N == 0) {
                    return static_cast<T&>(elements[1]);
                } else if constexpr (N == 1) {
                    return static_cast<T&>(elements[0]);
                }
            }

            template<std::size_t N>
            constexpr auto do_get() const&
                -> decltype(auto)
            {
                if constexpr (N == 0) {
                    return static_cast<T const&>(elements[1]);
                } else if constexpr (N == 1) {
                    return static_cast<T const&>(elements[0]);
                }
            }

            template<std::size_t N>
            constexpr auto do_get() &&
                -> decltype(auto)
            {
                if constexpr (N == 0) {
                    return static_cast<T&&>(elements[1]);
                } else if constexpr (N == 1) {
                    return static_cast<T&&>(elements[0]);
                }
            }

            template<std::size_t N>
            constexpr auto do_get() const&&
                -> decltype(auto)
            {
                if constexpr (N == 0) {
                    return static_cast<T const&&>(elements[1]);
                } else if constexpr (N == 1) {
                    return static_cast<T const&&>(elements[0]);
                }
            }
        };
    }

    ////////////////////////////////////////////////////////////
    // Main class

    template<typename T1, typename T2>
    struct tight_pair:
        private detail::tight_pair_storage<T1, T2>
    {
        private:

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

            struct check_tuple_like_constructor
            {
                template<typename Tuple>
                static constexpr bool enable_implicit()
                {
                    return detail::adl_hook::pair_convertible<Tuple, tight_pair>::value;
                }

                template<typename Tuple>
                static constexpr bool enable_explicit()
                {
                    return detail::adl_hook::pair_constructible<tight_pair&, Tuple>::value
                        && not detail::adl_hook::pair_convertible<Tuple, tight_pair>::value;
                }

                template<typename Tuple>
                static constexpr bool enable_assign()
                {
                    return detail::adl_hook::pair_assignable<tight_pair&, Tuple>::value;
                }
            };

            template<typename T>
            using check_pair_like = std::conditional_t<
                detail::pair_like<std::remove_reference_t<T>>::value
                && not std::is_same_v<std::decay_t<T>, tight_pair>,
                check_tuple_like_constructor,
                detail::check_tuple_constructor_fail
            >;

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
                detail::tight_pair_storage<T1, T2>()
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
                detail::tight_pair_storage<T1, T2>(first, second)
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
                detail::tight_pair_storage<T1, T2>(first, second)
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
                detail::tight_pair_storage<T1, T2>(std::forward<U1>(first),
                                                   std::forward<U2>(second))
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
                detail::tight_pair_storage<T1, T2>(std::forward<U1>(first),
                                                   std::forward<U2>(second))
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
                detail::tight_pair_storage<T1, T2>(get<0>(pair), get<1>(pair))
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
                detail::tight_pair_storage<T1, T2>(get<0>(pair), get<1>(pair))
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
                detail::tight_pair_storage<T1, T2>(std::forward<U1>(get<0>(pair)),
                                                   std::forward<U2>(get<1>(pair)))
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
                detail::tight_pair_storage<T1, T2>(std::forward<U1>(get<0>(pair)),
                                                   std::forward<U2>(get<1>(pair)))
            {}

            template<
                typename Tuple,
                std::enable_if_t<
                    check_pair_like<Tuple>::template enable_explicit<Tuple>()
                >
            >
            constexpr explicit tight_pair(Tuple&& tuple):
                detail::tight_pair_storage<T1, T2>(
                    std::forward<T1>(get<0>(std::forward<Tuple>(tuple))),
                    std::forward<T2>(get<1>(std::forward<Tuple>(tuple)))
                )
            {}

            template<
                typename Tuple,
                std::enable_if_t<
                    check_pair_like<Tuple>::template enable_implicit<Tuple>()
                >
            >
            constexpr tight_pair(Tuple&& tuple):
                detail::tight_pair_storage<T1, T2>(
                    std::forward<T1>(get<0>(std::forward<Tuple>(tuple))),
                    std::forward<T2>(get<1>(std::forward<Tuple>(tuple)))
                )
            {}

            template<typename... Args1, typename... Args2>
            constexpr tight_pair(std::piecewise_construct_t pc,
                                 std::tuple<Args1...> first_args,
                                 std::tuple<Args2...> second_args):
                detail::tight_pair_storage<T1, T2>(pc, std::move(first_args),
                                                       std::move(second_args))
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
                get<0>(*this) = get<0>(pair);
                get<1>(*this) = get<1>(pair);
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
                get<0>(*this) = std::forward<T1>(get<0>(pair));
                get<1>(*this) = std::forward<T2>(get<1>(pair));
                return *this;
            }

            template<
                typename Tuple,
                typename = std::enable_if_t<
                    check_pair_like<Tuple>::template enable_assign<Tuple>()
                >
            >
            constexpr auto operator=(Tuple&& pair)
                -> tight_pair&
            {
                using std::get;
                get<0>(*this) = get<0>(std::forward<Tuple>(pair));
                get<1>(*this) = get<1>(std::forward<Tuple>(pair));
                return *this;
            }

            ////////////////////////////////////////////////////////////
            // Swap

            constexpr auto swap(tight_pair& other)
                noexcept(std::is_nothrow_swappable_v<T1> &&
                         std::is_nothrow_swappable_v<T2>)
                -> void
            {
                using std::swap;
                swap(get<0>(*this), get<0>(other));
                swap(get<1>(*this), get<1>(other));
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

    template<typename T1, typename T2>
    tight_pair(std::reference_wrapper<T1>, T2)
        -> tight_pair<T1&, T2>;

    template<typename T1, typename T2>
    tight_pair(T1, std::reference_wrapper<T2>)
        -> tight_pair<T1, T2&>;

    template<typename T1, typename T2>
    tight_pair(std::reference_wrapper<T1>, std::reference_wrapper<T2>)
        -> tight_pair<T1&, T2&>;

    ////////////////////////////////////////////////////////////
    // Free swap function

    template<
        typename T1,
        typename T2,
        typename = std::enable_if_t<
            std::is_swappable_v<T1> && std::is_swappable_v<T2>
        >
    >
    constexpr auto swap(tight_pair<T1, T2>& lhs, tight_pair<T1, T2>& rhs)
        noexcept(noexcept(lhs.swap(rhs)))
        -> void
    {
        lhs.swap(rhs);
    }

    ////////////////////////////////////////////////////////////
    // Index-based get functions for element access and
    // structured bindings

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2>& pair) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>>&
    {
        using storage_t = detail::tight_pair_storage<T1, T2>;
        return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>>&>(
            static_cast<storage_t&>(pair).template do_get<N>()
        );
    }

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2> const& pair) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>> const&
    {
        using storage_t = detail::tight_pair_storage<T1, T2>;
        return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>> const&>(
            static_cast<storage_t const&>(pair).template do_get<N>()
        );
    }

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2>&& pair) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>>&&
    {
        using storage_t = detail::tight_pair_storage<T1, T2>;
        return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>>&&>(
            static_cast<storage_t&&>(pair).template do_get<N>()
        );
    }

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2> const&& pair) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>> const&&
    {
        using storage_t = detail::tight_pair_storage<T1, T2>;
        return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>> const&&>(
            static_cast<const storage_t&&>(pair).template do_get<N>()
        );
    }

    ////////////////////////////////////////////////////////////
    // Type-based get functions for element access

    template<typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2>& pair) noexcept
        -> T1&
    {
        return cruft::get<0>(pair);
    }

    template<typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2> const& pair) noexcept
        -> T1 const&
    {
        return cruft::get<0>(pair);
    }

    template<typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2>&& pair) noexcept
        -> T1&&
    {
        return cruft::get<0>(std::move(pair));
    }

    template<typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2> const&& pair) noexcept
        -> T1 const&&
    {
        return cruft::get<0>(std::move(pair));
    }

    template<typename T1, typename T2>
    constexpr auto get(tight_pair<T2, T1>& pair) noexcept
        -> T1&
    {
        return cruft::get<1>(pair);
    }

    template<typename T1, typename T2>
    constexpr auto get(tight_pair<T2, T1> const& pair) noexcept
        -> T1 const&
    {
        return cruft::get<1>(pair);
    }

    template<typename T1, typename T2>
    constexpr auto get(tight_pair<T2, T1>&& pair) noexcept
        -> T1&&
    {
        return cruft::get<1>(std::move(pair));
    }

    template<typename T1, typename T2>
    constexpr auto get(tight_pair<T2, T1> const&& pair) noexcept
        -> T1 const&&
    {
        return cruft::get<1>(std::move(pair));
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

    template<typename T>
    auto operator<(tight_pair<T, T> const& lhs, tight_pair<T, T> const& rhs)
        -> std::enable_if_t<
            std::conjunction<
                std::is_unsigned<T>,
                detail::has_twice_as_big<T>
            >::value,
            bool
        >
    {
        using big_t = decltype(detail::twice_as_big<T>());
        big_t big_lhs, big_rhs;
        std::memcpy(&big_lhs, &lhs, sizeof(big_t));
        std::memcpy(&big_rhs, &rhs, sizeof(big_t));
        return big_lhs < big_rhs;
    }

    template<typename T>
    auto operator<=(tight_pair<T, T> const& lhs, tight_pair<T, T> const& rhs)
        -> std::enable_if_t<
            std::conjunction<
                std::is_unsigned<T>,
                detail::has_twice_as_big<T>
            >::value,
            bool
        >
    {
        using big_t = decltype(detail::twice_as_big<T>());
        big_t big_lhs, big_rhs;
        std::memcpy(&big_lhs, &lhs, sizeof(big_t));
        std::memcpy(&big_rhs, &rhs, sizeof(big_t));
        return big_lhs <= big_rhs;
    }

    template<typename T>
    auto operator>(tight_pair<T, T> const& lhs, tight_pair<T, T> const& rhs)
        -> std::enable_if_t<
            std::conjunction<
                std::is_unsigned<T>,
                detail::has_twice_as_big<T>
            >::value,
            bool
        >
    {
        using big_t = decltype(detail::twice_as_big<T>());
        big_t big_lhs, big_rhs;
        std::memcpy(&big_lhs, &lhs, sizeof(big_t));
        std::memcpy(&big_rhs, &rhs, sizeof(big_t));
        return big_lhs > big_rhs;
    }

    template<typename T>
    auto operator>=(tight_pair<T, T> const& lhs, tight_pair<T, T> const& rhs)
        -> std::enable_if_t<
            std::conjunction<
                std::is_unsigned<T>,
                detail::has_twice_as_big<T>
            >::value,
            bool
        >
    {
        using big_t = decltype(detail::twice_as_big<T>());
        big_t big_lhs, big_rhs;
        std::memcpy(&big_lhs, &lhs, sizeof(big_t));
        std::memcpy(&big_rhs, &rhs, sizeof(big_t));
        return big_lhs >= big_rhs;
    }
}

#endif // CRUFT_TIGHT_PAIR_H_

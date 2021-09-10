/*
 * Copyright (c) 2017-2021 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CRUFT_TIGHT_PAIR_H_
#define CRUFT_TIGHT_PAIR_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <climits>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#ifndef CRUFT_TIGHT_PAIR_USE_UNSIGNED_128INT
#   if defined(__clang__)
#       define CRUFT_TIGHT_PAIR_USE_UNSIGNED_128INT 1
#   elif defined(__GNUC__)
#       if __GNUC__ >= 8
#           define CRUFT_TIGHT_PAIR_USE_UNSIGNED_128INT 1
#       else
#           define CRUFT_TIGHT_PAIR_USE_UNSIGNED_128INT 0
#       endif
#   else
#       define CRUFT_TIGHT_PAIR_USE_UNSIGNED_128INT 0
#   endif
#endif

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
    //
    // Depending on the standard library used, Clang might warn
    // about mismatched struct/class on tuple_* traits, so we
    // just silence the warning instead of special-casing for
    // every standard library version

#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wmismatched-tags"
#endif
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
#ifdef __clang__
#   pragma clang diagnostic pop
#endif
}

namespace cruft
{
    namespace detail
    {
        ////////////////////////////////////////////////////////////
        // is_implicitly_default_constructible type trait,
        // taken as is from libc++

        template<typename T>
        auto test_implicit_default_constructible(T)
            -> void;

        template<
            typename T,
            typename = void,
            bool = std::is_default_constructible_v<T>
        >
        struct is_implicitly_default_constructible:
            std::false_type
        {};

        template<typename T>
        struct is_implicitly_default_constructible<
            T,
            decltype(test_implicit_default_constructible<T const&>({})),
            true
        >:
            std::true_type
        {};

        template<typename T>
        struct is_implicitly_default_constructible<
            T,
            decltype(test_implicit_default_constructible<T const&>({})),
            false
        >:
            std::false_type
        {};

        ////////////////////////////////////////////////////////////
        // is_copy_assignable_nocheck: avoid issues with incomplete
        // type warnings in MSVC

#if defined(_IS_ASSIGNABLE_NOCHECK_SUPPORTED) && !defined(__CUDACC__)
        template<typename T>
        struct is_copy_assignable_nocheck:
            std::integral_constant<bool, __is_assignable_no_precondition_check(
                std::add_lvalue_reference_t<T>,
                std::add_lvalue_reference_t<const T>
            )>
        {};

        template<typename T>
        struct is_move_assignable_nocheck:
            std::integral_constant<bool, __is_assignable_no_precondition_check(
                std::add_lvalue_reference_t<T>,
                T
            )>
        {};
#else
        template<typename T>
        using is_copy_assignable_nocheck = std::is_copy_assignable<T>;

        template<typename T>
        using is_move_assignable_nocheck = std::is_move_assignable<T>;
#endif

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

        template<typename UnsignedInteger>
        constexpr auto has_padding_bits()
            -> bool
        {
#ifdef __cpp_lib_has_unique_object_representations
            // A note in the standard mentions than unsigned integer types
            // are guaranteed to have unique object representations when
            // they don't have padding bits

            return not std::has_unique_object_representations<UnsignedInteger>::value;
#else
            // Algorithm partly taken from WG14 N1899, also handles unsigned
            // integer types such as __uint128_t that don't have an
            // std::numeric_limits specialization

            std::size_t precision = 0;
            auto num = static_cast<UnsignedInteger>(-1);
            while (num != 0) {
                if (num % 2 == 1) {
                    ++precision;
                }
                num >>= 1;
            }
            return precision != sizeof(UnsignedInteger) * CHAR_BIT;
#endif
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

#if CRUFT_TIGHT_PAIR_USE_UNSIGNED_128INT && defined(__SIZEOF_INT128__)
            // Only use __uint128_t with compilers which are known
            // to produce branchless code for comparisons
            else if constexpr(sizeof(__uint128_t) == 2 * sizeof(UInt) &&
                         not has_padding_bits<__uint128_t>()) {
                return static_cast<__uint128_t>(0);
            }
#endif
        }

        template<typename T>
        struct has_twice_as_big:
            std::negation<std::is_void<
                decltype(twice_as_big<T>())
            >>
        {};

        template<typename T>
        constexpr auto get_twice_as_big(tight_pair<T, T> const& value) noexcept
            -> decltype(twice_as_big<T>())
        {
            // If the two parts of the tight_pair are unsigned integers
            // suitably ordered (which depends on the byte order), this
            // whole function be optimized away as a no-op

            // We use CHAR_BIT instead of std::numeric_limits because the
            // latter might lack a few specializations for types such as
            // __uint128_t unless some specific constant is defined

            using cruft::get;
            return static_cast<decltype(twice_as_big<T>())>(
                get<0>(value)) << sizeof(T) * CHAR_BIT | get<1>(value);
        }

        template<typename T>
        struct can_optimize_compare:
            std::bool_constant<
                (endian::native == endian::little || endian::native == endian::big) &&
                std::conjunction_v<
                    std::is_unsigned<T>,
                    has_twice_as_big<T>
                >
            >
        {};

        template<typename T>
        constexpr auto best_alignment()
            -> std::size_t
        {
            if constexpr(can_optimize_compare<T>::value) {
                return alignof(decltype(twice_as_big<T>()));
            } else {
                return alignof(T);
            }
        }

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
            static constexpr bool enable_explicit_default = false;

            template<typename...>
            static constexpr bool enable_implicit_default = false;

            template<typename...>
            static constexpr bool enable_explicit = false;

            template<typename...>
            static constexpr bool enable_implicit = false;

            template<typename...>
            static constexpr bool enable_assign = false;
        };

        struct check_args_default
        {
            template<typename U1, typename U2>
            static constexpr bool enable_implicit_default =
                detail::is_implicitly_default_constructible<U1>::value &&
                detail::is_implicitly_default_constructible<U2>::value;

            template<typename U1, typename U2>
            static constexpr bool enable_explicit_default =
                std::is_default_constructible<U1>::value &&
                std::is_default_constructible<U2>::value &&
                not enable_implicit_default<U1, U2>;
        };

        template<typename T1, typename T2>
        struct check_args
        {
            template<typename U1, typename U2>
            static constexpr bool enable_explicit =
                std::is_constructible<T1, U1>::value &&
                std::is_constructible<T2, U2>::value &&
                (not std::is_convertible<U1, T1>::value || not std::is_convertible<U2, T2>::value);

            template<typename U1, typename U2>
            static constexpr bool enable_implicit =
                std::is_constructible<T1, U1>::value &&
                std::is_constructible<T2, U2>::value &&
                std::is_convertible<U1, T1>::value &&
                std::is_convertible<U2, T2>::value;
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
                    typename std::tuple_size<std::decay_t<T>>::type,
                    decltype(get<0>(std::declval<T&>())),
                    decltype(get<1>(std::declval<T&>()))
                >
            >:
                std::bool_constant<std::tuple_size_v<std::decay_t<T>> == 2>
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

        template<
            std::size_t N,
            typename T,
            bool = is_ebco_eligible_v<T>,
            bool = std::is_reference_v<T>
        >
        struct tight_pair_element;

        template<std::size_t N, typename T>
        struct tight_pair_element<N, T, false, false>
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

                template<typename Tuple>
                constexpr tight_pair_element(std::piecewise_construct_t, Tuple&& args):
                    value(std::make_from_tuple<T>(std::forward<Tuple>(args)))
                {}

                template<typename U>
                constexpr auto operator=(U&& other)
                    noexcept(std::is_nothrow_assignable<T&, U>::value)
                    -> tight_pair_element&
                {
                    value = std::forward<U>(other);
                    return *this;
                }

                tight_pair_element& operator=(tight_pair_element const&) = default;
                tight_pair_element& operator=(tight_pair_element&&) = default;

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
        struct tight_pair_element<N, T, false, true>
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

                template<typename Tuple>
                constexpr tight_pair_element(std::piecewise_construct_t, Tuple&& args):
                    value(std::make_from_tuple<T>(std::forward<Tuple>(args)))
                {}

                constexpr auto operator=(tight_pair_element const& other)
                    -> tight_pair_element&
                {
                    value = other.value;
                    return *this;
                }

                constexpr auto operator=(tight_pair_element&& other)
                    -> tight_pair_element&
                {
                    value = std::move(other.value);
                    return *this;
                }

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
        struct tight_pair_element<N, T, true, false>:
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

            template<typename Tuple>
            constexpr tight_pair_element(std::piecewise_construct_t, Tuple&& args):
                T(std::make_from_tuple<T>(std::forward<Tuple>(args)))
            {}

            tight_pair_element& operator=(tight_pair_element const&) = default;
            tight_pair_element& operator=(tight_pair_element&&) = default;

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
                can_optimize_compare<T>::value
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

            template<typename Tuple1, typename Tuple2>
            constexpr tight_pair_storage(std::piecewise_construct_t pc,
                                         Tuple1&& first_args, Tuple2&& second_args):
                tight_pair_element<0, T1>(pc, std::forward<Tuple1>(first_args)),
                tight_pair_element<1, T2>(pc, std::forward<Tuple2>(second_args))
            {}

            tight_pair_storage& operator=(tight_pair_storage const&) = default;
            tight_pair_storage& operator=(tight_pair_storage&&) = default;

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
            alignas(best_alignment<T>()) T elements[2];

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

            template<typename Tuple1, typename Tuple2>
            constexpr tight_pair_storage(std::piecewise_construct_t,
                                         Tuple1&& first_args, Tuple2&& second_args):
                elements{std::make_from_tuple<T>(std::forward<Tuple1>(first_args)),
                         std::make_from_tuple<T>(std::forward<Tuple2>(second_args))}
            {}

            tight_pair_storage& operator=(tight_pair_storage const&) = default;
            tight_pair_storage& operator=(tight_pair_storage&&) = default;

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
            alignas(best_alignment<T>()) T elements[2];

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

            template<typename Tuple1, typename Tuple2>
            constexpr tight_pair_storage(std::piecewise_construct_t,
                                         Tuple2&& second_args, Tuple1&& first_args):
                elements{std::make_from_tuple<T>(std::forward<Tuple1>(first_args)),
                         std::make_from_tuple<T>(std::forward<Tuple2>(second_args))}
            {}

            tight_pair_storage& operator=(tight_pair_storage const&) = default;
            tight_pair_storage& operator=(tight_pair_storage&&) = default;

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

        ////////////////////////////////////////////////////////////
        // Free swap function

        template<
            typename T1,
            typename T2,
            typename = std::enable_if_t<
                std::is_swappable_v<T1> && std::is_swappable_v<T2>
            >
        >
        constexpr auto swap(tight_pair_storage<T1, T2>& lhs, tight_pair_storage<T1, T2>& rhs)
            noexcept(std::is_nothrow_swappable_v<T1> && std::is_nothrow_swappable_v<T2>)
            -> void
        {
            auto tmp = std::move(lhs);
            lhs = std::move(rhs);
            rhs = std::move(tmp);
        }
    }

    ////////////////////////////////////////////////////////////
    // Main class

    template<typename T1, typename T2>
    struct tight_pair:
        private detail::tight_pair_storage<T1, T2>
    {
        private:

            struct check_tuple_like_constructor
            {
                template<typename Tuple>
                static constexpr bool enable_implicit =
                    detail::adl_hook::pair_convertible<Tuple, tight_pair>::value;

                template<typename Tuple>
                static constexpr bool enable_explicit =
                    detail::adl_hook::pair_constructible<tight_pair&, Tuple>::value &&
                    not detail::adl_hook::pair_convertible<Tuple, tight_pair>::value;

                template<typename Tuple>
                static constexpr bool enable_assign =
                    detail::adl_hook::pair_assignable<tight_pair&, Tuple>::value;
            };

            template<typename T>
            using check_pair_like = std::conditional_t<
                detail::pair_like<std::remove_reference_t<T>>::value,
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
                    detail::check_args_default::enable_explicit_default<U1, U2>,
                    bool
                > = false
            >
            explicit constexpr tight_pair()
                noexcept(std::is_nothrow_default_constructible_v<T1> &&
                         std::is_nothrow_default_constructible_v<T2>):
                detail::tight_pair_storage<T1, T2>()
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    detail::check_args_default::enable_implicit_default<U1, U2>,
                    bool
                > = false
            >
            constexpr tight_pair()
                noexcept(std::is_nothrow_default_constructible_v<T1> &&
                         std::is_nothrow_default_constructible_v<T2>):
                detail::tight_pair_storage<T1, T2>()
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    detail::check_args<T1, T2>::template enable_explicit<U1 const&, U2 const&>,
                    bool
                > = false
            >
            constexpr explicit tight_pair(T1 const& first, T2 const& second)
                noexcept(std::is_nothrow_copy_constructible_v<T1> &&
                         std::is_nothrow_copy_constructible_v<T2>):
                detail::tight_pair_storage<T1, T2>(first, second)
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    detail::check_args<T1, T2>::template enable_implicit<U1 const&, U2 const&>,
                    bool
                > = false
            >
            constexpr tight_pair(T1 const& first, T2 const& second)
                noexcept(std::is_nothrow_copy_constructible_v<T1> &&
                         std::is_nothrow_copy_constructible_v<T2>):
                detail::tight_pair_storage<T1, T2>(first, second)
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    detail::check_args<T1, T2>::template enable_explicit<U1, U2>,
                    bool
                > = false
            >
            constexpr explicit tight_pair(U1&& first, U2&& second)
                noexcept(std::is_nothrow_constructible_v<T1, U1> &&
                         std::is_nothrow_constructible_v<T2, U2>):
                detail::tight_pair_storage<T1, T2>(std::forward<U1>(first),
                                                   std::forward<U2>(second))
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    detail::check_args<T1, T2>::template enable_implicit<U1, U2>,
                    bool
                > = false
            >
            constexpr tight_pair(U1&& first, U2&& second)
                noexcept(std::is_nothrow_constructible_v<T1, U1> &&
                         std::is_nothrow_constructible_v<T2, U2>):
                detail::tight_pair_storage<T1, T2>(std::forward<U1>(first),
                                                   std::forward<U2>(second))
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    detail::check_args<T1, T2>::template enable_explicit<U1 const&, U2 const&>,
                    bool
                > = false
            >
            constexpr explicit tight_pair(tight_pair<U1, U2> const& pair)
                noexcept(std::is_nothrow_constructible_v<T1, U1 const&> &&
                         std::is_nothrow_constructible_v<T2, U2 const&>):
                detail::tight_pair_storage<T1, T2>(pair.template get<0>(), pair.template get<1>())
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    detail::check_args<T1, T2>::template enable_implicit<U1 const&, U2 const&>,
                    bool
                > = false
            >
            constexpr tight_pair(tight_pair<U1, U2> const& pair)
                noexcept(std::is_nothrow_constructible_v<T1, U1 const&> &&
                         std::is_nothrow_constructible_v<T2, U2 const&>):
                detail::tight_pair_storage<T1, T2>(pair.template get<0>(), pair.template get<1>())
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    detail::check_args<T1, T2>::template enable_explicit<U1, U2>,
                    bool
                > = false
            >
            constexpr explicit tight_pair(tight_pair<U1, U2>&& pair)
                noexcept(std::is_nothrow_constructible_v<T1, U1&&> &&
                         std::is_nothrow_constructible_v<T2, U2&&>):
                detail::tight_pair_storage<T1, T2>(std::forward<U1>(pair.template get<0>()),
                                                   std::forward<U2>(pair.template get<1>()))
            {}

            template<
                typename U1 = T1,
                typename U2 = T2,
                std::enable_if_t<
                    detail::check_args<T1, T2>::template enable_implicit<U1, U2>,
                    bool
                > = false
            >
            constexpr tight_pair(tight_pair<U1, U2>&& pair)
                noexcept(std::is_nothrow_constructible_v<T1, U1&&> &&
                         std::is_nothrow_constructible_v<T2, U2&&>):
                detail::tight_pair_storage<T1, T2>(std::forward<U1>(pair.template get<0>()),
                                                   std::forward<U2>(pair.template get<1>()))
            {}

            template<
                typename Tuple,
                std::enable_if_t<
                    check_pair_like<Tuple>::template enable_explicit<Tuple>
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
                    check_pair_like<Tuple>::template enable_implicit<Tuple>,
                    int
                > = 0
            >
            constexpr tight_pair(Tuple&& tuple):
                detail::tight_pair_storage<T1, T2>(
                    std::forward<T1>(get<0>(std::forward<Tuple>(tuple))),
                    std::forward<T2>(get<1>(std::forward<Tuple>(tuple)))
                )
            {}

            // TODO: proper noexcept clause
            template<typename Tuple1, typename Tuple2>
            constexpr tight_pair(std::piecewise_construct_t pc,
                                 Tuple1&& first_args, Tuple2&& second_args):
                detail::tight_pair_storage<T1, T2>(pc, std::forward<Tuple1>(first_args),
                                                       std::forward<Tuple2>(second_args))
            {}

            ////////////////////////////////////////////////////////////
            // Assignment operator

            constexpr auto operator=(std::conditional_t<
                                detail::is_copy_assignable_nocheck<T1>::value &&
                                detail::is_copy_assignable_nocheck<T2>::value,
                            tight_pair, detail::nat> const& other)
                noexcept(noexcept(std::is_nothrow_copy_assignable<T1>::value &&
                                  std::is_nothrow_copy_assignable<T2>::value))
                -> tight_pair&
            {
                static_cast<detail::tight_pair_storage<T1, T2>&>(*this)
                    = static_cast<detail::tight_pair_storage<T1, T2> const&>(other);
                return *this;
            }

            constexpr auto operator=(std::conditional_t<
                                detail::is_move_assignable_nocheck<T1>::value &&
                                detail::is_move_assignable_nocheck<T2>::value,
                            tight_pair, detail::nat>&& other)
                noexcept(noexcept(std::is_nothrow_move_assignable<T1>::value &&
                                  std::is_nothrow_move_assignable<T2>::value))
                -> tight_pair&
            {
                static_cast<detail::tight_pair_storage<T1, T2>&>(*this)
                    = static_cast<detail::tight_pair_storage<T1, T2>&&>(other);
                return *this;
            }

            template<
                typename Tuple,
                typename = std::enable_if_t<
                    check_pair_like<Tuple>::template enable_assign<Tuple>
                >
            >
            constexpr auto operator=(Tuple&& other)
                -> tight_pair&
            {
                using std::get;
                get<0>(*this) = get<0>(std::forward<Tuple>(other));
                get<1>(*this) = get<1>(std::forward<Tuple>(other));
                return *this;
            }

            ////////////////////////////////////////////////////////////
            // Swap

            constexpr auto swap(tight_pair& other)
                noexcept(std::is_nothrow_swappable_v<T1> && std::is_nothrow_swappable_v<T2>)
                -> void
            {
                cruft::detail::swap(
                    static_cast<detail::tight_pair_storage<T1, T2>&>(*this),
                    static_cast<detail::tight_pair_storage<T1, T2>&>(other)
                );
            }

            ////////////////////////////////////////////////////////////
            // Element access
            //
            // This function is necessary to avoid structured bindings
            // picking a potential inaccessible get from an empty base
            // class

            template<std::size_t N>
            constexpr auto get() & noexcept
                -> std::tuple_element_t<N, tight_pair>&
            {
                using storage_t = detail::tight_pair_storage<T1, T2>;
                return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>>&>(
                    static_cast<storage_t&>(*this).template do_get<N>()
                );
            }

            template<std::size_t N>
            constexpr auto get() const& noexcept
                -> std::tuple_element_t<N, tight_pair> const&
            {
                using storage_t = detail::tight_pair_storage<T1, T2>;
                return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>> const&>(
                    static_cast<storage_t const&>(*this).template do_get<N>()
                );
            }

            template<std::size_t N>
            constexpr auto get() && noexcept
                -> std::tuple_element_t<N, tight_pair>&&
            {
                using storage_t = detail::tight_pair_storage<T1, T2>;
                return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>>&&>(
                    static_cast<storage_t&&>(*this).template do_get<N>()
                );
            }

            template<std::size_t N>
            constexpr auto get() const&& noexcept
                -> std::tuple_element_t<N, tight_pair> const&&
            {
                using storage_t = detail::tight_pair_storage<T1, T2>;
                return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>> const&&>(
                    static_cast<storage_t const&&>(*this).template do_get<N>()
                );
            }

            ////////////////////////////////////////////////////////////
            // Comparison operators (hidden friends)

            friend constexpr auto operator==(tight_pair const& lhs, tight_pair const& rhs)
                -> bool
            {
                if constexpr (std::is_same_v<T1, T2> && detail::can_optimize_compare<T1>::value) {
                    auto big_lhs = detail::get_twice_as_big(lhs);
                    auto big_rhs = detail::get_twice_as_big(rhs);
                    return big_lhs == big_rhs;
                } else {
                    return lhs.get<0>() == rhs.get<0>()
                        && lhs.get<1>() == rhs.get<1>();
                }
            }

            friend constexpr auto operator!=(tight_pair const& lhs, tight_pair const& rhs)
                -> bool
            {
                if constexpr (std::is_same_v<T1, T2> && detail::can_optimize_compare<T1>::value) {
                    auto big_lhs = detail::get_twice_as_big(lhs);
                    auto big_rhs = detail::get_twice_as_big(rhs);
                    return big_lhs != big_rhs;
                } else {
                    return lhs.get<0>() != rhs.get<0>()
                        || lhs.get<1>() != rhs.get<1>();
                }
            }

            ////////////////////////////////////////////////////////////
            // Relational operators, optimized to be branchless when
            // possible (hidden friends)

            friend constexpr auto operator<(tight_pair const& lhs, tight_pair const& rhs)
                -> bool
            {
                if constexpr (std::is_same_v<T1, T2> && detail::can_optimize_compare<T1>::value) {
                    auto big_lhs = detail::get_twice_as_big(lhs);
                    auto big_rhs = detail::get_twice_as_big(rhs);
                    return big_lhs < big_rhs;
                } else {
                    if (lhs.get<0>() < rhs.get<0>()) {
                        return true;
                    }
                    if (rhs.get<0>() < lhs.get<0>()) {
                        return false;
                    }
                    return lhs.get<1>() < rhs.get<1>();
                }
            }

            friend constexpr auto operator<=(tight_pair const& lhs, tight_pair const& rhs)
                -> bool
            {
                if constexpr (std::is_same_v<T1, T2> && detail::can_optimize_compare<T1>::value) {
                    auto big_lhs = detail::get_twice_as_big(lhs);
                    auto big_rhs = detail::get_twice_as_big(rhs);
                    return big_lhs <= big_rhs;
                } else {
                    return not(rhs < lhs);
                }
            }

            friend constexpr auto operator>(tight_pair const& lhs, tight_pair const& rhs)
                -> bool
            {
                if constexpr (std::is_same_v<T1, T2> && detail::can_optimize_compare<T1>::value) {
                    auto big_lhs = detail::get_twice_as_big(lhs);
                    auto big_rhs = detail::get_twice_as_big(rhs);
                    return big_lhs > big_rhs;
                } else {
                    return rhs < lhs;
                }
            }

            friend constexpr auto operator>=(tight_pair const& lhs, tight_pair const& rhs)
                -> bool
            {
                if constexpr (std::is_same_v<T1, T2> && detail::can_optimize_compare<T1>::value) {
                    auto big_lhs = detail::get_twice_as_big(lhs);
                    auto big_rhs = detail::get_twice_as_big(rhs);
                    return big_lhs >= big_rhs;
                } else {
                    return not(lhs < rhs);
                }
            }
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
        return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>>&>(
            pair.template get<N>()
        );
    }

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2> const& pair) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>> const&
    {
        return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>> const&>(
            pair.template get<N>()
        );
    }

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2>&& pair) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>>&&
    {
        return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>>&&>(
            pair.template get<N>()
        );
    }

    template<std::size_t N, typename T1, typename T2>
    constexpr auto get(tight_pair<T1, T2> const&& pair) noexcept
        -> std::tuple_element_t<N, tight_pair<T1, T2>> const&&
    {
        return static_cast<std::tuple_element_t<N, tight_pair<T1, T2>> const&&>(
            pair.template get<N>()
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
}

namespace cppsort
{
    template<typename Compare, typename T>
    struct is_probably_branchless_comparison;

    template<typename T>
    struct is_probably_branchless_comparison<std::less<>, cruft::tight_pair<T, T>>:
        cruft::detail::can_optimize_compare<T>
    {};

    template<typename T>
    struct is_probably_branchless_comparison<std::less<T>, cruft::tight_pair<T, T>>:
        cruft::detail::can_optimize_compare<T>
    {};

    template<typename T>
    struct is_probably_branchless_comparison<std::greater<>, cruft::tight_pair<T, T>>:
        cruft::detail::can_optimize_compare<T>
    {};

    template<typename T>
    struct is_probably_branchless_comparison<std::greater<T>, cruft::tight_pair<T, T>>:
        cruft::detail::can_optimize_compare<T>
    {};
}

#endif // CRUFT_TIGHT_PAIR_H_

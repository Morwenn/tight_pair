/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>

struct DeletedDefault
{
    // A class with a deleted default constructor. Used to test the SFINAE
    // on cruft::tight_pair's default constructor.
    constexpr explicit DeletedDefault(int x): value(x) {}
    constexpr DeletedDefault() = delete;
    int value;
};

template<typename T, bool>
struct DependantType: T {};

template<typename T, bool Val>
using DependantIsDefault = DependantType<std::is_default_constructible<T>, Val>;

template<typename T>
struct DefaultSFINAES
{
    template<
        bool Dummy = false,
        typename = typename std::enable_if<
            DependantIsDefault<T, Dummy>::value
        >::type
    >
    constexpr DefaultSFINAES(): value() {}
    constexpr explicit DefaultSFINAES(T const& x): value(x) {}
    T value;
};

struct NoDefault
{
    constexpr NoDefault(int v): value(v) {}
    int value;
};

template<typename T>
void test_not_is_default_constructible()
{
    {
        using P = cruft::tight_pair<int, T>;
        static_assert(not std::is_default_constructible_v<P>);
        static_assert(std::is_constructible_v<P, int, T>);
    }
    {
        using P = cruft::tight_pair<T, int>;
        static_assert(not std::is_default_constructible_v<P>);
        static_assert(std::is_constructible_v<P, T, int>);
    }
    {
        using P = cruft::tight_pair<T, T>;
        static_assert(not std::is_default_constructible_v<P>);
        static_assert(std::is_constructible_v<P, T, T>);
    }
}

template<typename T>
void test_is_default_constructible()
{
    {
        using P = cruft::tight_pair<int, T>;
        static_assert(std::is_default_constructible_v<P>);
    }
    {
        using P = cruft::tight_pair<T, int>;
        static_assert(std::is_default_constructible_v<P>);
    }
    {
        using P = cruft::tight_pair<T, T>;
        static_assert(std::is_default_constructible_v<P>);
    }
}

template<typename T>
struct IllFormedDefaultImp
{
    constexpr explicit IllFormedDefaultImp(int v): value(v) {}
    constexpr IllFormedDefaultImp(): value(T::DoesNotExistAndShouldNotCompile) {}
    int value;
};

using IllFormedDefault = IllFormedDefaultImp<int>;
    // A class which provides a constexpr default constructor with a valid
    // signature but an ill-formed body. The A compile error will be emitted if
    // the default constructor is instantiated.


// Check that the SFINAE on the default constructor is not evaluated when
// it isn't needed. If the default constructor of 'IllFormedDefault' is evaluated
// in C++11, even with is_default_constructible, then this test should fail to
// compile. In C++14 and greater evaluate each test is evaluated as a constant
// expression.
// See LWG issue #2367
inline void test_illformed_default()
{
    using cruft::get;

    {
        using P = cruft::tight_pair<IllFormedDefault, int>;
        static_assert(std::is_constructible_v<P, IllFormedDefault, int>);
        constexpr P p(IllFormedDefault(42), -5);
        static_assert(get<0>(p).value == 42 && get<1>(p) == -5);
    }
    {
        using P = cruft::tight_pair<int, IllFormedDefault>;
        static_assert(std::is_constructible_v<P, int, IllFormedDefault>);
        constexpr IllFormedDefault dd(-5);
        constexpr P p(42, dd);
        static_assert(get<0>(p) == 42 && get<1>(p).value == -5);
    }
    {
        using P = cruft::tight_pair<IllFormedDefault, IllFormedDefault>;
        static_assert(std::is_constructible_v<P, IllFormedDefault, IllFormedDefault>);
        constexpr P p(IllFormedDefault(42), IllFormedDefault(-5));
        static_assert(get<0>(p).value == 42 && get<1>(p).value == -5);
    }
}

TEST_CASE( "default sfinae" )
{
    {
        // Check that pair<T, U> can still be used even if
        // is_default_constructible<T> or is_default_constructible<U> cause
        // a compilation error.
        test_illformed_default();
    }
    {
        // pair::pair() is only disable in C++11 and beyond.
        test_not_is_default_constructible<NoDefault>();
        test_not_is_default_constructible<DeletedDefault>();
        test_not_is_default_constructible<DefaultSFINAES<int&>>();
        test_not_is_default_constructible<DefaultSFINAES<int&&>>();
        test_not_is_default_constructible<int&>();
        test_not_is_default_constructible<int&&>();
    }
    {
        test_is_default_constructible<int>();
        test_is_default_constructible<DefaultSFINAES<int>>();
    }
}

/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Morwenn
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
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>
#include "archetypes.h"
#include "test_convertible.h"

namespace
{
    template<
        typename T1,
        typename U1,
        bool CanCopy = true,
        bool CanConvert = CanCopy
    >
    void test_pair_const()
    {
        using P1 = cruft::tight_pair<T1, int>;
        using P2 = cruft::tight_pair<int, T1>;
        using UP1 = cruft::tight_pair<U1, int> const&;
        using UP2 = cruft::tight_pair<int, U1> const&;
        static_assert(std::is_constructible_v<P1, UP1> == CanCopy);
        static_assert(test_convertible<P1, UP1>() == CanConvert);
        static_assert(std::is_constructible_v<P2, UP2> == CanCopy);
        static_assert(test_convertible<P2,  UP2>() == CanConvert);
    }

    template<typename T, typename U>
    struct DPair:
        cruft::tight_pair<T, U>
    {
        using Base = cruft::tight_pair<T, U>;
        using Base::Base;
    };

    struct ExplicitT
    {
        constexpr explicit ExplicitT(int x): value(x) {}
        constexpr explicit ExplicitT(ExplicitT const& o): value(o.value) {}
        int value;
    };

    struct ImplicitT
    {
        constexpr ImplicitT(int x): value(x) {}
        constexpr ImplicitT(ImplicitT const& o): value(o.value) {}
        int value;
    };

     struct ExplicitNothrowT
     {
         explicit ExplicitNothrowT(int x) noexcept: value(x) {}
         int value;
     };

     struct ImplicitNothrowT
     {
         ImplicitNothrowT(int x) noexcept: value(x) {}
         int value;
     };
}

TEST_CASE( "const pair U C" )
{
    using cruft::get;

    {
        using P1 = cruft::tight_pair<int, short>;
        using P2 = cruft::tight_pair<double, long>;
        const P1 p1(3, 4);
        const P2 p2 = p1;
        CHECK(get<0>(p2) == 3);
        CHECK(get<1>(p2) == 4);
    }

    {
        // We allow derived types to use this constructor
        using P1 = DPair<long, long>;
        using P2 = cruft::tight_pair<int, int>;
        P1 p1(42, 101);
        P2 p2(p1);
        CHECK(get<0>(p2) == 42);
        CHECK(get<1>(p2) == 101);
    }

    {
        test_pair_const<ImplicitTypes::AllCtors, ImplicitTypes::AllCtors>(); // copy construction
        test_pair_const<ImplicitTypes::AllCtors, ImplicitTypes::AllCtors&>();
        test_pair_const<ImplicitTypes::AllCtors, ImplicitTypes::AllCtors&&>();
        test_pair_const<ImplicitTypes::AllCtors, const ImplicitTypes::AllCtors&>();
        test_pair_const<ImplicitTypes::AllCtors, const ImplicitTypes::AllCtors&&>();

        test_pair_const<ExplicitTypes::AllCtors, ExplicitTypes::AllCtors>(); // copy construction
        test_pair_const<ExplicitTypes::AllCtors, ExplicitTypes::AllCtors&, true, false>();
        test_pair_const<ExplicitTypes::AllCtors, ExplicitTypes::AllCtors&&, true, false>();
        test_pair_const<ExplicitTypes::AllCtors, const ExplicitTypes::AllCtors&, true, false>();
        test_pair_const<ExplicitTypes::AllCtors, const ExplicitTypes::AllCtors&&, true, false>();

        test_pair_const<ImplicitTypes::MoveOnly, ImplicitTypes::MoveOnly, false>(); // copy construction
        test_pair_const<ImplicitTypes::MoveOnly, ImplicitTypes::MoveOnly&, false>();
        test_pair_const<ImplicitTypes::MoveOnly, ImplicitTypes::MoveOnly&&, false>();

        test_pair_const<ExplicitTypes::MoveOnly, ExplicitTypes::MoveOnly, false>(); // copy construction
        test_pair_const<ExplicitTypes::MoveOnly, ExplicitTypes::MoveOnly&, false>();
        test_pair_const<ExplicitTypes::MoveOnly, ExplicitTypes::MoveOnly&&, false>();

        test_pair_const<ImplicitTypes::CopyOnly, ImplicitTypes::CopyOnly>();
        test_pair_const<ImplicitTypes::CopyOnly, ImplicitTypes::CopyOnly&>();
        test_pair_const<ImplicitTypes::CopyOnly, ImplicitTypes::CopyOnly&&>();

        test_pair_const<ExplicitTypes::CopyOnly, ExplicitTypes::CopyOnly>();
        test_pair_const<ExplicitTypes::CopyOnly, ExplicitTypes::CopyOnly&, true, false>();
        test_pair_const<ExplicitTypes::CopyOnly, ExplicitTypes::CopyOnly&&, true, false>();

        test_pair_const<ImplicitTypes::NonCopyable, ImplicitTypes::NonCopyable, false>();
        test_pair_const<ImplicitTypes::NonCopyable, ImplicitTypes::NonCopyable&, false>();
        test_pair_const<ImplicitTypes::NonCopyable, ImplicitTypes::NonCopyable&&, false>();
        test_pair_const<ImplicitTypes::NonCopyable, const ImplicitTypes::NonCopyable&, false>();
        test_pair_const<ImplicitTypes::NonCopyable, const ImplicitTypes::NonCopyable&&, false>();
    }

    { // Test construction of references
        test_pair_const<ImplicitTypes::NonCopyable&, ImplicitTypes::NonCopyable&>();
        test_pair_const<ImplicitTypes::NonCopyable&, ImplicitTypes::NonCopyable&&>();
        test_pair_const<ImplicitTypes::NonCopyable&, ImplicitTypes::NonCopyable const&, false>();
        test_pair_const<ImplicitTypes::NonCopyable const&, ImplicitTypes::NonCopyable&&>();
        test_pair_const<ImplicitTypes::NonCopyable&&, ImplicitTypes::NonCopyable&&, false>();

        test_pair_const<ImplicitTypes::ConvertingType&, int, false>();
        test_pair_const<ExplicitTypes::ConvertingType&, int, false>();
        // Unfortunately the below conversions are allowed and create dangling
        // references.
        //test_pair_const<ConvertingType&&, int>();
        //test_pair_const<ConvertingType const&, int>();
        //test_pair_const<ConvertingType const&&, int>();
        // But these are not because the converting constructor is explicit.
        test_pair_const<ExplicitTypes::ConvertingType&&, int, false>();
        test_pair_const<ExplicitTypes::ConvertingType const&, int, false>();
        test_pair_const<ExplicitTypes::ConvertingType const&&, int, false>();

    }

    {
        test_pair_const<ImplicitTypes::AllCtors, int, false>();
        test_pair_const<ExplicitTypes::AllCtors, int, false>();
        test_pair_const<ImplicitTypes::ConvertingType, int>();
        test_pair_const<ExplicitTypes::ConvertingType, int, true, false>();

        test_pair_const<ImplicitTypes::ConvertingType, int>();
        test_pair_const<ImplicitTypes::ConvertingType, ImplicitTypes::ConvertingType>();
        test_pair_const<ImplicitTypes::ConvertingType, ImplicitTypes::ConvertingType const&>();
        test_pair_const<ImplicitTypes::ConvertingType, ImplicitTypes::ConvertingType&>();
        test_pair_const<ImplicitTypes::ConvertingType, ImplicitTypes::ConvertingType&&>();

        test_pair_const<ExplicitTypes::ConvertingType, int, true, false>();
        test_pair_const<ExplicitTypes::ConvertingType, int&, true, false>();
        test_pair_const<ExplicitTypes::ConvertingType, const int&, true, false>();
        test_pair_const<ExplicitTypes::ConvertingType, int&&, true, false>();
        test_pair_const<ExplicitTypes::ConvertingType, const int&&, true, false>();

        test_pair_const<ExplicitTypes::ConvertingType, ExplicitTypes::ConvertingType>();
        test_pair_const<ExplicitTypes::ConvertingType, ExplicitTypes::ConvertingType const&, true, false>();
        test_pair_const<ExplicitTypes::ConvertingType, ExplicitTypes::ConvertingType&, true, false>();
        test_pair_const<ExplicitTypes::ConvertingType, ExplicitTypes::ConvertingType&&, true, false>();
    }

    {
        typedef cruft::tight_pair<int, short> P1;
        typedef cruft::tight_pair<double, long> P2;
        constexpr P1 p1(3, 4);
        constexpr P2 p2 = p1;
        static_assert(get<0>(p2) == 3);
        static_assert(get<1>(p2) == 4);
    }

    {
        using P1 = cruft::tight_pair<int, int>;
        using P2 = cruft::tight_pair<ExplicitT, ExplicitT>;
        constexpr P1 p1(42, 101);
        constexpr P2 p2(p1);
        static_assert(get<0>(p2).value == 42);
        static_assert(get<1>(p2).value == 101);
    }

    {
        using P1 = cruft::tight_pair<int, int>;
        using P2 = cruft::tight_pair<ImplicitT, ImplicitT>;
        constexpr P1 p1(42, 101);
        constexpr P2 p2 = p1;
        static_assert(get<0>(p2).value == 42);
        static_assert(get<1>(p2).value == 101);
    }
}

TEST_CASE( "const pair U C noexcept" )
{
     { // explicit noexcept test
         static_assert(not std::is_nothrow_constructible_v<cruft::tight_pair<ExplicitT, ExplicitT>,
                                                           cruft::tight_pair<int, int> const&>);
         static_assert(not std::is_nothrow_constructible_v<cruft::tight_pair<ExplicitNothrowT, ExplicitT>,
                                                           cruft::tight_pair<int, int> const&>);
         static_assert(not std::is_nothrow_constructible_v<cruft::tight_pair<ExplicitT, ExplicitNothrowT>,
                                                           cruft::tight_pair<int, int> const&>);
         static_assert(    std::is_nothrow_constructible_v<cruft::tight_pair<ExplicitNothrowT, ExplicitNothrowT>,
                                                           cruft::tight_pair<int, int> const&>);
     }
     { // implicit noexcept test
         static_assert(not std::is_nothrow_constructible_v<cruft::tight_pair<ImplicitT, ImplicitT>,
                                                           cruft::tight_pair<int, int> const&>);
         static_assert(not std::is_nothrow_constructible_v<cruft::tight_pair<ImplicitNothrowT, ImplicitT>,
                                                           cruft::tight_pair<int, int> const&>);
         static_assert(not std::is_nothrow_constructible_v<cruft::tight_pair<ImplicitT, ImplicitNothrowT>,
                                                           cruft::tight_pair<int, int> const&>);
         static_assert(    std::is_nothrow_constructible_v<cruft::tight_pair<ImplicitNothrowT, ImplicitNothrowT>,
                                                           cruft::tight_pair<int, int> const&>);
     }
}

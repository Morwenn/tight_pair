/*
 * Copyright (c) 2017-2021 Morwenn
 * SPDX-License-Identifier: MIT
 */

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>
#include "archetypes.h"
#include "test_convertible.h"

namespace
{
    using namespace ImplicitTypes; // Get implicitly archetypes

    template<class T1, class U1, bool CanCopy = true, bool CanConvert = CanCopy>
    constexpr void test_pair_const() {
        using P1 = cruft::tight_pair<T1, int>;
        using P2 = cruft::tight_pair<int, T1>;
        using UP1 = cruft::tight_pair<U1, int> const&;
        using UP2 = cruft::tight_pair<int, U1> const&;
        static_assert(std::is_constructible<P1, UP1>::value == CanCopy);
        static_assert(test_convertible<P1, UP1>() == CanConvert);
        static_assert(std::is_constructible<P2, UP2>::value == CanCopy);
        static_assert(test_convertible<P2, UP2>() == CanConvert);
    }

    template<class T, class U>
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

    constexpr bool test()
    {
        using cruft::get;
        {
            using P1 = cruft::tight_pair<int, int>;
            using P2 = cruft::tight_pair<double, long>;
            const P1 p1(3, 4);
            const P2 p2 = p1;
            assert(get<0>(p2) == 3);
            assert(get<1>(p2) == 4);
        }
        {
            // We allow derived types to use this constructor
            using P1 = DPair<long, long>;
            using P2 = cruft::tight_pair<int, int>;
            P1 p1(42, 101);
            P2 p2(p1);
            assert(get<0>(p2) == 42);
            assert(get<1>(p2) == 101);
        }
        {
            test_pair_const<AllCtors, AllCtors>(); // copy construction
            test_pair_const<AllCtors, AllCtors&>();
            test_pair_const<AllCtors, AllCtors&&>();
            test_pair_const<AllCtors, const AllCtors&>();
            test_pair_const<AllCtors, const AllCtors&&>();

            test_pair_const<ExplicitTypes::AllCtors,
            ExplicitTypes::AllCtors>(); // copy construction
            test_pair_const<ExplicitTypes::AllCtors, ExplicitTypes::AllCtors&, true, false>();
            test_pair_const<ExplicitTypes::AllCtors, ExplicitTypes::AllCtors&&, true, false>();
            test_pair_const<ExplicitTypes::AllCtors, const ExplicitTypes::AllCtors&, true, false>();
            test_pair_const<ExplicitTypes::AllCtors, const ExplicitTypes::AllCtors&&, true, false>();

            test_pair_const<MoveOnly, MoveOnly, false>(); // copy construction
            test_pair_const<MoveOnly, MoveOnly&, false>();
            test_pair_const<MoveOnly, MoveOnly&&, false>();

            test_pair_const<ExplicitTypes::MoveOnly, ExplicitTypes::MoveOnly, false>(); // copy construction
            test_pair_const<ExplicitTypes::MoveOnly, ExplicitTypes::MoveOnly&, false>();
            test_pair_const<ExplicitTypes::MoveOnly, ExplicitTypes::MoveOnly&&, false>();

            test_pair_const<CopyOnly, CopyOnly>();
            test_pair_const<CopyOnly, CopyOnly&>();
            test_pair_const<CopyOnly, CopyOnly&&>();

            test_pair_const<ExplicitTypes::CopyOnly, ExplicitTypes::CopyOnly>();
            test_pair_const<ExplicitTypes::CopyOnly, ExplicitTypes::CopyOnly&, true, false>();
            test_pair_const<ExplicitTypes::CopyOnly, ExplicitTypes::CopyOnly&&, true, false>();

            test_pair_const<NonCopyable, NonCopyable, false>();
            test_pair_const<NonCopyable, NonCopyable&, false>();
            test_pair_const<NonCopyable, NonCopyable&&, false>();
            test_pair_const<NonCopyable, const NonCopyable&, false>();
            test_pair_const<NonCopyable, const NonCopyable&&, false>();
        }

        { // Test construction of references
            test_pair_const<NonCopyable&, NonCopyable&>();
            test_pair_const<NonCopyable&, NonCopyable&&>();
            test_pair_const<NonCopyable&, NonCopyable const&, false>();
            test_pair_const<NonCopyable const&, NonCopyable&&>();
            test_pair_const<NonCopyable&&, NonCopyable&&, false>();

            test_pair_const<ConvertingType&, int, false>();
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
            test_pair_const<AllCtors, int, false>();
            test_pair_const<ExplicitTypes::AllCtors, int, false>();
            test_pair_const<ConvertingType, int>();
            test_pair_const<ExplicitTypes::ConvertingType, int, true, false>();

            test_pair_const<ConvertingType, int>();
            test_pair_const<ConvertingType, ConvertingType>();
            test_pair_const<ConvertingType, ConvertingType const&>();
            test_pair_const<ConvertingType, ConvertingType&>();
            test_pair_const<ConvertingType, ConvertingType&&>();

            test_pair_const<ExplicitTypes::ConvertingType, int, true, false>();
            test_pair_const<ExplicitTypes::ConvertingType, int&, true, false>();
            test_pair_const<ExplicitTypes::ConvertingType, const int&, true, false>();
            test_pair_const<ExplicitTypes::ConvertingType, int&&, true, false>();
            test_pair_const<ExplicitTypes::ConvertingType, const int&&, true, false>();

            test_pair_const<ExplicitTypes::ConvertingType,
            ExplicitTypes::ConvertingType>();
            test_pair_const<ExplicitTypes::ConvertingType,
            ExplicitTypes::ConvertingType const&, true, false>();
            test_pair_const<ExplicitTypes::ConvertingType,
            ExplicitTypes::ConvertingType&, true, false>();
            test_pair_const<ExplicitTypes::ConvertingType,
            ExplicitTypes::ConvertingType&&, true, false>();
        }

        {
            using P1 = cruft::tight_pair<int, int>;
            using P2 = cruft::tight_pair<double, long>;
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

        return true;
    }
}

TEST_CASE( "const pair U C" )
{
    static_assert(test());
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

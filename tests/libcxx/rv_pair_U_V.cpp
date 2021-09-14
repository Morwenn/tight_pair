/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <memory>
#include <type_traits>
#include <utility>
#include <catch2/catch.hpp>
#include <tight_pair.h>
#include "archetypes.h"
#include "test_convertible.h"

using namespace ImplicitTypes; // Get implicitly archetypes

namespace
{
    template<
        typename T1,
        typename U1,
        bool CanCopy = true,
        bool CanConvert = CanCopy
    >
    void test_pair_rv()
    {
        using P1 = cruft::tight_pair<T1, int>;
        using P2 = cruft::tight_pair<int, T1>;
        using UP1 = cruft::tight_pair<U1, int>&&;
        using UP2 = cruft::tight_pair<int, U1>&&;
        static_assert(std::is_constructible_v<P1, UP1> == CanCopy);
        static_assert(test_convertible<P1, UP1>() == CanConvert);
        static_assert(std::is_constructible_v<P2, UP2> == CanCopy);
        static_assert(test_convertible<P2,  UP2>() == CanConvert);
    }

    struct Base
    {
        virtual ~Base() {}
    };

    struct Derived:
        Base
    {};


    template<typename T, typename U>
    struct DPair:
        public cruft::tight_pair<T, U>
    {
        using Base = cruft::tight_pair<T, U>;
        using Base::Base;
    };

    struct ExplicitT
    {
        constexpr explicit ExplicitT(int x):
            value(x)
        {}
        int value;
    };

    struct ImplicitT
    {
        constexpr ImplicitT(int x):
            value(x)
        {}
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

    struct NotCopyOrMoveConstructible
    {
        NotCopyOrMoveConstructible() = default;
        NotCopyOrMoveConstructible(NotCopyOrMoveConstructible const&) = delete;
        NotCopyOrMoveConstructible(NotCopyOrMoveConstructible&&) = delete;
    };

    struct NonCopyConstructible
    {
        NonCopyConstructible(NonCopyConstructible const&) = delete;
        NonCopyConstructible(NonCopyConstructible&&) = default;
    };
}

TEST_CASE( "rv pair U V" )
{
    using cruft::get;

    {
        using P1 = cruft::tight_pair<std::unique_ptr<Derived>, short>;
        using P2 = cruft::tight_pair<std::unique_ptr<Base>, long>;
        P1 p1(std::unique_ptr<Derived>(), 4);
        P2 p2 = std::move(p1);
        CHECK(get<0>(p2) == nullptr);
        CHECK(get<1>(p2) == 4);
    }
    {
        // We allow derived types to use this constructor
        using P1 = DPair<long, long>;
        using P2 = cruft::tight_pair<int, int>;
        P1 p1(42, 101);
        P2 p2(std::move(p1));
        CHECK(get<0>(p2) == 42);
        CHECK(get<1>(p2) == 101);
    }
    {
        test_pair_rv<AllCtors, AllCtors>();
        test_pair_rv<AllCtors, AllCtors&>();
        test_pair_rv<AllCtors, AllCtors&&>();
        test_pair_rv<AllCtors, const AllCtors&>();
        test_pair_rv<AllCtors, const AllCtors&&>();

        test_pair_rv<ExplicitTypes::AllCtors, ExplicitTypes::AllCtors>();
        test_pair_rv<ExplicitTypes::AllCtors, ExplicitTypes::AllCtors&, true, false>();
        test_pair_rv<ExplicitTypes::AllCtors, ExplicitTypes::AllCtors&&, true, false>();
        test_pair_rv<ExplicitTypes::AllCtors, const ExplicitTypes::AllCtors&, true, false>();
        test_pair_rv<ExplicitTypes::AllCtors, const ExplicitTypes::AllCtors&&, true, false>();

        test_pair_rv<MoveOnly, MoveOnly>();
        test_pair_rv<MoveOnly, MoveOnly&, false>();
        test_pair_rv<MoveOnly, MoveOnly&&>();

        test_pair_rv<ExplicitTypes::MoveOnly, ExplicitTypes::MoveOnly>(); // copy construction
        test_pair_rv<ExplicitTypes::MoveOnly, ExplicitTypes::MoveOnly&, false>();
        test_pair_rv<ExplicitTypes::MoveOnly, ExplicitTypes::MoveOnly&&, true, false>();

        test_pair_rv<CopyOnly, CopyOnly>();
        test_pair_rv<CopyOnly, CopyOnly&>();
        test_pair_rv<CopyOnly, CopyOnly&&>();

        test_pair_rv<ExplicitTypes::CopyOnly, ExplicitTypes::CopyOnly>();
        test_pair_rv<ExplicitTypes::CopyOnly, ExplicitTypes::CopyOnly&, true, false>();
        test_pair_rv<ExplicitTypes::CopyOnly, ExplicitTypes::CopyOnly&&, true, false>();

        test_pair_rv<NonCopyable, NonCopyable, false>();
        test_pair_rv<NonCopyable, NonCopyable&, false>();
        test_pair_rv<NonCopyable, NonCopyable&&, false>();
        test_pair_rv<NonCopyable, const NonCopyable&, false>();
        test_pair_rv<NonCopyable, const NonCopyable&&, false>();
    }
    { // Test construction of references
        test_pair_rv<NonCopyable&, NonCopyable&>();
        test_pair_rv<NonCopyable&, NonCopyable&&>();
        test_pair_rv<NonCopyable&, NonCopyable const&, false>();
        test_pair_rv<NonCopyable const&, NonCopyable&&>();
        test_pair_rv<NonCopyable&&, NonCopyable&&>();

        test_pair_rv<ConvertingType&, int, false>();
        test_pair_rv<ExplicitTypes::ConvertingType&, int, false>();
        // Unfortunately the below conversions are allowed and create dangling
        // references.
        //test_pair_rv<ConvertingType&&, int>();
        //test_pair_rv<ConvertingType const&, int>();
        //test_pair_rv<ConvertingType const&&, int>();
        // But these are not because the converting constructor is explicit.
        test_pair_rv<ExplicitTypes::ConvertingType&&, int, false>();
        test_pair_rv<ExplicitTypes::ConvertingType const&, int, false>();
        test_pair_rv<ExplicitTypes::ConvertingType const&&, int, false>();
    }
    {
        test_pair_rv<AllCtors, int, false>();
        test_pair_rv<ExplicitTypes::AllCtors, int, false>();
        test_pair_rv<ConvertingType, int>();
        test_pair_rv<ExplicitTypes::ConvertingType, int, true, false>();

        test_pair_rv<ConvertingType, int>();
        test_pair_rv<ConvertingType, ConvertingType>();
        test_pair_rv<ConvertingType, ConvertingType const&>();
        test_pair_rv<ConvertingType, ConvertingType&>();
        test_pair_rv<ConvertingType, ConvertingType&&>();

        test_pair_rv<ExplicitTypes::ConvertingType, int, true, false>();
        test_pair_rv<ExplicitTypes::ConvertingType, int&, true, false>();
        test_pair_rv<ExplicitTypes::ConvertingType, const int&, true, false>();
        test_pair_rv<ExplicitTypes::ConvertingType, int&&, true, false>();
        test_pair_rv<ExplicitTypes::ConvertingType, const int&&, true, false>();

        test_pair_rv<ExplicitTypes::ConvertingType, ExplicitTypes::ConvertingType>();
        test_pair_rv<ExplicitTypes::ConvertingType, ExplicitTypes::ConvertingType const&, true, false>();
        test_pair_rv<ExplicitTypes::ConvertingType, ExplicitTypes::ConvertingType&, true, false>();
        test_pair_rv<ExplicitTypes::ConvertingType, ExplicitTypes::ConvertingType&&, true, false>();
    }
    {
        // When constructing a pair containing a reference, we only bind the
        // reference, so it doesn't matter whether the type is or isn't
        // copy/move constructible.
        {
            using P1 = cruft::tight_pair<NotCopyOrMoveConstructible&, long>;
            using P2 = cruft::tight_pair<NotCopyOrMoveConstructible&, int>;
            static_assert(std::is_constructible<P1, P2&&>::value);

            NotCopyOrMoveConstructible obj;
            P2 p2{obj, 3};
            P1 p1(std::move(p2));
            CHECK(&get<0>(p1) == &obj);
            CHECK(&get<0>(p2) == &obj);
        }
        {
            using P1 = cruft::tight_pair<NotCopyOrMoveConstructible&&, long>;
            using P2 = cruft::tight_pair<NotCopyOrMoveConstructible&&, int>;
            static_assert(std::is_constructible<P1, P2&&>::value);

            NotCopyOrMoveConstructible obj;
            P2 p2{std::move(obj), 3};
            P1 p1(std::move(p2));
            CHECK(&get<0>(p1) == &obj);
            CHECK(&get<0>(p2) == &obj);
        }
    }
    {
        // Make sure we can't move-construct from a pair containing a reference
        // if that type isn't copy-constructible (since otherwise we'd be stealing
        // the object through the reference).
        using P1 = cruft::tight_pair<NonCopyConstructible, long>;
        using P2 = cruft::tight_pair<NonCopyConstructible&, int>;
        static_assert(not std::is_constructible<P1, P2&&>::value);
    }
    { // explicit constexpr test
        constexpr cruft::tight_pair<int, int> p1(42, 43);
        constexpr cruft::tight_pair<ExplicitT, ExplicitT> p2(std::move(p1));
        static_assert(get<0>(p2).value == 42);
        static_assert(get<1>(p2).value == 43);
    }
    { // implicit constexpr test
        constexpr cruft::tight_pair<int, int> p1(42, 43);
        constexpr cruft::tight_pair<ImplicitT, ImplicitT> p2 = std::move(p1);
        static_assert(get<0>(p2).value == 42);
        static_assert(get<1>(p2).value == 43);
    }
}

TEST_CASE( "rv pair U V noexcept" )
{
     { // explicit noexcept test
         static_assert(not std::is_nothrow_constructible_v<cruft::tight_pair<ExplicitT, ExplicitT>,
                                                           cruft::tight_pair<int, int>&&>);
         static_assert(not std::is_nothrow_constructible_v<cruft::tight_pair<ExplicitNothrowT, ExplicitT>,
                                                           cruft::tight_pair<int, int>&&>);
         static_assert(not std::is_nothrow_constructible_v<cruft::tight_pair<ExplicitT, ExplicitNothrowT>,
                                                           cruft::tight_pair<int, int>&&>);
         static_assert( std::is_nothrow_constructible_v<cruft::tight_pair<ExplicitNothrowT, ExplicitNothrowT>,
                                                        cruft::tight_pair<int, int>&&>);
     }
     { // implicit noexcept test
         static_assert(not std::is_nothrow_constructible_v<cruft::tight_pair<ImplicitT, ImplicitT>,
                                                           cruft::tight_pair<int, int>&&>);
         static_assert(not std::is_nothrow_constructible_v<cruft::tight_pair<ImplicitNothrowT, ImplicitT>,
                                                           cruft::tight_pair<int, int>&&>);
         static_assert(not std::is_nothrow_constructible_v<cruft::tight_pair<ImplicitT, ImplicitNothrowT>,
                                                           cruft::tight_pair<int, int>&&>);
         static_assert( std::is_nothrow_constructible_v<cruft::tight_pair<ImplicitNothrowT, ImplicitNothrowT>,
                                                        cruft::tight_pair<int, int>&&>);
     }
}

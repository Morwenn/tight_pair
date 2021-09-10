/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <memory>
#include <type_traits>
#include <utility>
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    struct NonAssignable
    {
        NonAssignable& operator=(NonAssignable const&) = delete;
        NonAssignable& operator=(NonAssignable&&) = delete;
    };
    struct CopyAssignable
    {
        CopyAssignable() = default;
        CopyAssignable& operator=(CopyAssignable const&) = default;
        CopyAssignable& operator=(CopyAssignable&&) = delete;
    };
    struct MoveAssignable
    {
        MoveAssignable() = default;
        MoveAssignable& operator=(MoveAssignable const&) = delete;
        MoveAssignable& operator=(MoveAssignable&&) = default;
    };

    struct CountAssign
    {
        static int copied;
        static int moved;
        static void reset() { copied = moved = 0; }
        CountAssign() = default;
        CountAssign& operator=(CountAssign const&) { ++copied; return *this; }
        CountAssign& operator=(CountAssign&&) { ++moved; return *this; }
    };
    int CountAssign::copied = 0;
    int CountAssign::moved = 0;
}

TEST_CASE( "assign rv pair" )
{
    using cruft::get;

    {
        using P = cruft::tight_pair<std::unique_ptr<int>, short>;
        P p1(std::unique_ptr<int>(new int(3)), 4);
        P p2;
        p2 = std::move(p1);
        CHECK(*get<0>(p2) == 3);
        CHECK(get<1>(p2) == 4);
    }
    {
        using P = cruft::tight_pair<int&, int&&>;
        int x = 42;
        int y = 101;
        int x2 = -1;
        int y2 = 300;
        P p1(x, std::move(y));
        P p2(x2, std::move(y2));
        p1 = std::move(p2);
        CHECK(get<0>(p1) == x2);
        CHECK(get<1>(p1) == y2);
    }
    {
        using P = cruft::tight_pair<int, NonAssignable>;
        static_assert(not std::is_move_assignable_v<P>);
    }
    {
        // The move decays to the copy constructor
        CountAssign::reset();
        using P = cruft::tight_pair<CountAssign, CopyAssignable>;
        static_assert(std::is_move_assignable_v<P>);
        P p;
        P p2;
        p = std::move(p2);
        CHECK(CountAssign::moved == 0);
        CHECK(CountAssign::copied == 1);
    }
    {
        CountAssign::reset();
        using P = cruft::tight_pair<CountAssign, MoveAssignable>;
        static_assert(std::is_move_assignable_v<P>);
        P p;
        P p2;
        p = std::move(p2);
        CHECK(CountAssign::moved == 1);
        CHECK(CountAssign::copied == 0);
    }
}

/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
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
        CopyAssignable(CopyAssignable const&) = default;
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

struct Incomplete;
extern Incomplete inc_obj;

TEST_CASE( "assign pair" )
{
    using cruft::get;

    {
        using P = cruft::tight_pair<CopyAssignable, short>;
        const P p1(CopyAssignable(), 4);
        P p2;
        p2 = p1;
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
        p1 = p2;
        CHECK(get<0>(p1) == x2);
        CHECK(get<1>(p1) == y2);
    }
    {
        using P = cruft::tight_pair<int, NonAssignable>;
        static_assert(not std::is_copy_assignable_v<P>);
    }
    {
        CountAssign::reset();
        using P = cruft::tight_pair<CountAssign, CopyAssignable>;
        static_assert(std::is_copy_assignable_v<P>);
        P p;
        P p2;
        p = p2;
        CHECK(CountAssign::copied == 1);
        CHECK(CountAssign::moved == 0);
    }
    {
        using P = cruft::tight_pair<int, MoveAssignable>;
        static_assert(not std::is_copy_assignable_v<P>);
    }
    {
        using P = cruft::tight_pair<int, Incomplete&>;
        static_assert(not std::is_copy_assignable_v<P>);
        P p(42, inc_obj);
        CHECK(&get<1>(p) == &inc_obj);
    }
}

struct Incomplete {};
Incomplete inc_obj;

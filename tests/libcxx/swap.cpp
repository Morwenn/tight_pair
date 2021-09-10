/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <cassert>
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    struct S
    {
        int i;
        S(): i(0) {}
        S(int j): i(j) {}
        S* operator&() { assert(false); return this; }
        S const* operator&() const { assert(false); return this; }
        bool operator==(int x) const { return i == x; }
    };
}

TEST_CASE( "swap" )
{
    using cruft::get;

    {
        using P1 = cruft::tight_pair<int, short>;
        P1 p1(3, static_cast<short>(4));
        P1 p2(5, static_cast<short>(6));
        p1.swap(p2);
        CHECK(get<0>(p1) == 5);
        CHECK(get<1>(p1) == 6);
        CHECK(get<0>(p2) == 3);
        CHECK(get<1>(p2) == 4);
    }

    {
        using P1 = cruft::tight_pair<int, S>;
        P1 p1(3, S(4));
        P1 p2(5, S(6));
        p1.swap(p2);
        CHECK(get<0>(p1) == 5);
        CHECK(get<1>(p1) == 6);
        CHECK(get<0>(p2) == 3);
        CHECK(get<1>(p2) == 4);
    }
}

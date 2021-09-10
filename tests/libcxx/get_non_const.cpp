/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    struct S
    {
        cruft::tight_pair<int, int> a;
        int k;
        constexpr S(): a{1, 2}, k(cruft::get<0>(a)) {}
    };

    constexpr cruft::tight_pair<int, int> getP() { return { 3, 4 }; }
}

TEST_CASE( "get non const" )
{
    {
        using P = cruft::tight_pair<int, short>;
        P p(3, static_cast<short>(4));
        CHECK(cruft::get<0>(p) == 3);
        CHECK(cruft::get<1>(p) == 4);
        cruft::get<0>(p) = 5;
        cruft::get<1>(p) = 6;
        CHECK(cruft::get<0>(p) == 5);
        CHECK(cruft::get<1>(p) == 6);
    }

    {
        static_assert(S().k == 1);
        static_assert(cruft::get<1>(getP()) == 4);
    }
}

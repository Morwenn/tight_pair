/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "non member swap" )
{
    using cruft::get;
    {
        using P1 = std::pair<int, short>;
        P1 p1(3, static_cast<short>(4));
        P1 p2(5, static_cast<short>(6));
        swap(p1, p2);
        CHECK(get<0>(p1)== 5);
        CHECK(get<1>(p1) == 6);
        CHECK(get<0>(p2) == 3);
        CHECK(get<1>(p2) == 4);
    }
}

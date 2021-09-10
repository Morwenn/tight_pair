/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "assign const pair U V" )
{
    using cruft::get;

    using P1 = cruft::tight_pair<int, short>;
    using P2 = cruft::tight_pair<double, long>;
    constexpr P1 p1(3, static_cast<short>(4));
    P2 p2;
    p2 = p1;
    CHECK(get<0>(p2) == 3);
    CHECK(get<1>(p2) == 4);
}

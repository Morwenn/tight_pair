/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "copy constructor tests" )
{
    using cruft::get;

    using P1 = cruft::tight_pair<int, short>;
    constexpr P1 p1(3, static_cast<short>(4));
    constexpr P1 p2 = p1;
    CHECK(get<0>(p2) == 3);
    CHECK(get<1>(p2)== 4);
    static_assert(get<0>(p2) == 3);
    static_assert(get<1>(p2) == 4);
}

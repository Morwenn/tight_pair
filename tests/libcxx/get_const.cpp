/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "get const" )
{
    {
        using P = cruft::tight_pair<int, short>;
        const P p(3, static_cast<short>(4));
        CHECK(cruft::get<0>(p) == 3);
        CHECK(cruft::get<1>(p) == 4);
    }

    {
        using P = cruft::tight_pair<int, short>;
        constexpr P p1(3, static_cast<short>(4));
        static_assert(cruft::get<0>(p1) == 3);
        static_assert(cruft::get<1>(p1) == 4);
    }
}

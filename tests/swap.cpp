/*
 * Copyright (c) 2021 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <catch2/catch.hpp>
#include <tight_pair.h>

constexpr auto test_swap()
    -> bool
{
    cruft::tight_pair<int, bool> p1(5, false);
    cruft::tight_pair<int, bool> p2(8, true);
    swap(p1, p2);

    using cruft::get;
    return get<0>(p1) == 8
        && get<1>(p1) == true
        && get<0>(p2) == 5
        && get<1>(p2) == false;
}

TEST_CASE( "test constexpr swap (issue #15)" )
{
    constexpr bool res = test_swap();
    CHECK( res );
    static_assert(res);
}

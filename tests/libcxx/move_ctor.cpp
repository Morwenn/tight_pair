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
    struct Dummy
    {
        Dummy(Dummy const&) = delete;
        Dummy(Dummy&&) = default;
    };
}

TEST_CASE( "move constructor tests" )
{
    using cruft::get;

    using P1 = cruft::tight_pair<int, short>;
    static_assert(std::is_move_constructible_v<P1>);
    P1 p1(3, static_cast<short>(4));
    P1 p2 = std::move(p1);
    CHECK(get<0>(p2) == 3);
    CHECK(get<1>(p2) == 4);

    using P = cruft::tight_pair<Dummy, int>;
    static_assert(not std::is_copy_constructible_v<P>);
    static_assert(std::is_move_constructible_v<P>);
}

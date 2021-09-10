/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <type_traits>
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

TEST_CASE( "trivial copy move" )
{
    using P = cruft::tight_pair<int, short>;
    {
        static_assert(std::is_copy_constructible_v<P>);
        static_assert(std::is_trivially_copy_constructible_v<P>);
    }

    {
        static_assert(std::is_move_constructible_v<P>);
        static_assert(std::is_trivially_move_constructible_v<P>);
    }

    {
        using P1 = cruft::tight_pair<Dummy, int>;
        static_assert(not std::is_copy_constructible_v<P1>);
        static_assert(not std::is_trivially_copy_constructible_v<P1>);
        static_assert(std::is_move_constructible_v<P1>);
        static_assert(std::is_trivially_move_constructible_v<P1>);
    }
}

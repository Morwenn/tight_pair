/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    // Empty class with get<I> function, which is picked instead
    // of ADL get with some std::tuple implementations

    struct foobar
    {
        template<std::size_t N>
        int get() { return N; }
    };
}

TEST_CASE( "test structured bindings with base class get" )
{
    cruft::tight_pair<int, foobar> tu(19, {});
    auto [a, b] = tu;
    CHECK(a == 19);
}

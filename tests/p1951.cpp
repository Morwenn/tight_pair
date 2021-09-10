/*
 * Copyright (c) 2020 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    struct foo
    {
        bool construction_is_optimized;

        foo(std::string const&):
            construction_is_optimized(true)
        {}

        foo(foo const&):
            construction_is_optimized(false)
        {}
    };

    struct bar
    {
        bar() = default;
        bar(bar const&) = default;
    };
}

TEST_CASE( "P1951" )
{
    using cruft::get;

    cruft::tight_pair<foo, bar> p("hello", {});
    CHECK(get<0>(p).construction_is_optimized);
}

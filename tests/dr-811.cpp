/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "DR 811" )
{
    using cruft::get;

    cruft::tight_pair<char*, char*> p(0, 0);
    CHECK(get<0>(p) == nullptr);
    CHECK(get<1>(p) == nullptr);
}

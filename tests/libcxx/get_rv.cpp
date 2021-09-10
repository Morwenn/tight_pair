/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <memory>
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "get rv" )
{
    using P = cruft::tight_pair<std::unique_ptr<int>, short>;
    P p(std::unique_ptr<int>(new int(3)), static_cast<short>(4));
    std::unique_ptr<int> ptr = cruft::get<0>(std::move(p));
    CHECK(*ptr == 3);
}

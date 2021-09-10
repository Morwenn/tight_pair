/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <memory>
#include <utility>
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    struct Base
    {
        virtual ~Base() {}
    };

    struct Derived:
        Base
    {};
}

TEST_CASE( "assign rv pair U V" )
{
    using cruft::get;

    using P1 = cruft::tight_pair<std::unique_ptr<Derived>, short>;
    using P2 = cruft::tight_pair<std::unique_ptr<Base>, long>;
    P1 p1(std::unique_ptr<Derived>(), static_cast<short>(4));
    P2 p2;
    p2 = std::move(p1);
    CHECK(get<0>(p2) == nullptr);
    CHECK(get<1>(p2) == 4);
}

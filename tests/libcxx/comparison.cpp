/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "comparison" )
{
    {
        using P = cruft::tight_pair<int, short>;
        P p1(3, static_cast<short>(4));
        P p2(3, static_cast<short>(4));
        CHECK( (p1 == p2));
        CHECK(!(p1 != p2));
        CHECK(!(p1 <  p2));
        CHECK( (p1 <= p2));
        CHECK(!(p1 >  p2));
        CHECK( (p1 >= p2));
    }
    {
        using P = cruft::tight_pair<int, short>;
        P p1(2, static_cast<short>(4));
        P p2(3, static_cast<short>(4));
        CHECK(!(p1 == p2));
        CHECK( (p1 != p2));
        CHECK( (p1 <  p2));
        CHECK( (p1 <= p2));
        CHECK(!(p1 >  p2));
        CHECK(!(p1 >= p2));
    }
    {
        using P = cruft::tight_pair<int, short>;
        P p1(3, static_cast<short>(2));
        P p2(3, static_cast<short>(4));
        CHECK(!(p1 == p2));
        CHECK( (p1 != p2));
        CHECK( (p1 <  p2));
        CHECK( (p1 <= p2));
        CHECK(!(p1 >  p2));
        CHECK(!(p1 >= p2));
    }
    {
        using P = cruft::tight_pair<int, short>;
        P p1(3, static_cast<short>(4));
        P p2(2, static_cast<short>(4));
        CHECK(!(p1 == p2));
        CHECK( (p1 != p2));
        CHECK(!(p1 <  p2));
        CHECK(!(p1 <= p2));
        CHECK( (p1 >  p2));
        CHECK( (p1 >= p2));
    }
    {
        using P = cruft::tight_pair<int, short>;
        P p1(3, static_cast<short>(4));
        P p2(3, static_cast<short>(2));
        CHECK(!(p1 == p2));
        CHECK( (p1 != p2));
        CHECK(!(p1 <  p2));
        CHECK(!(p1 <= p2));
        CHECK( (p1 >  p2));
        CHECK( (p1 >= p2));
    }
    {
        using P = cruft::tight_pair<int, short>;
        constexpr P p1(3, static_cast<short>(4));
        constexpr P p2(3, static_cast<short>(2));
        static_assert(!(p1 == p2));
        static_assert( (p1 != p2));
        static_assert(!(p1 <  p2));
        static_assert(!(p1 <= p2));
        static_assert( (p1 >  p2));
        static_assert( (p1 >= p2));
    }
}

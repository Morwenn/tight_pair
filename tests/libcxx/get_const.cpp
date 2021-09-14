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

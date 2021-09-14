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

TEST_CASE( "copy constructor tests" )
{
    using cruft::get;

    using P1 = cruft::tight_pair<int, short>;
    constexpr P1 p1(3, static_cast<short>(4));
    constexpr P1 p2 = p1;
    CHECK(get<0>(p2) == 3);
    CHECK(get<1>(p2)== 4);
    static_assert(get<0>(p2) == 3);
    static_assert(get<1>(p2) == 4);
}

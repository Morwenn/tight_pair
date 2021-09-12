/*
 * Copyright (c) 2017-2021 Morwenn
 * SPDX-License-Identifier: MIT
 */

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    struct S
    {
        int i;
        constexpr S(): i(0) {}
        constexpr S(int j): i(j) {}
        constexpr bool operator==(int x) const { return i == x; }
    };

    constexpr bool test()
    {
        using cruft::get;
        {
            using P1 = cruft::tight_pair<int, short>;
            P1 p1(3, static_cast<short>(4));
            P1 p2(5, static_cast<short>(6));
            p1.swap(p2);
            assert(get<0>(p1) == 5);
            assert(get<1>(p1) == 6);
            assert(get<0>(p2) == 3);
            assert(get<1>(p2) == 4);
        }
        {
            using P1 = cruft::tight_pair<int, S>;
            P1 p1(3, S(4));
            P1 p2(5, S(6));
            p1.swap(p2);
            assert(get<0>(p1) == 5);
            assert(get<1>(p1) == 6);
            assert(get<0>(p2) == 3);
            assert(get<1>(p2) == 4);
        }
        return true;
    }
}

TEST_CASE( "swap" )
{
    static_assert(test());
}

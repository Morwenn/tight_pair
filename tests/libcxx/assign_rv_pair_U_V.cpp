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
#include <utility>
#include <catch2/catch.hpp>
#include <tight_pair.h>
#include "archetypes.h"

namespace
{
    struct Derived:
        ConstexprTestTypes::MoveOnly
    {
        Derived() = default;
        constexpr Derived(ConstexprTestTypes::MoveOnly&&) {};
    };

    struct CountAssign
    {
        int copied = 0;
        int moved = 0;
        constexpr CountAssign() = default;
        constexpr CountAssign(const int) {};
        constexpr CountAssign& operator=(CountAssign const&) {
            ++copied;
            return *this;
        }
        constexpr CountAssign& operator=(CountAssign&&) {
            ++moved;
            return *this;
        }
    };

    constexpr bool test()
    {
        using cruft::get;
        {
            using P1 = cruft::tight_pair<Derived, short>;
            using P2 = cruft::tight_pair<ConstexprTestTypes::MoveOnly, long>;
            P1 p1(Derived(), static_cast<short>(4));
            P2 p2;
            p2 = std::move(p1);
            assert(get<1>(p2) == 4);
        }
        {
            using P = cruft::tight_pair<int, CountAssign>;
            using T = cruft::tight_pair<long, CountAssign>;
            T t(42, -42);
            P p(101, 101);
            p = std::move(t);
            assert(get<0>(p) == 42);
            assert(get<1>(p).moved == 1);
            assert(get<1>(p).copied == 0);
            assert(get<1>(t).moved == 0);
            assert(get<1>(t).copied == 0);
        }
        return true;
    }
}

TEST_CASE( "assign rv pair U V" )
{
    static_assert(test());
}

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
#include "archetypes.h"

namespace
{
    struct CopyAssignableInt
    {
        CopyAssignableInt& operator=(int&) { return *this; }
    };

    struct Unrelated {};

    constexpr bool test()
    {
        using cruft::get;
        {
            using P1 = cruft::tight_pair<int, short>;
            using P2 = cruft::tight_pair<double, long>;
            P1 p1(3, static_cast<short>(4));
            P2 p2;
            p2 = p1;
            assert(get<0>(p2) == 3);
            assert(get<1>(p2) == 4);
        }

        {
            using C = ConstexprTestTypes::TestType;
            using P = cruft::tight_pair<int, C>;
            using T = cruft::tight_pair<long, C>;
            const T t(42, -42);
            P p(101, 101);
            p = t;
            assert(get<0>(p) == 42);
            assert(get<1>(p).value == -42);
        }


        { // test const requirement
            using T = cruft::tight_pair<CopyAssignableInt, CopyAssignableInt>;
            using P = cruft::tight_pair<int, int>;
            static_assert(not std::is_assignable<T&, P const>::value);
        }
        {
            using T = cruft::tight_pair<int, Unrelated>;
            using P = cruft::tight_pair<Unrelated, int>;
            static_assert(not std::is_assignable<T&, P&>::value);
            static_assert(not std::is_assignable<P&, T&>::value);
        }
        return true;
    }
}


TEST_CASE( "assign const pair U V" )
{
    static_assert(test());
}

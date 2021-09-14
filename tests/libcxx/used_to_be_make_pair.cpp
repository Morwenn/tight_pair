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

#include <memory>
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "used to be make_pair" )
{
    using cruft::get;
    {
        using P1 = cruft::tight_pair<int, short>;
        P1 p1 = cruft::tight_pair(3, static_cast<short>(4));
        CHECK(get<0>(p1) == 3);
        CHECK(get<1>(p1) == 4);
    }
    {
        using P1 = cruft::tight_pair<std::unique_ptr<int>, short>;
        P1 p1 = cruft::tight_pair(std::unique_ptr<int>(new int(3)), static_cast<short>(4));
        CHECK(*get<0>(p1) == 3);
        CHECK(get<1>(p1) == 4);
    }
    {
        using P1 = cruft::tight_pair<std::unique_ptr<int>, short>;
        P1 p1 = cruft::tight_pair(nullptr, static_cast<short>(4));
        CHECK(get<0>(p1) == nullptr);
        CHECK(get<1>(p1) == 4);
    }
    {
        using P1 = cruft::tight_pair<int, short>;
        constexpr P1 p1 = cruft::tight_pair(3, static_cast<short>(4));
        static_assert(get<0>(p1) == 3, "");
        static_assert(get<1>(p1) == 4, "");
    }
}

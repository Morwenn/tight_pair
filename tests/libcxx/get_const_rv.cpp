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
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "get const rv" )
{
    {
        using P = cruft::tight_pair<std::unique_ptr<int>, short>;
        const P p(std::unique_ptr<int>(new int(3)), static_cast<short>(4));
        static_assert(std::is_same_v<const std::unique_ptr<int>&&, decltype(cruft::get<0>(std::move(p)))>);
        static_assert(noexcept(cruft::get<0>(std::move(p))));
        const std::unique_ptr<int>&& ptr = cruft::get<0>(std::move(p));
        CHECK(*ptr == 3);
    }

    {
        int x = 42;
        int const y = 43;
        cruft::tight_pair<int&, int const&> const p(x, y);
        static_assert(std::is_same_v<int&, decltype(cruft::get<0>(std::move(p)))>);
        static_assert(noexcept(cruft::get<0>(std::move(p))));
        static_assert(std::is_same_v<int const&, decltype(cruft::get<1>(std::move(p)))>);
        static_assert(noexcept(cruft::get<1>(std::move(p))));
    }

    {
        int x = 42;
        int const y = 43;
        cruft::tight_pair<int&&, int const&&> const p(std::move(x), std::move(y));
        static_assert(std::is_same_v<int&&, decltype(cruft::get<0>(std::move(p)))>);
        static_assert(noexcept(cruft::get<0>(std::move(p))));
        static_assert(std::is_same_v<int const&&, decltype(cruft::get<1>(std::move(p)))>);
        static_assert(noexcept(cruft::get<1>(std::move(p))));
    }

    {
        using P = cruft::tight_pair<int, short>;
        constexpr const P p1(3, static_cast<short>(4));
        static_assert(cruft::get<0>(std::move(p1)) == 3);
        static_assert(cruft::get<1>(std::move(p1)) == 4);
    }
}

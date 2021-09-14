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

#include <complex>
#include <memory>
#include <utility>
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "pairs by type" )
{
    using cf = std::complex<float>;
    {
        auto t1 = cruft::tight_pair<int, cf> ( 42, { 1,2 } );
        CHECK(cruft::get<int>(t1) == 42);
        CHECK(cruft::get<cf>(t1).real() == 1);
        CHECK(cruft::get<cf>(t1).imag() == 2);
    }

    {
        const cruft::tight_pair<int, const int> p1 { 1, 2 };
        const int& i1 = cruft::get<int>(p1);
        const int& i2 = cruft::get<const int>(p1);
        CHECK(i1 == 1);
        CHECK(i2 == 2);
    }

    {
        using upint = std::unique_ptr<int>;
        cruft::tight_pair<upint, int> t(upint(new int(4)), 42);
        upint p = cruft::get<upint>(std::move(t)); // get rvalue
        CHECK(*p == 4);
        CHECK(cruft::get<upint>(t) == nullptr); // has been moved from
    }

    {
        using upint = std::unique_ptr<int>;
        const cruft::tight_pair<upint, int> t(upint(new int(4)), 42);
        static_assert(std::is_same_v<const upint&&, decltype(cruft::get<upint>(std::move(t)))>);
        static_assert(noexcept(cruft::get<upint>(std::move(t))));
        static_assert(std::is_same_v<const int&&, decltype(cruft::get<int>(std::move(t)))>);
        static_assert(noexcept(cruft::get<int>(std::move(t))));
        auto&& p = cruft::get<upint>(std::move(t)); // get const rvalue
        auto&& i = cruft::get<int>(std::move(t)); // get const rvalue
        CHECK(*p == 4);
        CHECK(i == 42);
        CHECK(cruft::get<upint>(t) != nullptr);
    }

    {
        int x = 42;
        int const y = 43;
        cruft::tight_pair<int&, int const&> const p(x, y);
        static_assert(std::is_same_v<int&, decltype(cruft::get<int&>(std::move(p)))>);
        static_assert(noexcept(cruft::get<int&>(std::move(p))));
        static_assert(std::is_same_v<int const&, decltype(cruft::get<int const&>(std::move(p)))>);
        static_assert(noexcept(cruft::get<int const&>(std::move(p))));
    }

    {
        int x = 42;
        int const y = 43;
        cruft::tight_pair<int&&, int const&&> const p(std::move(x), std::move(y));
        static_assert(std::is_same_v<int&&, decltype(cruft::get<int&&>(std::move(p)))>);
        static_assert(noexcept(cruft::get<int&&>(std::move(p))));
        static_assert(std::is_same_v<int const&&, decltype(cruft::get<int const&&>(std::move(p)))>);
        static_assert(noexcept(cruft::get<int const&&>(std::move(p))));
    }

    {
        constexpr const cruft::tight_pair<int, const int> p { 1, 2 };
        static_assert(cruft::get<int>(std::move(p)) == 1);
        static_assert(cruft::get<const int>(std::move(p)) == 2);
    }
}

/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    struct NoDefault
    {
        NoDefault() = delete;
    };
}

TEST_CASE( "test default constructor" )
{
    using cruft::get;

    {
        using P = cruft::tight_pair<float, short*>;
        P p;
        CHECK(get<0>(p) == 0.0f);
        CHECK(get<1>(p) == nullptr);
    }

    {
        using P = cruft::tight_pair<float, short*>;;
        constexpr P p;
        static_assert(get<0>(p) == 0.0f);
        static_assert(get<1>(p) == nullptr);
    }

    {
        using P = cruft::tight_pair<int, NoDefault>;
        static_assert(not std::is_default_constructible_v<P>);
        using P2 = cruft::tight_pair<NoDefault, int>;
        static_assert(not std::is_default_constructible_v<P2>);
    }

    {
        struct Base {};
        struct Derived: Base { protected: Derived() = default; };
        static_assert(not std::is_default_constructible_v<cruft::tight_pair<Derived, int>>);
    }
}

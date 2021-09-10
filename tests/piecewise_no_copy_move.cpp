/*
 * Copyright (c) 2017-2021 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    struct DummyA
    {
        DummyA(DummyA const&) = delete;
        DummyA(DummyA&&) = delete;

        constexpr DummyA(int a, int b): a(a), b(b) {}

        int a, b;
    };

    struct DummyB
    {
        DummyB(DummyB const&) = delete;
        DummyB(DummyB&&) = delete;

        constexpr DummyB(int a, int b): a(a), b(b) {}

        int a, b;
    };
}

TEST_CASE( "test that piecewise construction works with no copy/move ctor" )
{
    using cruft::get;

    constexpr cruft::tight_pair<DummyA, DummyB> pr1(
        std::piecewise_construct,
        std::forward_as_tuple(1, 2),
        std::forward_as_tuple(3, 4)
    );

    CHECK(get<0>(pr1).a == 1);
    CHECK(get<0>(pr1).b == 2);
    CHECK(get<1>(pr1).a == 3);
    CHECK(get<1>(pr1).b == 4);
    static_assert(get<0>(pr1).a == 1);
    static_assert(get<0>(pr1).b == 2);
    static_assert(get<1>(pr1).a == 3);
    static_assert(get<1>(pr1).b == 4);

#if defined(_MSC_VER) && _MSC_VER >= 1930
    // MSVC has some constexpr bug before version 19.30

    constexpr cruft::tight_pair<DummyA, DummyA> pr2(
        std::piecewise_construct,
        std::forward_as_tuple(1, 2),
        std::forward_as_tuple(3, 4)
    );

    CHECK(get<0>(pr2).a == 1);
    CHECK(get<0>(pr2).b == 2);
    CHECK(get<1>(pr2).a == 3);
    CHECK(get<1>(pr2).b == 4);
    static_assert(get<0>(pr2).a == 1);
    static_assert(get<0>(pr2).b == 2);
    static_assert(get<1>(pr2).a == 3);
    static_assert(get<1>(pr2).b == 4);
#endif // defined
}

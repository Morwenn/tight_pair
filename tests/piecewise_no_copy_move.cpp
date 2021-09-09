/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2021 Morwenn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

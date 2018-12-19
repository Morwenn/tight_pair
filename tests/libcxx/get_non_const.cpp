/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Morwenn
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
    struct S
    {
        cruft::tight_pair<int, int> a;
        int k;
        constexpr S(): a{1, 2}, k(cruft::get<0>(a)) {}
    };

    constexpr cruft::tight_pair<int, int> getP() { return { 3, 4 }; }
}

TEST_CASE( "get non const" )
{
    {
        using P = cruft::tight_pair<int, short>;
        P p(3, static_cast<short>(4));
        CHECK(cruft::get<0>(p) == 3);
        CHECK(cruft::get<1>(p) == 4);
        cruft::get<0>(p) = 5;
        cruft::get<1>(p) = 6;
        CHECK(cruft::get<0>(p) == 5);
        CHECK(cruft::get<1>(p) == 6);
    }

    {
        static_assert(S().k == 1);
        static_assert(cruft::get<1>(getP()) == 4);
    }
}

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

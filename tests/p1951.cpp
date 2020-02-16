/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Morwenn
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
#include <string>
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    struct foo
    {
        bool construction_is_optimized;

        foo(std::string const&):
            construction_is_optimized(true)
        {}

        foo(foo const&):
            construction_is_optimized(false)
        {}
    };

    struct bar
    {
        bar() = default;
        bar(bar const&) = default;
    };
}

TEST_CASE( "P1951" )
{
    using cruft::get;

    cruft::tight_pair<foo, bar> p("hello", {});
    CHECK(get<0>(p).construction_is_optimized);
}

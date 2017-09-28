/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Morwenn
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
#include <complex>
#include <string>
#include <tuple>
#include <utility>
#include <catch.hpp>
#include <tight_pair.h>

TEST_CASE( "test from cppreference.com examples" )
{
    SECTION( "constructor" )
    {
        using namespace std::string_literals;
        using cruft::get;

        // Check value initialization
        cruft::tight_pair<int, float> p1;
        CHECK(get<0>(p1) == 0);
        CHECK(get<1>(p1) == 0.0f);

        // Check initialization with two values
        cruft::tight_pair<int, double> p2(42, 0.123);
        CHECK(get<0>(p2) == 42);
        CHECK(get<1>(p2) == 0.123);

        // Check implicit conversions at construction
        cruft::tight_pair<char, int> p4(p2);
        CHECK(get<0>(p4) == '*');
        CHECK(get<1>(p4) == 0);

        // Piecewise construction
        cruft::tight_pair<std::complex<double>, std::string> p6(
            std::piecewise_construct,
            std::forward_as_tuple(0.123, 7.7),
            std::forward_as_tuple(10, 'a'));
        CHECK(get<0>(p6) == std::complex<double>(0.123, 7.7));
        CHECK(get<1>(p6) == "aaaaaaaaaa"s);
    }
}

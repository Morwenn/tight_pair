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
#include <functional>
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "test std::reference_wrapper unwrapping" )
{
    int a = 5;
    double b = 5.2;

    {
        auto p1 = cruft::tight_pair(std::ref(a), b);
        static_assert(std::is_same_v<std::tuple_element_t<0, decltype(p1)>, int&>);
        static_assert(std::is_same_v<std::tuple_element_t<1, decltype(p1)>, double>);

        auto p2 = cruft::tight_pair(a, std::ref(b));
        static_assert(std::is_same_v<std::tuple_element_t<0, decltype(p2)>, int>);
        static_assert(std::is_same_v<std::tuple_element_t<1, decltype(p2)>, double&>);

        auto p3 = cruft::tight_pair(std::ref(a), std::ref(b));
        static_assert(std::is_same_v<std::tuple_element_t<0, decltype(p3)>, int&>);
        static_assert(std::is_same_v<std::tuple_element_t<1, decltype(p3)>, double&>);
    }

    {
        auto p1 = cruft::tight_pair(std::cref(a), b);
        static_assert(std::is_same_v<std::tuple_element_t<0, decltype(p1)>, int const&>);
        static_assert(std::is_same_v<std::tuple_element_t<1, decltype(p1)>, double>);

        auto p2 = cruft::tight_pair(a, std::cref(b));
        static_assert(std::is_same_v<std::tuple_element_t<0, decltype(p2)>, int>);
        static_assert(std::is_same_v<std::tuple_element_t<1, decltype(p2)>, double const&>);

        auto p3 = cruft::tight_pair(std::cref(a), std::cref(b));
        static_assert(std::is_same_v<std::tuple_element_t<0, decltype(p3)>, int const&>);
        static_assert(std::is_same_v<std::tuple_element_t<1, decltype(p3)>, double const&>);
    }
}

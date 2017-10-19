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
#include <string>
#include <type_traits>
#include <utility>
#include <catch.hpp>
#include <tight_pair.h>
#include "archetypes.h"

TEST_CASE( "test deduction guides" )
{
    // Overloads
    // ---------------
    // (1)  tight_pair(const T1&, const T2&) -> tight_pair<T1, T2>
    // (2)  explicit tight_pair(const T1&, const T2&) -> tight_pair<T1, T2>
    // (3)  tight_pair(tight_pair const& t) -> decltype(t)
    // (4)  tight_pair(tight_pair&& t) -> decltype(t)
    // (5)  tight_pair(tight_pair<U1, U2> const&) -> tight_pair<U1, U2>
    // (6)  explicit tight_pair(tight_pair<U1, U2> const&) -> tight_pair<U1, U2>
    // (7)  tight_pair(tight_pair<U1, U2> &&) -> tight_pair<U1, U2>
    // (8)  explicit tight_pair(tight_pair<U1, U2> &&) -> tight_pair<U1, U2>

    using E = ExplicitTestTypes::TestType;
    static_assert(not std::is_convertible_v<E const&, E>);
    { // Testing (1)
        int const x = 42;
        cruft::tight_pair t1("abc", x);
        static_assert(std::is_same_v<decltype(t1), cruft::tight_pair<const char*, int>>);
    }
    /*{ // Testing (2)
        cruft::tight_pair p1(E{}, 42);
        static_assert(std::is_same_v<decltype(p1), cruft::tight_pair<E, int>>);

        const E t{};
        cruft::tight_pair p2(t, E{});
        static_assert(std::is_same_v<decltype(p2), cruft::tight_pair<E, E>>);
    }*/
    { // Testing (3, 5)
        cruft::tight_pair<double, decltype(nullptr)> const p(0.0, nullptr);
        cruft::tight_pair p1(p);
        static_assert(std::is_same_v<decltype(p1), cruft::tight_pair<double, decltype(nullptr)>>);
    }
    { // Testing (3, 6)
        cruft::tight_pair<E, decltype(nullptr)> const p(E{}, nullptr);
        cruft::tight_pair p1(p);
        static_assert(std::is_same_v<decltype(p1), cruft::tight_pair<E, decltype(nullptr)>>);
    }
    { // Testing (4, 7)
        cruft::tight_pair<std::string, void*> p("abc", nullptr);
        cruft::tight_pair p1(std::move(p));
        static_assert(std::is_same_v<decltype(p1), cruft::tight_pair<std::string, void*>>);
    }
    { // Testing (4, 8)
        cruft::tight_pair<std::string, E> p("abc", E{});
        cruft::tight_pair p1(std::move(p));
        static_assert(std::is_same_v<decltype(p1), cruft::tight_pair<std::string, E>>);
    }
}

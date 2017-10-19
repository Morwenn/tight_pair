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
#include <catch.hpp>
#include <tight_pair.h>

template<typename T1, typename T2>
void test()
{
    {
        using Exp1 = T1;
        using Exp2 = T2;
        using P = cruft::tight_pair<T1, T2>;
        static_assert(std::is_same_v<std::tuple_element_t<0, P>, Exp1>);
        static_assert(std::is_same_v<std::tuple_element_t<1, P>, Exp2>);
    }
    {
        using Exp1 = const T1;
        using Exp2 = const T2;
        using P = const cruft::tight_pair<T1, T2>;
        static_assert(std::is_same_v<std::tuple_element_t<0, P>, Exp1>);
        static_assert(std::is_same_v<std::tuple_element_t<1, P>, Exp2>);
    }
    {
        using Exp1 = volatile T1;
        using Exp2 = volatile T2;
        using P = volatile cruft::tight_pair<T1, T2>;
        static_assert(std::is_same_v<std::tuple_element_t<0, P>, Exp1>);
        static_assert(std::is_same_v<std::tuple_element_t<1, P>, Exp2>);
    }
    {
        using Exp1 = const volatile T1;
        using Exp2 = const volatile T2;
        using P = const volatile cruft::tight_pair<T1, T2>;
        static_assert(std::is_same_v<std::tuple_element_t<0, P>, Exp1>);
        static_assert(std::is_same_v<std::tuple_element_t<1, P>, Exp2>);
    }
}

TEST_CASE( "test tuple_element" )
{
    test<int, short>();
    test<int*, char>();
}

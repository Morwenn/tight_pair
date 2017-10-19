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
#include <tuple>
#include <utility>
#include <catch.hpp>
#include <tight_pair.h>

namespace
{
    class A
    {
        int i_;
        char c_;

        public:

            A(int i, char c): i_(i), c_(c) {}
            int get_i() const { return i_; }
            char get_c() const { return c_; }
    };

    class B
    {
        double d_;
        unsigned u1_;
        unsigned u2_;

        public:

            B(double d, unsigned u1, unsigned u2): d_(d), u1_(u1), u2_(u2) {}
            double get_d() const { return d_; }
            unsigned get_u1() const { return u1_; }
            unsigned get_u2() const { return u2_; }
    };
}

TEST_CASE( "piecewise construct" )
{
    using cruft::get;

    cruft::tight_pair<A, B> p(std::piecewise_construct,
                              std::make_tuple(4, 'a'),
                              std::make_tuple(3.5, 6u, 2u));
    CHECK(get<0>(p).get_i() == 4);
    CHECK(get<0>(p).get_c() == 'a');
    CHECK(get<1>(p).get_d() == 3.5);
    CHECK(get<1>(p).get_u1() == 6u);
    CHECK(get<1>(p).get_u2() == 2u);
}


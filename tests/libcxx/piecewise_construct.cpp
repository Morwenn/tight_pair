/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <tuple>
#include <utility>
#include <catch2/catch.hpp>
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


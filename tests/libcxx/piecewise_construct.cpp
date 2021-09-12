/*
 * Copyright (c) 2017-2021 Morwenn
 * SPDX-License-Identifier: MIT
 */

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

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

            constexpr A(int i, char c): i_(i), c_(c) {}
            constexpr int get_i() const { return i_; }
            constexpr char get_c() const { return c_; }
    };

    class B
    {
        double d_;
        unsigned u1_;
        unsigned u2_;

        public:

            constexpr explicit B(double d, unsigned u1, unsigned u2): d_(d), u1_(u1), u2_(u2) {}
            constexpr double get_d() const { return d_; }
            constexpr unsigned get_u1() const { return u1_; }
            constexpr unsigned get_u2() const { return u2_; }
    };

    constexpr bool test()
    {
        using cruft::get;
        {
            using P1 = cruft::tight_pair<int, int*>;
            using P2 = cruft::tight_pair<int*, int>;
            using P3 = cruft::tight_pair<P1, P2>;
            P3 p3(std::piecewise_construct,
                  std::tuple<int, int*>(3, nullptr),
                  std::tuple<int*, int>(nullptr, 4));
            assert(get<0>(p3) == P1(3, nullptr));
            assert(get<1>(p3) == P2(nullptr, 4));
        }
        return true;
    }
}

TEST_CASE( "piecewise construct" )
{
    using cruft::get;

    constexpr cruft::tight_pair<A, B> p(std::piecewise_construct,
                                        std::make_tuple(4, 'a'),
                                        std::make_tuple(3.5, 6u, 2u));
    static_assert(get<0>(p).get_i() == 4);
    static_assert(get<0>(p).get_c() == 'a');
    static_assert(get<1>(p).get_d() == 3.5);
    static_assert(get<1>(p).get_u1() == 6u);
    static_assert(get<1>(p).get_u2() == 2u);

    static_assert(test());
}


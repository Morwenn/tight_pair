/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <algorithm>
#include <complex>
#include <functional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "test from cppreference.com examples" )
{
    using cruft::get;

    SECTION( "constructor" )
    {
        using namespace std::string_literals;

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

    SECTION( "swap" )
    {
        cruft::tight_pair<int, std::string> p1, p2;
        p1 = cruft::tight_pair(10, "test");
        p2.swap(p1);
        CHECK(get<0>(p2) == 10);
        CHECK(get<1>(p2) == "test");
    }

    SECTION( "make_pair" )
    {
        int n = 1;
        int a[5] = {1, 2, 3, 4, 5};

        // build a pair from two ints
        auto p1 = cruft::tight_pair(n, a[1]);
        CHECK(get<0>(p1) == 1);
        CHECK(get<1>(p1) == 2);

        // build a pair from a reference to int and an array (decayed to pointer)
        auto p2 = cruft::tight_pair(std::ref(n), a);
        n = 7;
        CHECK(get<0>(p2) == 7);
        CHECK(*(get<1>(p2) + 2) == 3);
    }

    SECTION( "relational operators" )
    {
        std::vector<cruft::tight_pair<int, std::string>> v1 = { {2, "baz"},
                                                                {2, "bar"},
                                                                {1, "foo"} };
        std::vector<cruft::tight_pair<int, std::string>> v2 = { {1, "foo"},
                                                                {2, "bar"},
                                                                {2, "baz"} };
        std::sort(v1.begin(), v1.end());

        CHECK(v1 == v2);
    }

    SECTION( "get" )
    {
        auto p = cruft::tight_pair(1, 3.14);
        CHECK(cruft::get<0>(p) == 1);
        CHECK(cruft::get<1>(p) == 3.14);
        CHECK(cruft::get<int>(p) == 1);
        CHECK(cruft::get<double>(p) == 3.14);
    }
}

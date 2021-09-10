/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
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

/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "test std::tuple_element" )
{
    using P = cruft::tight_pair<float, short*>;
    static_assert(std::is_same_v<std::tuple_element_t<0, P>, float>);
    static_assert(std::is_same_v<std::tuple_element_t<1, P>, short*>);
}

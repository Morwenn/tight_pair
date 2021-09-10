/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "test default constructor triviality" )
{
    static_assert(std::is_trivially_destructible_v<cruft::tight_pair<int, float>>);
    static_assert(not std::is_trivially_destructible_v<cruft::tight_pair<int, std::string>>);
}

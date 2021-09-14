/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <string>
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "test default constructor triviality" )
{
    static_assert(std::is_trivially_destructible_v<cruft::tight_pair<int, float>>);
    static_assert(not std::is_trivially_destructible_v<cruft::tight_pair<int, std::string>>);
}

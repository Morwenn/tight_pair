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

#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "test std::tuple_element" )
{
    using P = cruft::tight_pair<float, short*>;
    static_assert(std::is_same_v<std::tuple_element_t<0, P>, float>);
    static_assert(std::is_same_v<std::tuple_element_t<1, P>, short*>);
}

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

#include <catch2/catch.hpp>
#include <tight_pair.h>

TEST_CASE( "test tuple_size" )
{
    {
        using P1 = cruft::tight_pair<int, short>;
        static_assert(std::tuple_size_v<P1> == 2);
    }
    {
        using P1 = const cruft::tight_pair<int, short>;
        static_assert(std::tuple_size_v<P1> == 2);
    }
    {
        using P1 = volatile cruft::tight_pair<int, short>;
        static_assert(std::tuple_size_v<P1> == 2);
    }
    {
        using P1 = const volatile cruft::tight_pair<int, short>;
        static_assert(std::tuple_size_v<P1> == 2);
    }
}

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

template<typename T1, typename T2>
void test()
{
    {
        using Exp1 = T1;
        using Exp2 = T2;
        using P = cruft::tight_pair<T1, T2>;
        static_assert(std::is_same_v<std::tuple_element_t<0, P>, Exp1>);
        static_assert(std::is_same_v<std::tuple_element_t<1, P>, Exp2>);
    }
    {
        using Exp1 = const T1;
        using Exp2 = const T2;
        using P = const cruft::tight_pair<T1, T2>;
        static_assert(std::is_same_v<std::tuple_element_t<0, P>, Exp1>);
        static_assert(std::is_same_v<std::tuple_element_t<1, P>, Exp2>);
    }
    {
        using Exp1 = volatile T1;
        using Exp2 = volatile T2;
        using P = volatile cruft::tight_pair<T1, T2>;
        static_assert(std::is_same_v<std::tuple_element_t<0, P>, Exp1>);
        static_assert(std::is_same_v<std::tuple_element_t<1, P>, Exp2>);
    }
    {
        using Exp1 = const volatile T1;
        using Exp2 = const volatile T2;
        using P = const volatile cruft::tight_pair<T1, T2>;
        static_assert(std::is_same_v<std::tuple_element_t<0, P>, Exp1>);
        static_assert(std::is_same_v<std::tuple_element_t<1, P>, Exp2>);
    }
}

TEST_CASE( "test tuple_element" )
{
    test<int, short>();
    test<int*, char>();
}

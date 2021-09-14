/*
 * Copyright (c) 2018-2021 Morwenn
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
#include "archetypes.h"

TEST_CASE( "default noexcept" )
{
    using NonThrowingDefault = NonThrowingTypes::DefaultOnly;
#if !defined(__GNUC__) || (defined(__GNUC__) && __GNUC__ >= 9)
    using ThrowingDefault = NonTrivialTypes::DefaultOnly;
    // NOTE: the behaviour for these test cases depends on whether the compiler implements
    //       the resolution of P0003R5 which removes a noexcept special case with regard to
    //       constant expressions, which apparently was a serendipitous change so compilers
    //       took some time to adapt.
    static_assert(not std::is_nothrow_default_constructible_v<cruft::tight_pair<ThrowingDefault, ThrowingDefault>>);
    static_assert(not std::is_nothrow_default_constructible_v<cruft::tight_pair<NonThrowingDefault, ThrowingDefault>>);
    static_assert(not std::is_nothrow_default_constructible_v<cruft::tight_pair<ThrowingDefault, NonThrowingDefault>>);
#endif
    static_assert(std::is_nothrow_default_constructible_v<cruft::tight_pair<NonThrowingDefault, NonThrowingDefault>>);
}

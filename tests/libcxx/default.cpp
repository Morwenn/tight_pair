/*
 * Copyright (c) 2018-2020 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>
#include "archetypes.h"

TEST_CASE( "default noexcept" )
{
    using NonThrowingDefault = NonThrowingTypes::DefaultOnly;
#ifdef __clang__
    using ThrowingDefault = NonTrivialTypes::DefaultOnly;
    // NOTE: the behaviour for these test cases depends on whether the compiler implements
    //       the resolution of P0003R5 which removes a noexcept special case with regard to
    //       constant expressions, which apparently was a serendipitous change so we can't
    //       rely on a guaranteed behaviour right now
    static_assert(not std::is_nothrow_default_constructible_v<cruft::tight_pair<ThrowingDefault, ThrowingDefault>>);
    static_assert(not std::is_nothrow_default_constructible_v<cruft::tight_pair<NonThrowingDefault, ThrowingDefault>>);
    static_assert(not std::is_nothrow_default_constructible_v<cruft::tight_pair<ThrowingDefault, NonThrowingDefault>>);
#endif
    static_assert(std::is_nothrow_default_constructible_v<cruft::tight_pair<NonThrowingDefault, NonThrowingDefault>>);
}

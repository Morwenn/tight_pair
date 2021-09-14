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
#include "archetypes.h"

using namespace ImplicitTypes;

namespace ConstructorTest
{
    template<
        typename T1,
        bool CanCopy = true,
        bool CanMove = CanCopy
    >
    void test()
    {
        using P1 = cruft::tight_pair<T1, int>;
        using P2 = cruft::tight_pair<int, T1>;
        static_assert(std::is_copy_constructible_v<P1> == CanCopy);
        static_assert(std::is_move_constructible_v<P1> == CanMove);
        static_assert(std::is_copy_constructible_v<P2> == CanCopy);
        static_assert(std::is_move_constructible_v<P2> == CanMove);
    }
}

inline void test_constructors_exist()
{
  using namespace ConstructorTest;
  {
    test<int>();
    test<int&>();
    test<int&&, false, true>();
    test<const int>();
    test<const int&>();
    test<const int&&, false, true>();
  }
  {
    test<Copyable>();
    test<Copyable&>();
    test<Copyable&&, false, true>();
  }
  {
    test<NonCopyable, false>();
    test<NonCopyable&, true>();
    test<NonCopyable&&, false, true>();
  }
  {
    // Even though CopyOnly has an explicitly deleted move constructor
    // pair's move constructor is only implicitly deleted and therefore
    // it doesn't participate in overload resolution.
    test<CopyOnly, true, true>();
    test<CopyOnly&, true>();
    test<CopyOnly&&, false, true>();
  }
  {
    test<MoveOnly, false, true>();
    test<MoveOnly&, true>();
    test<MoveOnly&&, false, true>();
  }
}

namespace AssignmentOperatorTest
{
    template<
        typename T1,
        bool CanCopy = true,
        bool CanMove = CanCopy
    >
    void test()
    {
        using P1 = cruft::tight_pair<T1, int>;
        using P2 = cruft::tight_pair<int, T1>;
        static_assert(std::is_copy_assignable_v<P1> == CanCopy);
        static_assert(std::is_move_assignable_v<P1> == CanMove);
        static_assert(std::is_copy_assignable_v<P2> == CanCopy);
        static_assert(std::is_move_assignable_v<P2> == CanMove);
    }
}

inline void test_assignment_operator_exists()
{
    using namespace AssignmentOperatorTest;
    {
        test<int>();
        test<int&>();
        test<int&&>();
        test<const int, false>();
        test<const int&, false>();
        test<const int&&, false>();
    }
    {
        test<Copyable>();
        test<Copyable&>();
        test<Copyable&&>();
    }
    {
        test<NonCopyable, false>();
        test<NonCopyable&, false>();
        test<NonCopyable&&, false>();
    }
    {
        test<CopyOnly, true>();
        test<CopyOnly&, true>();
        test<CopyOnly&&, true>();
    }
    {
        test<MoveOnly, false, true>();
        test<MoveOnly&, false, false>();
        test<MoveOnly&&, false, true>();
    }
}

TEST_CASE( "special members generation" )
{
    test_constructors_exist();
    test_assignment_operator_exists();
}

/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Morwenn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <catch.hpp>
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

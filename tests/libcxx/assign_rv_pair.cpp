/*
 * Copyright (c) 2017-2021 Morwenn
 * SPDX-License-Identifier: MIT
 */

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <type_traits>
#include <utility>
#include <catch2/catch.hpp>
#include <tight_pair.h>
#include "archetypes.h"

namespace
{
    struct CountAssign
    {
        int copied = 0;
        int moved = 0;
        constexpr CountAssign() = default;
        constexpr CountAssign& operator=(CountAssign const&) {
            ++copied;
            return *this;
        }
        constexpr CountAssign& operator=(CountAssign&&) {
            ++moved;
            return *this;
        }
    };

    struct NotAssignable
    {
        NotAssignable& operator=(NotAssignable const&) = delete;
        NotAssignable& operator=(NotAssignable&&) = delete;
    };

    struct MoveAssignable
    {
        MoveAssignable& operator=(MoveAssignable const&) = delete;
        MoveAssignable& operator=(MoveAssignable&&) = default;
    };

    struct CopyAssignable
    {
        CopyAssignable& operator=(CopyAssignable const&) = default;
        CopyAssignable& operator=(CopyAssignable&&) = delete;
    };

    constexpr bool test()
    {
        using cruft::get;
        {
            using P = cruft::tight_pair<ConstexprTestTypes::MoveOnly, int>;
            P p1(3, 4);
            P p2;
            p2 = std::move(p1);
            assert(get<0>(p2).value == 3);
            assert(get<1>(p2) == 4);
        }
        {
            using P = cruft::tight_pair<int&, int&&>;
            int x = 42;
            int y = 101;
            int x2 = -1;
            int y2 = 300;
            P p1(x, std::move(y));
            P p2(x2, std::move(y2));
            p1 = std::move(p2);
            assert(get<0>(p1) == x2);
            assert(get<1>(p1) == y2);
        }
        {
            using P = cruft::tight_pair<int, ConstexprTestTypes::DefaultOnly>;
            static_assert(not std::is_move_assignable<P>::value);
        }
        {
            // The move decays to the copy constructor
            using P = cruft::tight_pair<CountAssign, ConstexprTestTypes::CopyOnly>;
            static_assert(std::is_move_assignable<P>::value);
            P p;
            P p2;
            p = std::move(p2);
            assert(get<0>(p).moved == 0);
            assert(get<0>(p).copied == 1);
            assert(get<0>(p2).moved == 0);
            assert(get<0>(p2).copied == 0);
        }
        {
            using P = cruft::tight_pair<CountAssign, ConstexprTestTypes::MoveOnly>;
            static_assert(std::is_move_assignable<P>::value);
            P p;
            P p2;
            p = std::move(p2);
            assert(get<0>(p).moved == 1);
            assert(get<0>(p).copied == 0);
            assert(get<0>(p2).moved == 0);
            assert(get<0>(p2).copied == 0);
        }
        {
            using P1 = cruft::tight_pair<int, NotAssignable>;
            using P2 = cruft::tight_pair<NotAssignable, int>;
            using P3 = cruft::tight_pair<NotAssignable, NotAssignable>;
            static_assert(not std::is_move_assignable<P1>::value);
            static_assert(not std::is_move_assignable<P2>::value);
            static_assert(not std::is_move_assignable<P3>::value);
        }
        {
            // We assign through the reference and don't move out of the incoming ref,
            // so this doesn't work (but would if the type were CopyAssignable).
            using P1 = cruft::tight_pair<MoveAssignable&, int>;
            static_assert(not std::is_move_assignable<P1>::value);

            // ... works if it's CopyAssignable
            using P2 = cruft::tight_pair<CopyAssignable&, int>;
            static_assert(std::is_move_assignable<P2>::value);

            // For rvalue-references, we can move-assign if the type is MoveAssignable
            // or CopyAssignable (since in the worst case the move will decay into a copy).
            using P3 = cruft::tight_pair<MoveAssignable&&, int>;
            using P4 = cruft::tight_pair<CopyAssignable&&, int>;
            static_assert(std::is_move_assignable<P3>::value);
            static_assert(std::is_move_assignable<P4>::value);

            // In all cases, we can't move-assign if the types are not assignable,
            // since we assign through the reference.
            using P5 = cruft::tight_pair<NotAssignable&, int>;
            using P6 = cruft::tight_pair<NotAssignable&&, int>;
            static_assert(not std::is_move_assignable<P5>::value);
            static_assert(not std::is_move_assignable<P6>::value);
        }
        return true;
    }
}

TEST_CASE( "assign rv pair" )
{
    static_assert(test());
}

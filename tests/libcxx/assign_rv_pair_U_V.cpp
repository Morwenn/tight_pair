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
#include <utility>
#include <catch2/catch.hpp>
#include <tight_pair.h>
#include "archetypes.h"

namespace
{
    struct Derived:
        ConstexprTestTypes::MoveOnly
    {
        Derived() = default;
        constexpr Derived(ConstexprTestTypes::MoveOnly&&) {};
    };

    struct CountAssign
    {
        int copied = 0;
        int moved = 0;
        constexpr CountAssign() = default;
        constexpr CountAssign(const int) {};
        constexpr CountAssign& operator=(CountAssign const&) {
            ++copied;
            return *this;
        }
        constexpr CountAssign& operator=(CountAssign&&) {
            ++moved;
            return *this;
        }
    };

    struct CopyAssignableInt
    {
        CopyAssignableInt& operator=(int&) { return *this; }
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
            using P1 = cruft::tight_pair<Derived, short>;
            using P2 = cruft::tight_pair<ConstexprTestTypes::MoveOnly, long>;
            P1 p1(Derived(), static_cast<short>(4));
            P2 p2;
            p2 = std::move(p1);
            assert(get<1>(p2) == 4);
        }
        {
            using P = cruft::tight_pair<int, CountAssign>;
            using T = cruft::tight_pair<long, CountAssign>;
            T t(42, -42);
            P p(101, 101);
            p = std::move(t);
            assert(get<0>(p) == 42);
            assert(get<1>(p).moved == 1);
            assert(get<1>(p).copied == 0);
            assert(get<1>(t).moved == 0);
            assert(get<1>(t).copied == 0);
        }
        { // test const requirement
            using T = cruft::tight_pair<CopyAssignableInt, CopyAssignableInt>;
            using P = cruft::tight_pair<int, int>;
            static_assert(not std::is_assignable<T&, P&&>::value);
            static_assert(not std::is_assignable<P&, T&&>::value);
        }
        {
            // Make sure we can't move-assign from a pair containing a reference
            // if that type isn't copy-constructible (since otherwise we'd be
            // stealing the object through the reference).
            using P1 = cruft::tight_pair<MoveAssignable, long>;
            using P2 = cruft::tight_pair<MoveAssignable&, int>;
            static_assert(not std::is_assignable<P1&, P2&&>::value);

            // ... but this should work since we're going to steal out of the
            // incoming rvalue reference.
            using P3 = cruft::tight_pair<MoveAssignable, long>;
            using P4 = cruft::tight_pair<MoveAssignable&&, int>;
            static_assert(std::is_assignable<P3&, P4&&>::value);
        }
        {
            // We assign through the reference and don't move out of the incoming ref,
            // so this doesn't work (but would if the type were CopyAssignable).
            {
                using P1 = cruft::tight_pair<MoveAssignable&, long>;
                using P2 = cruft::tight_pair<MoveAssignable&, int>;
                static_assert(not std::is_assignable<P1&, P2&&>::value);
            }

            // ... works if it's CopyAssignable
            {
                using P1 = cruft::tight_pair<CopyAssignable&, long>;
                using P2 = cruft::tight_pair<CopyAssignable&, int>;
                static_assert(std::is_assignable<P1&, P2&&>::value);
            }

            // For rvalue-references, we can move-assign if the type is MoveAssignable,
            // or CopyAssignable (since in the worst case the move will decay into a copy).
            {
                using P1 = cruft::tight_pair<MoveAssignable&&, long>;
                using P2 = cruft::tight_pair<MoveAssignable&&, int>;
                static_assert(std::is_assignable<P1&, P2&&>::value);

                using P3 = cruft::tight_pair<CopyAssignable&&, long>;
                using P4 = cruft::tight_pair<CopyAssignable&&, int>;
                static_assert(std::is_assignable<P3&, P4&&>::value);
            }

            // In all cases, we can't move-assign if the types are not assignable,
            // since we assign through the reference.
            {
                using P1 = cruft::tight_pair<NotAssignable&, long>;
                using P2 = cruft::tight_pair<NotAssignable&, int>;
                static_assert(not std::is_assignable<P1&, P2&&>::value);

                using P3 = cruft::tight_pair<NotAssignable&&, long>;
                using P4 = cruft::tight_pair<NotAssignable&&, int>;
                static_assert(not std::is_assignable<P3&, P4&&>::value);
            }
        }
        return true;
    }
}

TEST_CASE( "assign rv pair U V" )
{
    static_assert(test());
}

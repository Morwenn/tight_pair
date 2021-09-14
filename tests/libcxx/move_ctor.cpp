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

#include <type_traits>
#include <utility>
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    struct Dummy
    {
        Dummy(Dummy const&) = delete;
        Dummy(Dummy&&) = default;
    };

    struct NotCopyOrMoveConstructible
    {
        NotCopyOrMoveConstructible() = default;
        NotCopyOrMoveConstructible(NotCopyOrMoveConstructible const&) = delete;
        NotCopyOrMoveConstructible(NotCopyOrMoveConstructible&&) = delete;
    };
}

TEST_CASE( "move constructor tests" )
{
    using cruft::get;
    {
        using P1 = cruft::tight_pair<int, short>;
        static_assert(std::is_move_constructible_v<P1>);
        P1 p1(3, static_cast<short>(4));
        P1 p2 = std::move(p1);
        CHECK(get<0>(p2) == 3);
        CHECK(get<1>(p2) == 4);
    }
    {
        using P = cruft::tight_pair<Dummy, int>;
        static_assert(not std::is_copy_constructible_v<P>);
        static_assert(std::is_move_constructible_v<P>);
    }
    {
        // When constructing a pair containing a reference, we only bind the
        // reference, so it doesn't matter whether the type is or isn't
        // copy/move constructible.
        {
            using P = cruft::tight_pair<NotCopyOrMoveConstructible&, int>;
            static_assert(std::is_move_constructible<P>::value);

            NotCopyOrMoveConstructible obj;
            P p2{obj, 3};
            P p1(std::move(p2));
            CHECK(&get<0>(p1) == &obj);
            CHECK(&get<0>(p2) == &obj);
        }
        {
            using P = cruft::tight_pair<NotCopyOrMoveConstructible&&, int>;
            static_assert(std::is_move_constructible<P>::value);

            NotCopyOrMoveConstructible obj;
            P p2{std::move(obj), 3};
            P p1(std::move(p2));
            CHECK(&get<0>(p1) == &obj);
            CHECK(&get<0>(p2) == &obj);
        }
    }
}

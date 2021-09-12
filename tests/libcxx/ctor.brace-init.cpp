/*
 * Copyright (c) 2021 Morwenn
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

namespace
{
    struct ExplicitT
    {
        constexpr explicit ExplicitT(int x): value(x) {}
        constexpr explicit ExplicitT(ExplicitT const& o): value(o.value) {}
        int value;
    };

    struct ImplicitT
    {
        constexpr ImplicitT(int x): value(x) {}
        constexpr ImplicitT(ImplicitT const& o): value(o.value) {}
        int value;
    };

    template<typename T, typename=decltype(cruft::tight_pair<T, T>({}, {}))>
    constexpr bool can_construct_with_brace_init(int) { return true; }
    template<typename T>
    constexpr bool can_construct_with_brace_init(...) { return false; }

    template<typename T, typename=decltype(cruft::tight_pair(T{}, {}))>
    constexpr bool can_construct_with_ctad_brace_init(int) { return true; }
    template<typename T>
    constexpr bool can_construct_with_ctad_brace_init(...) { return false; }

    struct BraceInit { BraceInit() = default; };
    struct NoBraceInit { NoBraceInit(int); };
    struct ExplicitBraceInit { explicit ExplicitBraceInit() = default; };

    constexpr int explicit_vs_implicit_brace_init(cruft::tight_pair<ExplicitBraceInit, ExplicitBraceInit>) { return 1; }
    constexpr int explicit_vs_implicit_brace_init(cruft::tight_pair<BraceInit, BraceInit>) { return 2; }

    constexpr bool test()
    {
        using cruft::get;

        // Explicit constructor
        {
            constexpr cruft::tight_pair<ExplicitT, BraceInit> p1(ExplicitT{42}, {});
            static_assert(get<0>(p1).value == 42);

            constexpr cruft::tight_pair<ExplicitT, BraceInit> p2{ExplicitT{42}, {}};
            static_assert(get<0>(p2).value == 42);
        }
        {
            constexpr cruft::tight_pair<BraceInit, ExplicitT> p1({}, ExplicitT{42});
            static_assert(get<1>(p1).value == 42);

            constexpr cruft::tight_pair<BraceInit, ExplicitT> p2{{}, ExplicitT{42}};
            static_assert(get<1>(p2).value == 42);
        }
        {
            cruft::tight_pair<BraceInit, BraceInit> p{{}, {}};
            (void)p;
        }

        // Implicit constructor
        {
            constexpr cruft::tight_pair<ImplicitT, BraceInit> p = {42, {}};
            static_assert(get<0>(p).value == 42);
        }
        {
            constexpr cruft::tight_pair<BraceInit, ImplicitT> p = {{}, 42};
            static_assert(get<1>(p).value == 42);
        }
        {
            cruft::tight_pair<BraceInit, BraceInit> p = {{}, {}};
            (void)p;
        }

        // SFINAE-friendliness of some invalid cases
        {
            static_assert(    can_construct_with_brace_init<BraceInit>(0));
            static_assert(not can_construct_with_brace_init<NoBraceInit>(0));

            // CTAD with {} should never work, since we can't possibly deduce the types
            static_assert(not can_construct_with_ctad_brace_init<BraceInit>(0));
            static_assert(not can_construct_with_ctad_brace_init<int>(0));
        }

#ifndef __GNUC__
        // Make sure there is no ambiguity between the explicit and the non-explicit constructors
        {
            static_assert(explicit_vs_implicit_brace_init({{}, {}}) == 2);
        }
#endif

        return true;
    }
}

TEST_CASE( "test constructor brace init" )
{
    static_assert(test());
}

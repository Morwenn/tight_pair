/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Morwenn
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
#include <type_traits>
#include <catch2/catch.hpp>
#include <tight_pair.h>
#include "archetypes.h"
#include "test_convertible.h"

using namespace ImplicitTypes; // Get implicitly archetypes

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

    template<
        typename T1,
        bool CanCopy = true,
        bool CanConvert = CanCopy
    >
    void test_sfinae() {
        using P1 = cruft::tight_pair<T1, int>;
        using P2 = cruft::tight_pair<int, T1>;
        using T1Arg = T1 const&;
        using T2 = int const&;
        static_assert(std::is_constructible_v<P1, T1Arg, T2> == CanCopy);
        static_assert(test_convertible<P1, T1Arg, T2>() == CanConvert);
        static_assert(std::is_constructible_v<P2, T2, T1Arg> == CanCopy);
        static_assert(test_convertible<P2, T2, T1Arg>() == CanConvert);
    }
}

TEST_CASE( "const first const second" )
{
    using cruft::get;

    {
        using P = cruft::tight_pair<float, short*>;
        P p(3.5f, 0);
        CHECK(get<0>(p) == 3.5f);
        CHECK(get<1>(p) == nullptr);
    }
    {
        using P = cruft::tight_pair<ImplicitT, int>;
        P p(1, 2);
        CHECK(get<0>(p).value == 1);
        CHECK(get<1>(p) == 2);
    }
    {
        test_sfinae<AllCtors>();
        test_sfinae<ExplicitTypes::AllCtors, true, false>();
        test_sfinae<CopyOnly>();
        test_sfinae<ExplicitTypes::CopyOnly, true, false>();
        test_sfinae<MoveOnly, false>();
        test_sfinae<ExplicitTypes::MoveOnly, false>();
        test_sfinae<NonCopyable, false>();
        test_sfinae<ExplicitTypes::NonCopyable, false>();
    }
    {
        using P = cruft::tight_pair<float, short*>;
        constexpr P p(3.5f, 0);
        static_assert(get<0>(p) == 3.5f);
        static_assert(get<1>(p) == nullptr);
    }
    {
        using P = cruft::tight_pair<ExplicitT, int>;
        constexpr ExplicitT e(42);
        constexpr int x = 10;
        constexpr P p(e, x);
        static_assert(get<0>(p).value == 42);
        static_assert(get<1>(p) == 10);
    }
    {
        using P = cruft::tight_pair<ImplicitT, int>;
        constexpr ImplicitT e(42);
        constexpr int x = 10;
        constexpr P p = {e, x};
        static_assert(get<0>(p).value == 42);
        static_assert(get<1>(p) == 10);
    }
}

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
#include <array>
#include <tuple>
#include <type_traits>
#include <utility>
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    struct CountingType
    {
        static int constructed;
        static int copy_constructed;
        static int move_constructed;
        static int assigned;
        static int copy_assigned;
        static int move_assigned;
        static void reset()
        {
            constructed = copy_constructed = move_constructed = 0;
            assigned = copy_assigned = move_assigned = 0;
        }

        CountingType(): value(0) { ++constructed; }
        CountingType(int v): value(v) { ++constructed; }
        CountingType(CountingType const& o): value(o.value) { ++constructed; ++copy_constructed; }
        CountingType(CountingType&& o): value(o.value) { ++constructed; ++move_constructed; o.value = -1;}

        CountingType& operator=(CountingType const& o)
        {
            ++assigned;
            ++copy_assigned;
            value = o.value;
            return *this;
        }

        CountingType& operator=(CountingType&& o)
        {
            ++assigned;
            ++move_assigned;
            value = o.value;
            o.value = -1;
            return *this;
        }
        int value;
    };

    int CountingType::constructed;
    int CountingType::copy_constructed;
    int CountingType::move_constructed;
    int CountingType::assigned;
    int CountingType::copy_assigned;
    int CountingType::move_assigned;
}

TEST_CASE( "assign tuple" )
{
    using cruft::get;

    using C = CountingType;
    {
        using P = cruft::tight_pair<int, C>;
        using T = std::tuple<int, C>;
        T t(42, C{42});
        P p(101, C{101});
        C::reset();
        p = t;
        CHECK(C::constructed == 0);
        CHECK(C::assigned == 1);
        CHECK(C::copy_assigned == 1);
        CHECK(C::move_assigned == 0);
        CHECK(get<0>(p) == 42);
        CHECK(get<1>(p).value == 42);
    }
    {
        using P = cruft::tight_pair<int, C>;
        using T = std::tuple<int, C>;
        T t(42, -42);
        P p(101, 101);
        C::reset();
        p = std::move(t);
        CHECK(C::constructed == 0);
        CHECK(C::assigned == 1);
        CHECK(C::copy_assigned == 0);
        CHECK(C::move_assigned == 1);
        CHECK(get<0>(p) == 42);
        CHECK(get<1>(p).value == -42);
    }
    {
        using P = cruft::tight_pair<C, C>;
        using T = std::array<C, 2>;
        T t = {42, -42};
        P p{101, 101};
        C::reset();
        p = t;
        CHECK(C::constructed == 0);
        CHECK(C::assigned == 2);
        CHECK(C::copy_assigned == 2);
        CHECK(C::move_assigned == 0);
        CHECK(get<0>(p).value == 42);
        CHECK(get<1>(p).value == -42);
    }
    {
        using P = cruft::tight_pair<C, C>;
        using T = std::array<C, 2>;
        T t = {42, -42};
        P p{101, 101};
        C::reset();
        p = t;
        CHECK(C::constructed == 0);
        CHECK(C::assigned == 2);
        CHECK(C::copy_assigned == 2);
        CHECK(C::move_assigned == 0);
        CHECK(get<0>(p).value == 42);
        CHECK(get<1>(p).value == -42);
    }
    {
        using P = cruft::tight_pair<C, C>;
        using T = std::array<C, 2>;
        T t = {42, -42};
        P p{101, 101};
        C::reset();
        p = std::move(t);
        CHECK(C::constructed == 0);
        CHECK(C::assigned == 2);
        CHECK(C::copy_assigned == 0);
        CHECK(C::move_assigned == 2);
        CHECK(get<0>(p).value == 42);
        CHECK(get<1>(p).value == -42);
    }
}

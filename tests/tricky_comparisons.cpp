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

TEST_CASE( "test optimized comparisons" )
{
    using cruft::get;

    SECTION( "unsigned char" )
    {
        using integer_t = unsigned char;

        cruft::tight_pair<integer_t, integer_t> p1(5, 6);
        cruft::tight_pair<integer_t, integer_t> p2(6, 5);
        CHECK( p1 < p2 );
        CHECK( p1 <= p2 );
        CHECK( p2 > p1 );
        CHECK( p2 >= p1 );

        cruft::tight_pair<integer_t, integer_t> p3(4, 7);
        cruft::tight_pair<integer_t, integer_t> p4(4, 8);
        CHECK( p3 < p4 );
        CHECK( p3 <= p4 );
        CHECK( p4 > p3 );
        CHECK( p4 >= p3 );

        cruft::tight_pair<integer_t, integer_t> p5(10, 22);
        cruft::tight_pair<integer_t, integer_t> p6(10, 22);
        CHECK( p5 == p6 );
        CHECK_FALSE( p5 != p6 );
        CHECK_FALSE( p5 < p6 );
        CHECK( p5 <= p6 );
        CHECK_FALSE( p5 > p6 );
        CHECK( p5 >= p6 );
    }

    SECTION( "unsigned short" )
    {
        using integer_t = unsigned short;

        cruft::tight_pair<integer_t, integer_t> p1(5, 6);
        cruft::tight_pair<integer_t, integer_t> p2(6, 5);
        CHECK( p1 < p2 );
        CHECK( p1 <= p2 );
        CHECK( p2 > p1 );
        CHECK( p2 >= p1 );

        cruft::tight_pair<integer_t, integer_t> p3(4, 7);
        cruft::tight_pair<integer_t, integer_t> p4(4, 8);
        CHECK( p3 < p4 );
        CHECK( p3 <= p4 );
        CHECK( p4 > p3 );
        CHECK( p4 >= p3 );

        cruft::tight_pair<integer_t, integer_t> p5(10, 22);
        cruft::tight_pair<integer_t, integer_t> p6(10, 22);
        CHECK( p5 == p6 );
        CHECK_FALSE( p5 != p6 );
        CHECK_FALSE( p5 < p6 );
        CHECK( p5 <= p6 );
        CHECK_FALSE( p5 > p6 );
        CHECK( p5 >= p6 );
    }

    SECTION( "unsigned int" )
    {
        using integer_t = unsigned int;

        cruft::tight_pair<integer_t, integer_t> p1(5, 6);
        cruft::tight_pair<integer_t, integer_t> p2(6, 5);
        CHECK( p1 < p2 );
        CHECK( p1 <= p2 );
        CHECK( p2 > p1 );
        CHECK( p2 >= p1 );

        cruft::tight_pair<integer_t, integer_t> p3(4, 7);
        cruft::tight_pair<integer_t, integer_t> p4(4, 8);
        CHECK( p3 < p4 );
        CHECK( p3 <= p4 );
        CHECK( p4 > p3 );
        CHECK( p4 >= p3 );

        cruft::tight_pair<integer_t, integer_t> p5(10, 22);
        cruft::tight_pair<integer_t, integer_t> p6(10, 22);
        CHECK( p5 == p6 );
        CHECK_FALSE( p5 != p6 );
        CHECK_FALSE( p5 < p6 );
        CHECK( p5 <= p6 );
        CHECK_FALSE( p5 > p6 );
        CHECK( p5 >= p6 );
    }

    SECTION( "unsigned long" )
    {
        using integer_t = unsigned long;

        cruft::tight_pair<integer_t, integer_t> p1(5, 6);
        cruft::tight_pair<integer_t, integer_t> p2(6, 5);
        CHECK( p1 < p2 );
        CHECK( p1 <= p2 );
        CHECK( p2 > p1 );
        CHECK( p2 >= p1 );

        cruft::tight_pair<integer_t, integer_t> p3(4, 7);
        cruft::tight_pair<integer_t, integer_t> p4(4, 8);
        CHECK( p3 < p4 );
        CHECK( p3 <= p4 );
        CHECK( p4 > p3 );
        CHECK( p4 >= p3 );

        cruft::tight_pair<integer_t, integer_t> p5(10, 22);
        cruft::tight_pair<integer_t, integer_t> p6(10, 22);
        CHECK( p5 == p6 );
        CHECK_FALSE( p5 != p6 );
        CHECK_FALSE( p5 < p6 );
        CHECK( p5 <= p6 );
        CHECK_FALSE( p5 > p6 );
        CHECK( p5 >= p6 );
    }

    SECTION( "unsigned long long" )
    {
        using integer_t = unsigned long long;

        cruft::tight_pair<integer_t, integer_t> p1(5, 6);
        cruft::tight_pair<integer_t, integer_t> p2(6, 5);
        CHECK( p1 < p2 );
        CHECK( p1 <= p2 );
        CHECK( p2 > p1 );
        CHECK( p2 >= p1 );

        cruft::tight_pair<integer_t, integer_t> p3(4, 7);
        cruft::tight_pair<integer_t, integer_t> p4(4, 8);
        CHECK( p3 < p4 );
        CHECK( p3 <= p4 );
        CHECK( p4 > p3 );
        CHECK( p4 >= p3 );

        cruft::tight_pair<integer_t, integer_t> p5(10, 22);
        cruft::tight_pair<integer_t, integer_t> p6(10, 22);
        CHECK( p5 == p6 );
        CHECK_FALSE( p5 != p6 );
        CHECK_FALSE( p5 < p6 );
        CHECK( p5 <= p6 );
        CHECK_FALSE( p5 > p6 );
        CHECK( p5 >= p6 );
    }
}

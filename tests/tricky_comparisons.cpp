/*
 * Copyright (c) 2017-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <catch2/catch.hpp>
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

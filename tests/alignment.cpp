/*
 * Copyright (c) 2019 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>
#include <catch2/catch.hpp>
#include <tight_pair.h>

namespace
{
    struct wrap
    {
        std::uint32_t wrapped;
    };
}

TEST_CASE( "test alignment of compressed pair" )
{
    using cruft::get;

    CHECK( alignof(cruft::tight_pair<std::uint16_t, std::uint16_t>) == alignof(std::uint32_t) );
    CHECK( alignof(cruft::tight_pair<std::uint32_t, std::uint32_t>) == alignof(std::uint64_t) );
    CHECK( alignof(cruft::tight_pair<wrap, wrap>) == alignof(std::uint32_t) );
}

/*
 * Copyright (c) 2019 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <functional>
#include <iostream>
#include <utility>
#include <tight_pair.h>

struct identity
{
    template<typename T>
    constexpr auto operator()(T&& value) const noexcept
        -> T&&
    {
        return std::forward<T>(value);
    }
};

int main()
{
    // Store a comparison and a projection, imagine that
    // the Ranges TS is already in the standard library :p
    auto p1 = std::pair(std::less{}, identity{});
    auto p2 = cruft::tight_pair(std::less{}, identity{});

    // prints "2 1" on my computer
    std::cout << sizeof(p1) << ' ' << sizeof(p2);
}

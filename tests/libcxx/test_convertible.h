/*
 * Copyright (c) 2017 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CRUFT_TIGHT_PAIR_TEST_LIBCXX_TEST_CONVERTIBLE_H_
#define CRUFT_TIGHT_PAIR_TEST_LIBCXX_TEST_CONVERTIBLE_H_

namespace detail
{
    template<typename Tp>
    void eat_type(Tp);

    template<typename Tp, typename... Args>
    constexpr auto test_convertible_imp(int)
        -> decltype(eat_type<Tp>({std::declval<Args>()...}), true)
    {
        return true;
    }

    template<typename Tp, typename... Args>
    constexpr auto test_convertible_imp(long)
        -> bool
    {
        return false;
    }
}

template<typename Tp, typename... Args>
constexpr bool test_convertible()
{
    return detail::test_convertible_imp<Tp, Args...>(0);
}

#endif // CRUFT_TIGHT_PAIR_TEST_LIBCXX_TEST_CONVERTIBLE_H_

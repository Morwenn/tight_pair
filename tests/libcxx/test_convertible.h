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

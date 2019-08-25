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
#ifndef CRUFT_TIGHT_PAIR_DETAIL_COMMON_H_
#define CRUFT_TIGHT_PAIR_DETAIL_COMMON_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <climits>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#ifndef CRUFT_TIGHT_PAIR_USE_UNSIGNED_128INT
#   if defined(__clang__)
#       define CRUFT_TIGHT_PAIR_USE_UNSIGNED_128INT 1
#   elif defined(__GNUC__)
#       if __GNUC__ >= 8
#           define CRUFT_TIGHT_PAIR_USE_UNSIGNED_128INT 1
#       else
#           define CRUFT_TIGHT_PAIR_USE_UNSIGNED_128INT 0
#       endif
#   else
#       define CRUFT_TIGHT_PAIR_USE_UNSIGNED_128INT 0
#   endif
#endif

#endif // CRUFT_TIGHT_PAIR_DETAIL_COMMON_H_

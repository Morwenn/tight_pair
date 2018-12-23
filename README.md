[![Build Status](https://travis-ci.org/Morwenn/tight_pair.svg?branch=master)](https://travis-ci.org/Morwenn/tight_pair)
[![License](http://img.shields.io/:license-mit-blue.svg)](http://doge.mit-license.org)

*This experimental project is a small single-header header-only C++17 library providing a single `tight_pair` class
template and associated functions. It is not meant to be anywhere near serious, but it was a fun little project to
try to design a C++17-only component, and to optimize it as possible despite the underlying layers of abstraction :)*

`cruft::tight_pair` is a C++17 compressed pair class. Unlike [Boost.Compressed_Pair](http://www.boost.org/doc/libs/1_65_1/libs/utility/doc/html/compressed_pair.html)
it is modelled after `std::pair`, with only a few additions and a few deletions. First, a short list of what is similar
to the standard library's `std::pair`, then we will have a look at what makes them different:
- It supports [the same set of constructors](http://en.cppreference.com/w/cpp/utility/pair/pair), and most notably the *EXPLICIT* ones.
- It also handles piecewise construction, which allows to build non-copyable, non-movable types.
- It has a tuple-like interface through `std::tuple_size`, `std::tuple_element`, and ADL-found `get`.
- It works with structured bindings.
- Its `get` function also accepts types, unless the pair stores two elements of the same type.
- It is trivially destructible when both elements are trivially destructible.
- It unwraps an `std::reference_wrapper<T>` as a `T&`.
- It supports comparison if its elements support comparison.

## Differences from `std::pair`

Now is the time to look at what actually makes this `tight_pair` different from the standard library's `std::pair`:
- As its name implies, it is what is known as a *compressed pair*: a pair that privately inherits from the elements to
  take advantage of [empty base class optimization](http://en.cppreference.com/w/cpp/language/ebo) and thus reduce the
  size of the pair. This kind of optimization is mostly used when storing empty function objects.

  ```cpp
  #include <functional>
  #include <iostream>
  #include <utility>
  #include <tight_pair.h>

  // Store a comparison and a projection, imagine that
  // the Ranges TS is already in the standard library :p
  auto p1 = std::pair(std::less{}, std::identity{});
  auto p2 = cruft::tight_pair(std::less{}, std::identity{});

  // prints "2 1" on my computer
  std::cout << sizeof(p1) << ' ' << sizeof(p2);
  ```

- Full EBCO requires to inherit privately from the empty base members in order to pack the pair as much as possible,
  even when holding instances of other empty pairs. However, this causes a problem with structured bindings: the
  lookup looks for a class-member `get` before looking for a `get` function with ADL. Therefore, if we privately
  inherit from an empty class with a conforming `get` function, the structured bindings lookup will find it first
  but will trigger an error because it is inaccessible.

  To bypass this potential bug without losing its compressing abilities, `cruft::tight_pair` has in-class `get`
  functions equivalent to those that can be found thanks to ADL.

- Unlike `std::pair`, it doesn't have `first` and `second` members nor does it provide `first_type` or `second_type`
  type aliases. The members can only be accessed through the `get` function template, and the member types can only be
  retrieved through `std::tuple_element`. It was a choice to only provide a minimal tuple-like interface.

- Since it is a C++17-only library, I decided not to provide an equivalent to `std::make_pair`: the type deduction is
  done through deduction guides. Unlike `std::pair`, the deduction guides are the ones that handle the decaying of
  array and reference parameters as well as the unwrapping of `std::reference_wrapper<T>` to `T&`. Of course it is
  still possible to store arrays or raw `std::reference_wrapper` instances by manually specifying the types.

  ```cpp
  int a = 5;
  // Creates a cruft::tight_pair<int const&, const char*>
  auto pair = cruft::tight_pair(std::cref(a), "welcome");
  ```

- I borrowed a libc++ extension which allows `cruft::tight_pair` to be constructed from any pair-like type. A pair-like
  type is any type `T` for which `std::tuple_size_v<T> == 2` and a `get` function template can be found through ADL.
  This notably allows to directly construct a `cruft::tight_pair` from `std::pair<T, U>`, `std::tuple<T, U>` or
  `std::array<T, 2>`, as well as other conforming types from third-party libraries.

- Piecewise construction accepts tuple-like classes instead of just instances of `std::tuple` to pass arguments to
  initialize the pair members.

- When possible the comparison operators are optimized with bit tricks to be branchless, and hopefully faster than the
  `std::pair` equivalents. They are currently optimized for a subset of the `unsigned` types. Here are some benchmarks
  results I obtained by feeding instances of `std::pair` and `cruft::tight_pair` to comparison sorts from another of my
  libraries:

  ![](https://i.imgur.com/4wRL5i1.png)

  *Of course* those benchmarks are biased: you may not always get results that good if you replace `unsigned short` by
  `unsigned int`, but you get the idea. These benchmarks can be found in the `bench` directory of the project, and the
  results have been obtained with MinGW g++ 7.1.0 with the options `-O3 -march=native`.

- Every function is `constexpr` if possible, even `operator=` (which is not the case for `std::pair`).

- Most of the constructors are conditionally `noexcept` (at the time of writing, only the piecewise constructor and the
  one that takes a pair-like object are not `noexcept`).

## Acknowledgements

I can't finish a project without stealing code around, so here are the main sources of the code that can be found in
this project when I didn't write it by myself:

* A great deal of code originally comes from the libc++ implementation of `std::pair` and `std::tuple` utilities.

* All the tests that can be found in `tests/libcxx` have been ported from the libc++ testsuite and modified so that
  they can work with the minimal tuple-like interface of the library.

* The tests in `tests/cppreference.cpp` were preliminary tests adapted from the examples on [cppreference.com](cppreference.com)
  to check that the basic features worked correctly.

* The algorithm used to detect whether unsigned integer types have padding bits originally comes from the WG14 paper
  [N1899 - Integer Precision Bits Update](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1899.pdf) by David Svoboda.

* The enumeration used to check the integer byte order of scalar types originally comes from the WG21 proposal
  [P0463R1 - `endian`, Just `endian`](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0463r1.html) by Howard Hinnant.

* I also want to thank the [Godbolt online compiler explorer](https://godbolt.org/) as well as people from Lounge<C++>
  who helped me optimize the small details down to assembly while still striving to remain standard-compliant :)

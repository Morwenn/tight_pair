/*
 * Copyright (c) 2017-2021 Morwenn
 * SPDX-License-Identifier: MIT
 */

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <cassert>
#include <initializer_list>
#include <type_traits>

// Some types dedicated to testing taken from libc++ tests

struct NullBase
{
    protected:
          NullBase() = default;
          NullBase(NullBase const&) = default;
          NullBase& operator=(NullBase const&) = default;
          NullBase(NullBase&&) = default;
          NullBase& operator=(NullBase&&) = default;
};

template<typename Derived, bool Explicit=false>
struct TestBase
{
    static int alive;
    static int constructed;
    static int value_constructed;
    static int default_constructed;
    static int copy_constructed;
    static int move_constructed;
    static int assigned;
    static int value_assigned;
    static int copy_assigned;
    static int move_assigned;
    static int destroyed;

    static void reset() {
        assert(alive == 0);
        alive = 0;
        reset_constructors();
    }

    static void reset_constructors() {
      constructed = value_constructed = default_constructed =
        copy_constructed = move_constructed = 0;
      assigned = value_assigned = copy_assigned = move_assigned = destroyed = 0;
    }

    TestBase() noexcept : value(0) {
        ++alive; ++constructed; ++default_constructed;
    }
    template<bool Dummy = true, typename std::enable_if<Dummy && Explicit, bool>::type = true>
    explicit TestBase(int x) noexcept : value(x) {
        ++alive; ++constructed; ++value_constructed;
    }
    template<bool Dummy = true, typename std::enable_if<Dummy && !Explicit, bool>::type = true>
    TestBase(int x) noexcept : value(x) {
        ++alive; ++constructed; ++value_constructed;
    }
    template<bool Dummy = true, typename std::enable_if<Dummy && Explicit, bool>::type = true>
    explicit TestBase(int, int y) noexcept : value(y) {
        ++alive; ++constructed; ++value_constructed;
    }
    template<bool Dummy = true, typename std::enable_if<Dummy && !Explicit, bool>::type = true>
    TestBase(int, int y) noexcept : value(y) {
        ++alive; ++constructed; ++value_constructed;
    }
    template<bool Dummy = true, typename std::enable_if<Dummy && Explicit, bool>::type = true>
    explicit TestBase(std::initializer_list<int>& il, int = 0) noexcept
      : value(static_cast<int>(il.size())) {
        ++alive; ++constructed; ++value_constructed;
    }
    template<bool Dummy = true, typename std::enable_if<Dummy && !Explicit, bool>::type = true>
    explicit TestBase(std::initializer_list<int>& il, int = 0) noexcept : value(static_cast<int>(il.size())) {
        ++alive; ++constructed; ++value_constructed;
    }
    TestBase& operator=(int xvalue) noexcept {
      value = xvalue;
      ++assigned; ++value_assigned;
      return *this;
    }

protected:

    ~TestBase() {
      assert(value != -999); assert(alive > 0);
      --alive; ++destroyed; value = -999;
    }
    explicit TestBase(TestBase const& o) noexcept : value(o.value) {
        assert(o.value != -1); assert(o.value != -999);
        ++alive; ++constructed; ++copy_constructed;
    }
    explicit TestBase(TestBase && o) noexcept : value(o.value) {
        assert(o.value != -1); assert(o.value != -999);
        ++alive; ++constructed; ++move_constructed;
        o.value = -1;
    }
    TestBase& operator=(TestBase const& o) noexcept {
      assert(o.value != -1); assert(o.value != -999);
      ++assigned; ++copy_assigned;
      value = o.value;
      return *this;
    }
    TestBase& operator=(TestBase&& o) noexcept {
        assert(o.value != -1); assert(o.value != -999);
        ++assigned; ++move_assigned;
        value = o.value;
        o.value = -1;
        return *this;
    }
public:
    int value;
};

template<typename D, bool E> int TestBase<D, E>::alive = 0;
template<typename D, bool E> int TestBase<D, E>::constructed = 0;
template<typename D, bool E> int TestBase<D, E>::value_constructed = 0;
template<typename D, bool E> int TestBase<D, E>::default_constructed = 0;
template<typename D, bool E> int TestBase<D, E>::copy_constructed = 0;
template<typename D, bool E> int TestBase<D, E>::move_constructed = 0;
template<typename D, bool E> int TestBase<D, E>::assigned = 0;
template<typename D, bool E> int TestBase<D, E>::value_assigned = 0;
template<typename D, bool E> int TestBase<D, E>::copy_assigned = 0;
template<typename D, bool E> int TestBase<D, E>::move_assigned = 0;
template<typename D, bool E> int TestBase<D, E>::destroyed = 0;


template<bool Explicit=false>
struct ValueBase
{
    template<bool Dummy = true, typename std::enable_if<Dummy && Explicit, bool>::type = true>
    explicit constexpr ValueBase(int x) : value(x) {}
    template<bool Dummy = true, typename std::enable_if<Dummy && !Explicit, bool>::type = true>
    constexpr ValueBase(int x) : value(x) {}
    template<bool Dummy = true, typename std::enable_if<Dummy && Explicit, bool>::type = true>
    explicit constexpr ValueBase(int, int y) : value(y) {}
    template<bool Dummy = true, typename std::enable_if<Dummy && !Explicit, bool>::type = true>
    constexpr ValueBase(int, int y) : value(y) {}
    template<bool Dummy = true, typename std::enable_if<Dummy && Explicit, bool>::type = true>
    explicit constexpr ValueBase(std::initializer_list<int>& il, int = 0) : value(static_cast<int>(il.size())) {}
    template<bool Dummy = true, typename std::enable_if<Dummy && !Explicit, bool>::type = true>
    constexpr ValueBase(std::initializer_list<int>& il, int = 0) : value(static_cast<int>(il.size())) {}
    constexpr ValueBase& operator=(int xvalue) noexcept {
        value = xvalue;
        return *this;
    }
    //~ValueBase() { assert(value != -999); value = -999; }
    int value;
protected:
    constexpr static int check_value(int const& val) {
      assert(val != -1); assert(val != 999);
      return val;
    }
    constexpr static int check_value(int& val, int val_cp = 0) {
      assert(val != -1); assert(val != 999);
      val_cp = val;
      val = -1;
      return val_cp;
    }
    constexpr ValueBase() noexcept : value(0) {}
    constexpr ValueBase(ValueBase const& o) noexcept : value(check_value(o.value)) {
    }
    constexpr ValueBase(ValueBase && o) noexcept : value(check_value(o.value)) {
    }
    constexpr ValueBase& operator=(ValueBase const& o) noexcept {
        assert(o.value != -1); assert(o.value != -999);
        value = o.value;
        return *this;
    }
    constexpr ValueBase& operator=(ValueBase&& o) noexcept {
        assert(o.value != -1); assert(o.value != -999);
        value = o.value;
        o.value = -1;
        return *this;
    }
};

namespace ImplicitTypes
{
    struct AllCtors:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        using Base::operator=;
        constexpr AllCtors() = default;
        constexpr AllCtors(AllCtors const&) = default;
        constexpr AllCtors(AllCtors&&) = default;
        constexpr AllCtors& operator=(AllCtors const&) = default;
        constexpr AllCtors& operator=(AllCtors&&) = default;
    };

    struct MoveOnly:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        constexpr MoveOnly() = default;
        constexpr MoveOnly(MoveOnly&&) = default;
        MoveOnly& operator=(MoveOnly&&) = default;
    };

    struct CopyOnly:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        constexpr CopyOnly() = default;
        constexpr CopyOnly(CopyOnly const&) = default;
        constexpr CopyOnly(CopyOnly&&) = delete;
        CopyOnly& operator=(CopyOnly const&) = default;
        CopyOnly& operator=(CopyOnly&&) = delete;
    };

    struct NonCopyable:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        constexpr NonCopyable() = default;
        constexpr NonCopyable(NonCopyable const&) = delete;
        NonCopyable& operator=(NonCopyable const&) = delete;
    };

    struct ConvertingType:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        constexpr ConvertingType() = default;
        constexpr ConvertingType(ConvertingType const&) = default;
        constexpr ConvertingType(ConvertingType&&) = default;
        ConvertingType& operator=(ConvertingType const&) = default;
        ConvertingType& operator=(ConvertingType&&) = default;
        template<typename... Args>
        constexpr ConvertingType(Args&&...) {}
        template<typename Arg>
        ConvertingType& operator=(Arg&&) { return *this; }
    };

    struct Copyable:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        constexpr Copyable() = default;
        constexpr Copyable(Copyable const&) = default;
        Copyable& operator=(Copyable const&) = default;
    };
}

namespace ExplicitTypes
{
    struct AllCtors:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        using Base::operator=;
        explicit constexpr AllCtors() = default;
        explicit constexpr AllCtors(AllCtors const&) = default;
        explicit constexpr AllCtors(AllCtors&&) = default;
        constexpr AllCtors &operator=(AllCtors const&) = default;
        constexpr AllCtors &operator=(AllCtors&&) = default;
    };

    struct MoveOnly:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        explicit constexpr MoveOnly() = default;
        explicit constexpr MoveOnly(MoveOnly&&) = default;
        MoveOnly& operator=(MoveOnly&&) = default;
    };

    struct CopyOnly:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        explicit constexpr CopyOnly() = default;
        explicit constexpr CopyOnly(CopyOnly const&) = default;
        explicit constexpr CopyOnly(CopyOnly&&) = delete;
        CopyOnly& operator=(CopyOnly const&) = default;
        CopyOnly& operator=(CopyOnly&&) = delete;
    };

    struct NonCopyable:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        explicit constexpr NonCopyable() = default;
        explicit constexpr NonCopyable(NonCopyable const&) = delete;
        NonCopyable& operator=(NonCopyable const&) = delete;
    };

    struct ConvertingType:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        explicit constexpr ConvertingType() = default;
        explicit constexpr ConvertingType(ConvertingType const&) = default;
        explicit constexpr ConvertingType(ConvertingType&&) = default;
        ConvertingType& operator=(ConvertingType const&) = default;
        ConvertingType& operator=(ConvertingType&&) = default;
        template<typename... Args>
        explicit constexpr ConvertingType(Args&&...) {}
        template<typename Arg>
        ConvertingType& operator=(Arg&&) { return *this; }
    };

    struct Copyable:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        explicit constexpr Copyable() = default;
        explicit constexpr Copyable(Copyable const&) = default;
        Copyable& operator=(Copyable const&) = default;
    };
}

namespace ExplicitTestTypes
{
    struct AllCtors:
        TestBase<AllCtors, true>
    {
        using Base = TestBase<AllCtors, true>;
        using Base::Base;
        using Base::operator=;
        explicit AllCtors() = default;
        explicit AllCtors(AllCtors const&) = default;
        explicit AllCtors(AllCtors&&) = default;
        AllCtors& operator=(AllCtors const&) = default;
        AllCtors& operator=(AllCtors&&) = default;
    };

    using TestType = AllCtors;

    // Add equality operators
    template<typename T>
    constexpr auto operator==(T const& lhs, T const& rhs) noexcept
        -> bool
    {
        return lhs.value == rhs.value;
    }

    template<typename T>
    constexpr auto operator!=(T const& lhs, T const& rhs) noexcept
        -> bool
    {
        return lhs.value != rhs.value;
    }
}

namespace ConstexprTestTypes
{
    struct AllCtors:
        ValueBase<>
    {
        using Base = ValueBase<>;
        using Base::Base;
        using Base::operator=;
        constexpr AllCtors() = default;
        constexpr AllCtors(AllCtors const&) = default;
        constexpr AllCtors(AllCtors&&) = default;
        constexpr AllCtors& operator=(AllCtors const&) = default;
        constexpr AllCtors& operator=(AllCtors&&) = default;
    };

    struct CopyOnly:
        ValueBase<>
    {
        using Base = ValueBase<>;
        using Base::Base;
        constexpr CopyOnly() = default;
        constexpr CopyOnly(CopyOnly const&) = default;
        CopyOnly(CopyOnly&&) = delete;
        constexpr CopyOnly& operator=(CopyOnly const&) = default;
        CopyOnly& operator=(CopyOnly&&) = delete;
    };

    struct MoveOnly:
        ValueBase<>
    {
        using Base = ValueBase<>;
        using Base::Base;
        constexpr MoveOnly() = default;
        constexpr MoveOnly(MoveOnly &&) = default;
        constexpr MoveOnly &operator=(MoveOnly &&) = default;
    };

    struct DefaultOnly:
        ValueBase<>
    {
      using Base = ValueBase<>;
      using Base::Base;
      constexpr DefaultOnly() = default;
      DefaultOnly(DefaultOnly const&) = delete;
      DefaultOnly& operator=(DefaultOnly const&) = delete;
    };

    struct Copyable:
        ValueBase<>
    {
        using Base = ValueBase<>;
        using Base::Base;
        constexpr Copyable() = default;
        constexpr Copyable(Copyable const&) = default;
        constexpr Copyable& operator=(Copyable const&) = default;
    };

    struct NonCopyable:
        ValueBase<>
    {
        using Base = ValueBase<>;
        using Base::Base;
        constexpr NonCopyable() = default;
        NonCopyable(NonCopyable const&) = delete;
        NonCopyable& operator=(NonCopyable const&) = delete;
    };

    struct MoveAssignOnly:
        ValueBase<>
    {
      using Base = ValueBase<>;
      using Base::Base;
      MoveAssignOnly() = delete;
      MoveAssignOnly& operator=(MoveAssignOnly const&) = delete;
      constexpr MoveAssignOnly& operator=(MoveAssignOnly &&) = default;
    };

    using TestType = AllCtors;

    // Add equality operators
    template<typename T>
    constexpr auto operator==(T const& lhs, T const& rhs) noexcept
        -> bool
    {
        return lhs.value == rhs.value;
    }

    template<typename T>
    constexpr auto operator!=(T const& lhs, T const& rhs) noexcept
        -> bool
    {
        return lhs.value != rhs.value;
    }
}

namespace NonThrowingTypes
{
    struct DefaultOnly:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        constexpr DefaultOnly() noexcept = default;
        DefaultOnly(DefaultOnly const&) noexcept = delete;
        DefaultOnly& operator=(DefaultOnly const&) noexcept = delete;
    };
}

namespace NonTrivialTypes
{
    struct DefaultOnly:
        NullBase
    {
        using Base = NullBase;
        using Base::Base;
        constexpr DefaultOnly() {};
        DefaultOnly(DefaultOnly const&) = delete;
        DefaultOnly& operator=(DefaultOnly const&) = delete;
    };
}

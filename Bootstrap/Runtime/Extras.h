/*
 * Copyright (c) 2018-2021, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, chris@deepscroll.com
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Detail.h"

#include "Assertions.h"

template<typename T, typename U>
constexpr auto roundUpToPowerOfTwo(T value, U power_of_two) requires(IsIntegral<T>&& IsIntegral<U>) {

    return ((value - 1) & ~(power_of_two - 1)) + power_of_two;
}

template<typename T>
constexpr bool isPowerOfTwo(T value) requires(IsIntegral<T>) {

    return value && !((value) & (value - 1));
}

///

#ifndef RUNTIME_DONT_REPLACE_STD

namespace std { // NOLINT(cert-dcl58-cpp) Names in std to aid tools

    // NOTE: These are in the "std" namespace since some compilers and static analyzers rely on it.

    template<typename T>
    constexpr T&& forward(Detail::RemoveReference<T>& param) {

        return static_cast<T&&>(param);
    }

    template<typename T>
    constexpr T&& forward(Detail::RemoveReference<T>&& param) noexcept {

        static_assert(!IsLValueReference<T>, "Can't forward an rvalue as an lvalue.");

        return static_cast<T&&>(param);
    }

    template<typename T>
    constexpr T&& move(T& arg) {

        return static_cast<T&&>(arg);
    }
}

#else
#include <utility>
#endif

using std::forward;
using std::move;

///

namespace Detail {

    template<typename T>
    struct _RawPointer {
    
        using Type = T*;
    };
}

///

template<typename T, typename SizeType = decltype(sizeof(T)), SizeType N>
constexpr SizeType arraySize(T (&)[N]) {

    return N;
}

template<typename T>
constexpr T min(const T& a, IdentityType<T> const& b) {

    return b < a ? b : a;
}

template<typename T>
constexpr T max(const T& a, IdentityType<T> const& b) {

    return a < b ? b : a;
}

template<typename T>
constexpr T clamp(const T& value, IdentityType<T> const& min, IdentityType<T> const& max) {

    VERIFY(max >= min);

    if (value > max) {

        return max;
    }

    if (value < min) {

        return min;
    }

    return value;
}

template<typename T, typename U>
constexpr T mix(T const& v1, T const& v2, U const& interpolation) {

    return v1 + (v2 - v1) * interpolation;
}

template<typename T, typename U>
constexpr T ceilDiv(T a, U b) {

    static_assert(sizeof(T) == sizeof(U));

    T result = a / b;

    if ((a % b) != 0) {

        ++result;
    }

    return result;
}

template<typename T, typename U>
inline void swap(T& a, U& b) {

    if (&a == &b) {

        return;
    }

    U tmp = move((U&)a);
    a = (T &&) move(b);
    b = move(tmp);
}

template<typename T, typename U = T>
constexpr T exchange(T& slot, U&& value) {

    T old_value = move(slot);
    slot = forward<U>(value);
    return old_value;
}

template<typename T>
using RawPointer = typename Detail::_RawPointer<T>::Type;

template<typename V>
constexpr decltype(auto) toUnderlying(V value) requires(IsEnum<V>) {

    return static_cast<UnderlyingType<V>>(value);
}

constexpr bool isConstantEvaluated() {

#if __has_builtin(__builtin_is_constant_evaluated)
    return __builtin_is_constant_evaluated();
#else
    return false;
#endif
}

// These can't be exported into the global namespace as they would clash with the C standard library.

namespace Detail {
        
    #define __DEFINE_GENERIC_ABS(type, zero, intrinsic) \
        constexpr type abs(type num) {                  \
                                                        \
            if (isConstantEvaluated()) {                \
                return num < (zero) ? -num : num;       \
            }                                           \
            return __builtin_##intrinsic(num);          \
        }

    __DEFINE_GENERIC_ABS(int, 0, abs);
    __DEFINE_GENERIC_ABS(long, 0L, labs);
    __DEFINE_GENERIC_ABS(long long, 0LL, llabs);
    #ifndef KERNEL
    __DEFINE_GENERIC_ABS(float, 0.0F, fabsf);
    __DEFINE_GENERIC_ABS(double, 0.0, fabs);
    __DEFINE_GENERIC_ABS(long double, 0.0L, fabsl);
    #endif

    #undef __DEFINE_GENERIC_ABS
}
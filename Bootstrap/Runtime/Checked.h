/*
 * Copyright (C) 2011-2019 Apple Inc. All rights reserved.
 * Copyright (c) 2020-2021, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, chris@deepscroll.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "Assertions.h"
#include "Concepts.h"
#include "NumericLimits.h"
#include "Extras.h"

template<typename Destination, typename Source, bool destinationIsWider = (sizeof(Destination) >= sizeof(Source)), bool destinationIsSigned = NumericLimits<Destination>::isSigned(), bool sourceIsSigned = NumericLimits<Source>::isSigned()>
struct TypeBoundsChecker;

///

template<typename Destination, typename Source>
struct TypeBoundsChecker<Destination, Source, false, false, false> {

    static constexpr bool isWithinRange(Source value) {

        return value <= NumericLimits<Destination>::max();
    }
};

///

template<typename Destination, typename Source>
struct TypeBoundsChecker<Destination, Source, false, true, true> {

    static constexpr bool isWithinRange(Source value) {

        return value <= NumericLimits<Destination>::max() && 
            NumericLimits<Destination>::min() <= value;
    }
};

///

template<typename Destination, typename Source>
struct TypeBoundsChecker<Destination, Source, false, false, true> {

    static constexpr bool isWithinRange(Source value) {

        return value >= 0 && value <= NumericLimits<Destination>::max();
    }
};

///

template<typename Destination, typename Source>
struct TypeBoundsChecker<Destination, Source, false, true, false> {

    static constexpr bool isWithinRange(Source value) {

        return value <= static_cast<Source>(NumericLimits<Destination>::max());
    }
};

///

template<typename Destination, typename Source>
struct TypeBoundsChecker<Destination, Source, true, false, false> {
    
    static constexpr bool isWithinRange(Source) {

        return true;
    }
};

///

template<typename Destination, typename Source>
struct TypeBoundsChecker<Destination, Source, true, true, true> {
    
    static constexpr bool isWithinRange(Source) {

        return true;
    }
};

///

template<typename Destination, typename Source>
struct TypeBoundsChecker<Destination, Source, true, false, true> {

    static constexpr bool isWithinRange(Source value) {

        return value >= 0;
    }
};

///

template<typename Destination, typename Source>
struct TypeBoundsChecker<Destination, Source, true, true, false> {

    static constexpr bool isWithinRange(Source value) {

        if (sizeof(Destination) > sizeof(Source)) {

            return true;
        }

        return value <= static_cast<Source>(NumericLimits<Destination>::max());
    }
};

///

template<typename Destination, typename Source>
[[nodiscard]] constexpr bool isWithinRange(Source value) {

    return TypeBoundsChecker<Destination, Source>::isWithinRange(value);
}

///

template<Integral T>
class Checked {

public:

    constexpr Checked() = default;

    explicit constexpr Checked(T value)
        : m_value(value) { }

    template<Integral U>
    constexpr Checked(U value) {

        m_overflow = !isWithinRange<T>(value);
        m_value = value;
    }

    constexpr Checked(Checked const&) = default;

    constexpr Checked(Checked&& other)
        : m_value(exchange(other.m_value, 0)), 
          m_overflow(exchange(other.m_overflow, false)) { }

    ///

    constexpr Checked& operator=(Checked const& other) = default;

    constexpr Checked& operator=(Checked&& other) {
        
        m_value = exchange(other.m_value, 0);
        m_overflow = exchange(other.m_overflow, false);
        return *this;
    }

    ///

    [[nodiscard]] constexpr bool hasOverflow() const {

        return m_overflow;
    }

    ///

    ALWAYS_INLINE constexpr bool operator!() const {

        VERIFY(!m_overflow);

        return !m_value;
    }

    ///

    ALWAYS_INLINE constexpr T value() const {

        VERIFY(!m_overflow);

        return m_value;
    }

    ///

    ALWAYS_INLINE constexpr T valueUnchecked() const {

        return m_value;
    }

    ///

    constexpr void add(T other) {

        m_overflow |= __builtin_add_overflow(m_value, other, &m_value);
    }

    ///

    constexpr void sub(T other) {

        m_overflow |= __builtin_sub_overflow(m_value, other, &m_value);
    }

    ///

    constexpr void mul(T other) {

        m_overflow |= __builtin_mul_overflow(m_value, other, &m_value);
    }

    ///

    constexpr void div(T other)
    {
        if constexpr (IsSigned<T>) {

            // Ensure that the resulting value won't be out of range, this can only happen when dividing by -1.

            if (other == -1 && m_value == NumericLimits<T>::min()) {

                m_overflow = true;

                return;
            }
        }

        if (other == 0) {

            m_overflow = true;

            return;
        }

        m_value /= other;
    }

    ///

    constexpr void mod(T other) {

        auto initial = m_value;
        div(other);
        m_value *= other;
        m_value = initial - m_value;
    }

    ///

    constexpr Checked& operator+=(Checked const& other) {

        m_overflow |= other.m_overflow;
        add(other.value());
        return *this;
    }

    constexpr Checked& operator+=(T other) {

        add(other);

        return *this;
    }

    ///

    constexpr Checked& operator-=(Checked const& other) {

        m_overflow |= other.m_overflow;

        sub(other.value());

        return *this;
    }

    constexpr Checked& operator-=(T other) {

        sub(other);
        return *this;
    }

    ///

    constexpr Checked& operator*=(Checked const& other) {

        m_overflow |= other.m_overflow;
        mul(other.value());
        return *this;
    }

    constexpr Checked& operator*=(T other) {

        mul(other);
        return *this;
    }

    ///

    constexpr Checked& operator/=(Checked const& other) {

        m_overflow |= other.m_overflow;
        div(other.value());
        return *this;
    }

    constexpr Checked& operator/=(T other) {

        div(other);
        return *this;
    }

    ///

    constexpr Checked& operator%=(Checked const& other) {

        m_overflow |= other.m_overflow;
        mod(other.value());
        return *this;
    }

    constexpr Checked& operator%=(T other) {

        mod(other);
        return *this;
    }

    ///

    constexpr Checked& operator++() {

        add(1);
        return *this;
    }

    constexpr Checked operator++(int) {

        Checked old { *this };
        add(1);
        return old;
    }

    ///

    constexpr Checked& operator--() {

        sub(1);
        return *this;
    }

    constexpr Checked operator--(int) {

        Checked old { *this };
        sub(1);
        return old;
    }

    ///

    template<typename U, typename V>
    [[nodiscard]] static constexpr bool additionWouldOverflow(U u, V v) {

#ifdef __clang__
        Checked checked;
        checked = u;
        checked += v;
        return checked.has_overflow();
#else
        return __builtin_add_overflow_p(u, v, (T)0);
#endif
    }

    ///

    template<typename U, typename V>
    [[nodiscard]] static constexpr bool multiplicationWouldOverflow(U u, V v) {

#ifdef __clang__
        Checked checked;
        checked = u;
        checked *= v;
        return checked.has_overflow();
#else
        return __builtin_mul_overflow_p(u, v, (T)0);
#endif
    }

    ///

    template<typename U, typename V, typename X>
    [[nodiscard]] static constexpr bool multiplicationWouldOverflow(U u, V v, X x) {

        Checked checked;
        checked = u;
        checked *= v;
        checked *= x;
        return checked.hasOverflow();
    }

private:
    
    T m_value {};

    bool m_overflow { false };
};

///

template<typename T>
constexpr Checked<T> operator+(Checked<T> const& a, Checked<T> const& b) {
    
    Checked<T> c { a };
    c.add(b.value());
    return c;
}

template<typename T>
constexpr Checked<T> operator-(Checked<T> const& a, Checked<T> const& b) {
    
    Checked<T> c { a };
    c.sub(b.value());
    return c;
}

template<typename T>
constexpr Checked<T> operator*(Checked<T> const& a, Checked<T> const& b) {
    
    Checked<T> c { a };
    c.mul(b.value());
    return c;
}

template<typename T>
constexpr Checked<T> operator/(Checked<T> const& a, Checked<T> const& b) {
    
    Checked<T> c { a };
    c.div(b.value());
    return c;
}

template<typename T>
constexpr bool operator<(Checked<T> const& a, T b) {
    
    return a.value() < b;
}

template<typename T>
constexpr bool operator>(Checked<T> const& a, T b) {
    
    return a.value() > b;
}

template<typename T>
constexpr bool operator>=(Checked<T> const& a, T b) {
    
    return a.value() >= b;
}

template<typename T>
constexpr bool operator<=(Checked<T> const& a, T b) {
    
    return a.value() <= b;
}

template<typename T>
constexpr bool operator==(Checked<T> const& a, T b) {
    
    return a.value() == b;
}

template<typename T>
constexpr bool operator!=(Checked<T> const& a, T b) {
    
    return a.value() != b;
}

template<typename T>
constexpr bool operator<(T a, Checked<T> const& b) {
    
    return a < b.value();
}

template<typename T>
constexpr bool operator>(T a, Checked<T> const& b) {
    
    return a > b.value();
}

template<typename T>
constexpr bool operator>=(T a, Checked<T> const& b) {
    
    return a >= b.value();
}

template<typename T>
constexpr bool operator<=(T a, Checked<T> const& b) {
    
    return a <= b.value();
}

template<typename T>
constexpr bool operator==(T a, Checked<T> const& b) {
    
    return a == b.value();
}

template<typename T>
constexpr bool operator!=(T a, Checked<T> const& b) {

    return a != b.value();
}

template<typename T>
constexpr Checked<T> makeChecked(T value) {

    return Checked<T>(value);
}
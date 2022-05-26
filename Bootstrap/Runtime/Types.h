/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, chris@deepscroll.com
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "IterationDecision.h"
#include "Platform.h"
#include "Extras.h"

using u64 = __UINT64_TYPE__;
using u32 = __UINT32_TYPE__;
using u16 = __UINT16_TYPE__;
using u8 = __UINT8_TYPE__;
using i64 = __INT64_TYPE__;
using i32 = __INT32_TYPE__;
using i16 = __INT16_TYPE__;
using i8 = __INT8_TYPE__;

#ifdef _WIN32
using ssize_t = MakeSigned<size_t>;
#endif

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __ptrdiff_t
using __ptrdiff_t = __PTRDIFF_TYPE__;
#endif

using FlatPointer = Conditional<sizeof(void*) == 8, u64, u32>;

constexpr u64 KiB = 1024;
constexpr u64 MiB = KiB * KiB;
constexpr u64 GiB = KiB * KiB * KiB;
constexpr u64 TiB = KiB * KiB * KiB * KiB;
constexpr u64 PiB = KiB * KiB * KiB * KiB * KiB;
constexpr u64 EiB = KiB * KiB * KiB * KiB * KiB * KiB;

namespace std { // NOLINT(cert-dcl58-cpp) nullptr_t must be in ::std:: for some analysis tools

    using nullptr_t = decltype(nullptr);
}

static constexpr FlatPointer explodeByte(u8 b) {

    FlatPointer value = b;

    if constexpr (sizeof(FlatPointer) == 4) {

        return value << 24 | value << 16 | value << 8 | value;
    }
    else if (sizeof(FlatPointer) == 8) {

        return value << 56 | value << 48 | value << 40 | value << 32 | value << 24 | value << 16 | value << 8 | value;
    }
}

static_assert(explodeByte(0xff) == (FlatPointer)0xffffffffffffffffull);
static_assert(explodeByte(0x80) == (FlatPointer)0x8080808080808080ull);
static_assert(explodeByte(0x7f) == (FlatPointer)0x7f7f7f7f7f7f7f7full);
static_assert(explodeByte(0) == 0);

constexpr size_t alignUpTo(const size_t value, const size_t alignment) {

    return (value + (alignment - 1)) & ~(alignment - 1);
}

enum class [[nodiscard]] TriState : u8 {

    False,
    True,
    Unknown
};

enum MemoryOrder {
    
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST
};

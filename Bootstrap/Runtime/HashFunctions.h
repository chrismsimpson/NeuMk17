/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, chris@deepscroll.com
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Types.h"

constexpr unsigned intHash(u32 key) {

    key += ~(key << 15);
    key ^= (key >> 10);
    key += (key << 3);
    key ^= (key >> 6);
    key += ~(key << 11);
    key ^= (key >> 16);
    return key;
}

constexpr unsigned doubleHash(u32 key) {

    unsigned const magic = 0xBA5EDB01;

    if (key == magic) {

        return 0u;
    }

    if (key == 0u) {

        key = magic;
    }

    key ^= key << 13;
    key ^= key >> 17;
    key ^= key << 5;
    return key;
}

constexpr unsigned pairIntHash(u32 key1, u32 key2) {

    return intHash((intHash(key1) * 209) ^ (intHash(key2 * 413)));
}

constexpr unsigned u64Hash(u64 key) {

    u32 first = key & 0xFFFFFFFF;
    u32 last = key >> 32;
    return pairIntHash(first, last);
}

constexpr unsigned pointerHash(FlatPointer ptr) {

    if constexpr (sizeof(ptr) == 8) {

        return u64Hash(ptr);
    }
    else {

        return intHash(ptr);
    }
}

inline unsigned pointerHash(void const* ptr) {

    return pointerHash(FlatPointer(ptr));
}

/*
 * Copyright (c) 2021, the SerenityOS developers.
 * Copyright (c) 2022, chris@deepscroll.com
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Concepts.h"
#include "Traits.h"
#include "Types.h"

template<typename TEndIterator, IteratorPairWith<TEndIterator> TIterator, typename TUnaryPredicate>
constexpr TIterator findIf(TIterator first, TEndIterator last, TUnaryPredicate&& pred) {

    for (; first != last; ++first) {

        if (pred(*first)) {

            return first;
        }
    }

    return last;
}

template<typename TEndIterator, IteratorPairWith<TEndIterator> TIterator, typename T>
constexpr TIterator find(TIterator first, TEndIterator last, T const& value) {

    return findIf(first, last, [&](auto const& v) { return Traits<T>::equals(value, v); });
}

template<typename TEndIterator, IteratorPairWith<TEndIterator> TIterator, typename T>
constexpr size_t findIndex(TIterator first, TEndIterator last, T const& value) requires(requires(TIterator it) { it.index(); }) {

    return findIf(first, last, [&](auto const& v) { return Traits<T>::equals(value, v); }).index();
}


/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, chris@deepscroll.com
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Types.h"

class Bitmap;
class Error;
class GenericLexer;
class String;
class StringBuilder;
class StringImpl;
class StringView;
class Time;
class Utf8CodePointIterator;
class Utf8View;

template<typename T>
class Span;

template<typename T>
using ReadOnlySpan = Span<const T>;

template<typename T, size_t Size>
struct LinearArray;

template<typename Container, typename ValueType>
class SimpleIterator;

using ReadOnlyBytes = ReadOnlySpan<u8>;

using Bytes = Span<u8>;

template<typename T, MemoryOrder DefaultMemoryOrder>
class Atomic;

template<typename T>
struct Traits;
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


class Checked {

};
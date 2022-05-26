/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, chris@deepscroll.com
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#ifdef KERNEL
#    include <Kernel/Library/ThreadSafeRefCounted.h>
#else
#    include "Assertions.h"
#    include "Checked.h"
#    include "NonCopyable.h"
#    include "Platform.h"
#    include "Extras.h"

class RefCountedBase {
    
    RUNTIME_MAKE_NONCOPYABLE(RefCountedBase);
    RUNTIME_MAKE_NONMOVABLE(RefCountedBase);

public:

    using RefCountType = unsigned int;

    ALWAYS_INLINE void ref() const {

        VERIFY(m_refCount > 0);

        VERIFY(!Checked<RefCountType>::additionWouldOverflow(m_refCount, 1));

        ++m_refCount;
    }

    [[nodiscard]] bool tryRef() const {

        if (m_refCount == 0) {

            return false;
        }

        ref();
        
        return true;
    }

    [[nodiscard]] RefCountType refCount() const { return m_refCount; }

protected:

    RefCountedBase() = default;

    ~RefCountedBase() { VERIFY(!m_refCount); }

    ALWAYS_INLINE RefCountType derefBase() const {

        VERIFY(m_refCount);

        return --m_refCount;
    }

    RefCountType mutable m_refCount { 1 };
};

///

template<typename T>
class RefCounted : public RefCountedBase {

public:

    bool unref() const {

        auto* that = const_cast<T*>(static_cast<T const*>(this));

        auto new_ref_count = derefBase();

        if (new_ref_count == 0) {

            if constexpr (requires { that->will_be_destroyed(); }) {

                that->will_be_destroyed();
            }

            delete static_cast<const T*>(this);
            
            return true;
        }
        
        return false;
    }
};

#endif
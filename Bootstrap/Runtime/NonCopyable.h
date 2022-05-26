/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, chris@deepscroll.com
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define RUNTIME_MAKE_NONCOPYABLE(c) \
private:                            \
    c(c const&) = delete;           \
    c& operator=(c const&) = delete

#define RUNTIME_MAKE_NONMOVABLE(c) \
private:                           \
    c(c&&) = delete;               \
    c& operator=(c&&) = delete

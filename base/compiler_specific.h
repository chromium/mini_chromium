// Copyright 2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_COMPILER_SPECIFIC_H_
#define MINI_CHROMIUM_BASE_COMPILER_SPECIFIC_H_

#include "build/build_config.h"

#if defined(COMPILER_MSVC)
#define ALLOW_UNUSED
#else
#define ALLOW_UNUSED __attribute__((unused))
#endif

#if defined(__clang__) || defined(COMPILER_MSVC)
#define OVERRIDE override
#else
#define OVERRIDE
#endif

#if defined(COMPILER_MSVC)
#define WARN_UNUSED_RESULT
#else
#define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#endif

#if defined(COMPILER_MSVC)
#define PRINTF_FORMAT(format_param, dots_param)
#else
#define PRINTF_FORMAT(format_param, dots_param) \
    __attribute__((format(printf, format_param, dots_param)))
#endif

#endif  // MINI_CHROMIUM_BASE_COMPILER_SPECIFIC_H_

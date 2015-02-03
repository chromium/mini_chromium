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

// Specify memory alignment for structs, classes, etc.
// Use like:
//   class ALIGNAS(16) MyClass { ... }
//   ALIGNAS(16) int array[4];
#if defined(COMPILER_MSVC)
#define ALIGNAS(byte_alignment) __declspec(align(byte_alignment))
#elif defined(COMPILER_GCC)
#define ALIGNAS(byte_alignment) __attribute__((aligned(byte_alignment)))
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

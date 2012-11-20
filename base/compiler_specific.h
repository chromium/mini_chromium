// Copyright 2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_COMPILER_SPECIFIC_H_
#define MINI_CHROMIUM_BASE_COMPILER_SPECIFIC_H_

#if defined(__clang__)
#define OVERRIDE override
#else
#define OVERRIDE
#endif

#define WARN_UNUSED_RESULT __attribute__((warn_unused_result))

#define PRINTF_FORMAT(format_param, dots_param) \
    __attribute__((format(printf, format_param, dots_param)))

#endif  // MINI_CHROMIUM_BASE_COMPILER_SPECIFIC_H_

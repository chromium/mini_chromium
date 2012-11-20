// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_PORT_H_
#define MINI_CHROMIUM_BASE_PORT_H_

#include <stdarg.h>

#include "build/build_config.h"

#if defined(COMPILER_GCC)
#define GG_VA_COPY(a, b) (va_copy(a, b))
#endif

#endif  // MINI_CHROMIUM_BASE_PORT_H_

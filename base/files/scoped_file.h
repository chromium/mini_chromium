// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_FILES_SCOPED_FILE_H_
#define MINI_CHROMIUM_BASE_FILES_SCOPED_FILE_H_

#include "base/memory/scoped_ptr.h"
#include "base/scoped_generic.h"

namespace base {

namespace internal {

struct ScopedFDCloseTraits {
  static int InvalidValue() {
    return -1;
  }
  static void Free(int fd);
};

struct ScopedFILECloser {
  void operator()(FILE* file) const;
};

}  // namespace internal

typedef ScopedGeneric<int, internal::ScopedFDCloseTraits> ScopedFD;
typedef scoped_ptr<FILE, internal::ScopedFILECloser> ScopedFILE;

}  // namespace base

#endif  // MINI_CHROMIUM_BASE_FILES_SCOPED_FILE_H_

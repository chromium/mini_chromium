// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/scoped_file.h"

#include <stdio.h>
#include <unistd.h>

#include "base/logging.h"
#include "base/posix/eintr_wrapper.h"

namespace base {
namespace internal {

void ScopedFDCloseTraits::Free(int fd) {
  PCHECK(IGNORE_EINTR(close(fd)) == 0);
}

void ScopedFILECloser::operator()(FILE* file) const {
  if (file) {
    if (fclose(file) < 0) {
      PLOG(ERROR) << "fclose";
    }
  }
}

}  // namespace internal
}  // namespace base

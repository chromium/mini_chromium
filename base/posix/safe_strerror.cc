// Copyright 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/posix/safe_strerror.h"

#include <stdio.h>
#include <string.h>

#include "base/macros.h"

namespace base {

// The Chromium implementation of this file is concerned with two distinct
// interfaces to strerror_r, and in the case of the POSIX interface,
// ambiguities regarding null-termination and error handling. Mac OS X only
// implements the POSIX strerror_r interface, and its behaves consistently in
// areas where the specification may be ambiguous. The POSIX interface to
// strerror_r has been simplified compared to the Chromium one, but may only
// be suitable for use on Mac OS X.

void safe_strerror_r(int err, char* buf, size_t len) {
  if (buf == NULL || len <= 0) {
    return;
  }

#if defined(__GLIBC__)
  char* ret = strerror_r(err, buf, len);
  if (ret != buf) {
    snprintf(buf, len, "%s", ret);
  }
#else
  int result = strerror_r(err, buf, len);
  if (result != 0) {
    snprintf(buf, len, "Error %d while retrieving error %d", result, err);
  }
#endif
}

std::string safe_strerror(int err) {
  char buf[256];
  safe_strerror_r(err, buf, arraysize(buf));
  return std::string(buf);
}

}  // namespace base

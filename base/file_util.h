// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_FILE_UTIL_H_
#define MINI_CHROMIUM_BASE_FILE_UTIL_H_

#include <unistd.h>

#include <algorithm>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/eintr_wrapper.h"
#include "base/logging.h"

namespace file_util {

class ScopedFD {
 public:
  explicit ScopedFD(int* fd = NULL)
      : fd_(fd) {
  }

  ~ScopedFD() {
    CloseFD();
  }

  void reset(int* fd = NULL) {
    if (fd_ != fd) {
      CloseFD();
      fd_ = fd;
    }
  }

  int& operator*() const {
    DCHECK(fd_);
    return *fd_;
  }

  int* get() const {
    return fd_;
  }

  void swap(ScopedFD& that) {
    std::swap(fd_, that.fd_);
  }

  int* release() WARN_UNUSED_RESULT {
    int* temp = fd_;
    fd_ = NULL;
    return temp;
  }

 private:
  void CloseFD() {
    if (fd_ && *fd_ >= 0) {
      if (HANDLE_EINTR(close(*fd_)) < 0) {
        PLOG(ERROR) << "close";
      }
    }
  }

  int* fd_;

  DISALLOW_COPY_AND_ASSIGN(ScopedFD);
};

}  // namespace file_util

#endif  // MINI_CHROMIUM_BASE_FILE_UTIL_H_

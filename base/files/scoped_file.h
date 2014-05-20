// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_FILES_SCOPED_FILE_H_
#define MINI_CHROMIUM_BASE_FILES_SCOPED_FILE_H_

#include <unistd.h>

#include <algorithm>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/logging.h"
#include "base/posix/eintr_wrapper.h"

namespace base {

class ScopedFD {
 public:
  explicit ScopedFD(int fd = -1)
      : fd_(fd) {
  }

  ~ScopedFD() {
    CloseFD();
  }

  void reset(int fd = -1) {
    if (fd_ != fd) {
      CloseFD();
      fd_ = fd;
    }
  }

  int get() const {
    return fd_;
  }

  void swap(ScopedFD& that) {
    std::swap(fd_, that.fd_);
  }

  int release() WARN_UNUSED_RESULT {
    int temp = fd_;
    fd_ = -1;
    return temp;
  }

 private:
  void CloseFD() {
    if (fd_ >= 0) {
      if (HANDLE_EINTR(close(fd_)) < 0) {
        PLOG(ERROR) << "close";
      }
      fd_ = -1;
    }
  }

  int fd_;

  DISALLOW_COPY_AND_ASSIGN(ScopedFD);
};

}  // namespace base

#endif  // MINI_CHROMIUM_BASE_FILES_SCOPED_FILE_H_

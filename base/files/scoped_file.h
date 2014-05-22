// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_FILES_SCOPED_FILE_H_
#define MINI_CHROMIUM_BASE_FILES_SCOPED_FILE_H_

#include <stdio.h>
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

class ScopedFILE {
 public:
  explicit ScopedFILE(FILE* file = NULL)
      : file_(file) {
  }

  ~ScopedFILE() {
    CloseFile();
  }

  void reset(FILE* file = NULL) {
    if (file_ != file) {
      CloseFile();
      file_ = file;
    }
  }

  FILE* get() const {
    return file_;
  }

  void swap(ScopedFILE& that) {
    std::swap(file_, that.file_);
  }

  FILE* release() WARN_UNUSED_RESULT {
    FILE* temp = file_;
    file_ = NULL;
    return temp;
  }

 private:
  void CloseFile() {
    if (file_) {
      if (fclose(file_) < 0) {
        PLOG(ERROR) << "fclose";
      }
      file_ = NULL;
    }
  }

  FILE* file_;

  DISALLOW_COPY_AND_ASSIGN(ScopedFILE);
};

}  // namespace base

#endif  // MINI_CHROMIUM_BASE_FILES_SCOPED_FILE_H_

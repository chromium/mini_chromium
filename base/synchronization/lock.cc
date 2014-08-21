// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is used for debugging assertion support.  The Lock class
// is functionally a wrapper around the LockImpl class, so the only
// real intelligence in the class is in the debugging logic.

#include "base/synchronization/lock.h"

#include "base/logging.h"

#ifndef NDEBUG

namespace base {

Lock::Lock() : owning_thread_(NULL), lock_() {
}

Lock::~Lock() {
  DCHECK_EQ(owning_thread_, static_cast<pthread_t>(NULL));
}

void Lock::AssertAcquired() const {
  DCHECK_EQ(owning_thread_, pthread_self());
}

void Lock::CheckHeldAndUnmark() {
  DCHECK_EQ(owning_thread_, pthread_self());
  owning_thread_ = NULL;
}

void Lock::CheckUnheldAndMark() {
  DCHECK_EQ(owning_thread_, static_cast<pthread_t>(NULL));
  owning_thread_ = pthread_self();
}

}  // namespace base

#endif

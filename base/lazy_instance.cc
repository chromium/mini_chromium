// Copyright 2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/lazy_instance.h"

#include "base/atomicops.h"
#include "base/macros.h"

#if defined(OS_POSIX)
#include <sched.h>
#elif defined(OS_WIN)
#include <windows.h>
#endif

namespace {

void ThreadYield() {
#if defined(OS_POSIX)
  sched_yield();
#elif defined(OS_WIN)
  Sleep(0);
#endif
}

}  // namespace

namespace base {
namespace internal {

// TODO(joth): This function could be shared with Singleton, in place of its
// WaitForInstance() call.
bool NeedsLazyInstance(subtle::AtomicWord* state) {
  // Try to create the instance, if we're the first, will go from 0 to
  // kLazyInstanceStateCreating, otherwise we've already been beaten here.
  // The memory access has no memory ordering as state 0 and
  // kLazyInstanceStateCreating have no associated data (memory barriers are
  // all about ordering of memory accesses to *associated* data).
  if (subtle::NoBarrier_CompareAndSwap(state, 0,
                                       kLazyInstanceStateCreating) == 0)
    // Caller must create instance
    return true;

  // It's either in the process of being created, or already created. Spin.
  // The load has acquire memory ordering as a thread which sees
  // state_ == STATE_CREATED needs to acquire visibility over
  // the associated data (buf_). Pairing Release_Store is in
  // CompleteLazyInstance().
  while (subtle::Acquire_Load(state) == kLazyInstanceStateCreating) {
    ThreadYield();
  }
  // Someone else created the instance.
  return false;
}

void CompleteLazyInstance(subtle::AtomicWord* state,
                          subtle::AtomicWord new_instance,
                          void* lazy_instance) {
  // Instance is created, go from CREATING to CREATED.
  // Releases visibility over private_buf_ to readers. Pairing Acquire_Load's
  // are in NeedsInstance() and Pointer().
  subtle::Release_Store(state, new_instance);
}

}  // namespace internal
}  // namespace base

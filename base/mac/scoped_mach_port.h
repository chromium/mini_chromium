// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_MAC_SCOPED_MACH_PORT_H_
#define MINI_CHROMIUM_BASE_MAC_SCOPED_MACH_PORT_H_

#include <mach/mach.h>

#include "base/scoped_generic.h"

namespace base {
namespace mac {

namespace internal {

struct SendRightTraits {
  static mach_port_t InvalidValue() {
    return MACH_PORT_NULL;
  }
  static void Free(mach_port_t port);
};

struct ReceiveRightTraits {
  static mach_port_t InvalidValue() {
    return MACH_PORT_NULL;
  }
  static void Free(mach_port_t port);
};

}  // namespace internal

class ScopedMachSendRight
    : public ScopedGeneric<mach_port_t, internal::SendRightTraits> {
 public:
  explicit ScopedMachSendRight(mach_port_t port = traits_type::InvalidValue())
      : ScopedGeneric(port) {
  }

  operator mach_port_t() const { return get(); }
};

class ScopedMachReceiveRight
    : public ScopedGeneric<mach_port_t, internal::ReceiveRightTraits> {
 public:
  explicit ScopedMachReceiveRight(
      mach_port_t port = traits_type::InvalidValue())
    : ScopedGeneric(port) {
  }

  operator mach_port_t() const { return get(); }
};

}  // namespace mac
}  // namespace base

#endif  // MINI_CHROMIUM_BASE_MAC_SCOPED_MACH_PORT_H_

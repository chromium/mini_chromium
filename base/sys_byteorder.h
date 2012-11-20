// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_SYS_BYTEORDER_H_
#define MINI_CHROMIUM_BASE_SYS_BYTEORDER_H_

#include "base/basictypes.h"

#if defined(OS_MACOSX)
#include <libkern/OSByteOrder.h>
#else
#include <byteswap.h>
#endif

namespace base {

inline uint16_t ByteSwap(uint16_t x) {
#if defined(OS_MACOSX)
  return OSSwapInt16(x);
#else
  return bswap_16(x);
#endif
}

inline uint32_t ByteSwap(uint32_t x) {
#if defined(OS_MACOSX)
  return OSSwapInt32(x);
#else
  return bswap_32(x);
#endif
}

inline uint64_t ByteSwap(uint64_t x) {
#if defined(OS_MACOSX)
  return OSSwapInt64(x);
#else
  return bswap_64(x);
#endif
}

inline uint16_t NetToHost16(uint16_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
  return ByteSwap(x);
#else
  return x;
#endif
}

inline uint32_t NetToHost32(uint32_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
  return ByteSwap(x);
#else
  return x;
#endif
}

inline uint64_t NetToHost64(uint64_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
  return ByteSwap(x);
#else
  return x;
#endif
}

inline uint16_t HostToNet16(uint16_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
  return ByteSwap(x);
#else
  return x;
#endif
}

inline uint32_t HostToNet32(uint32_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
  return ByteSwap(x);
#else
  return x;
#endif
}

inline uint64_t HostToNet64(uint64_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
  return ByteSwap(x);
#else
  return x;
#endif
}

}  // namespace base

#endif  // MINI_CHROMIUM_BASE_SYS_BYTEORDER_H_

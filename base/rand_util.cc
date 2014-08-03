// Copyright 2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/rand_util.h"

#include <fcntl.h>
#include <string.h>

#include <cmath>
#include <limits>

#include "base/compiler_specific.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/posix/eintr_wrapper.h"

namespace {

int GetUrandomFDInternal() {
  int fd = HANDLE_EINTR(open("/dev/urandom", O_RDONLY));
  PCHECK(fd >= 0) << "open /dev/urandom";
  return fd;
}

int GetUrandomFD() {
  static int fd = GetUrandomFDInternal();
  return fd;
}

}  // namespace

namespace base {

uint64_t RandUint64() {
  uint64_t number;
  RandBytes(&number, sizeof(number));
  return number;
}

int RandInt(int min, int max) {
  DCHECK_LE(min, max);

  uint64_t range = static_cast<uint64_t>(max) - min + 1;
  int result = min + static_cast<int>(base::RandGenerator(range));
  DCHECK_GE(result, min);
  DCHECK_LE(result, max);

  return result;
}

uint64_t RandGenerator(uint64_t range) {
  DCHECK_GT(range, 0u);

  uint64_t max_acceptable_value =
      (std::numeric_limits<uint64_t>::max() / range) * range - 1;

  uint64_t value;
  do {
    value = base::RandUint64();
  } while (value > max_acceptable_value);

  return value % range;
}

double RandDouble() {
  const int kMantissaBits = std::numeric_limits<double>::digits;
  uint64_t random_bits = RandUint64();

  COMPILE_ASSERT(std::numeric_limits<double>::radix == 2, otherwise_use_scalbn);
  uint64_t mantissa_bits = random_bits & ((UINT64_C(1) << kMantissaBits) - 1);

  double mantissa;
  COMPILE_ASSERT(std::numeric_limits<uint64_t>::digits >= kMantissaBits,
                 uint64_vs_double);
  memcpy(&mantissa, &mantissa_bits, sizeof(mantissa));

  double result = std::ldexp(random_bits, -1 * kMantissaBits);

  DCHECK_GE(result, 0.0);
  DCHECK_LT(result, 1.0);

  return result;
}

void RandBytes(void* output, size_t output_length) {
  if (output_length == 0) {
    return;
  }

  int fd = GetUrandomFD();
  bool success = ReadFromFD(fd, static_cast<char*>(output), output_length);
  CHECK(success);
}

std::string RandBytesAsString(size_t length) {
  if (length == 0) {
    return std::string();
  }

  std::string result(length, std::string::value_type());
  RandBytes(&result[0], length);
  return result;
}

}  // namespace base

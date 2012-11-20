// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/stringprintf.h"

#include <vector>

#include "base/basictypes.h"
#include "base/logging.h"
#include "base/string_util.h"

namespace base {

namespace {

inline int vsnprintfT(char* buffer,
                      size_t buf_size,
                      const char* format,
                      va_list argptr) {
  return base::vsnprintf(buffer, buf_size, format, argptr);
}

template <class StringType>
static void StringAppendVT(StringType* dst,
                           const typename StringType::value_type* format,
                           va_list ap) {
  typename StringType::value_type stack_buf[1024];

  va_list ap_copy;
  GG_VA_COPY(ap_copy, ap);

#if !defined(OS_WIN)
  errno = 0;
#endif
  int result = vsnprintfT(stack_buf, arraysize(stack_buf), format, ap_copy);
  va_end(ap_copy);

  if (result >= 0 && result < static_cast<int>(arraysize(stack_buf))) {
    dst->append(stack_buf, result);
    return;
  }

  int mem_length = arraysize(stack_buf);
  while (true) {
    if (result < 0) {
#if !defined(OS_WIN)
      if (errno != 0 && errno != EOVERFLOW)
#endif
      {
        DLOG(WARNING) << "Unable to printf the requested string due to error.";
        return;
      }
      mem_length *= 2;
    } else {
      mem_length = result + 1;
    }

    if (mem_length > 32 * 1024 * 1024) {
      DLOG(WARNING) << "Unable to printf the requested string due to size.";
      return;
    }

    std::vector<typename StringType::value_type> mem_buf(mem_length);

    GG_VA_COPY(ap_copy, ap);
    result = vsnprintfT(&mem_buf[0], mem_length, format, ap_copy);
    va_end(ap_copy);

    if ((result >= 0) && (result < mem_length)) {
      dst->append(&mem_buf[0], result);
      return;
    }
  }
}

}  // namespace

std::string StringPrintf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  std::string result;
  StringAppendV(&result, format, ap);
  va_end(ap);
  return result;
}

void StringAppendV(std::string* dst, const char* format, va_list ap) {
  StringAppendVT(dst, format, ap);
}

}  // namespace base

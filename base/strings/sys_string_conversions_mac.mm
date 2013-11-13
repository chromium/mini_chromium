// Copyright 2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/strings/sys_string_conversions.h"

#include <vector>

namespace base {

namespace {

const CFStringEncoding kNarrowStringEncoding = kCFStringEncodingUTF8;

template<typename StringType>
StringType CFStringToSTLStringWithEncodingT(CFStringRef cfstring,
                                            CFStringEncoding encoding) {
  CFIndex length = CFStringGetLength(cfstring);
  if (length == 0)
    return StringType();

  CFRange whole_string = CFRangeMake(0, length);
  CFIndex out_size;
  CFIndex converted = CFStringGetBytes(cfstring,
                                       whole_string,
                                       encoding,
                                       0,
                                       FALSE,
                                       NULL,
                                       0,
                                       &out_size);
  if (converted == 0 || out_size == 0)
    return StringType();

  typename StringType::size_type elements =
      out_size * sizeof(UInt8) / sizeof(typename StringType::value_type) + 1;

  std::vector<typename StringType::value_type> out_buffer(elements);
  converted = CFStringGetBytes(cfstring,
                               whole_string,
                               encoding,
                               0,
                               FALSE,
                               reinterpret_cast<UInt8*>(&out_buffer[0]),
                               out_size,
                               NULL);
  if (converted == 0)
    return StringType();

  out_buffer[elements - 1] = '\0';
  return StringType(&out_buffer[0], elements - 1);
}

}  // namespace

std::string SysCFStringRefToUTF8(CFStringRef ref) {
  return CFStringToSTLStringWithEncodingT<std::string>(ref,
                                                       kNarrowStringEncoding);
}

}  // namespace base

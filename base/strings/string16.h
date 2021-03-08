// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_STRINGS_STRING16_H_
#define MINI_CHROMIUM_BASE_STRINGS_STRING16_H_

#include <stdint.h>
#include <stdio.h>

#include <string>

namespace base {

using char16 = char16_t;
using string16 = std::u16string;

}  // namespace base

namespace std {
std::ostream& operator<<(std::ostream& out, const std::u16string& str);
}  // namespace std

#endif  // MINI_CHROMIUM_BASE_STRINGS_STRING16_H_

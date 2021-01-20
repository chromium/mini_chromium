// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/strings/string16.h"

#include <string.h>

#if defined(WCHAR_T_IS_UTF32)

#include <ostream>

#include "base/strings/utf_string_conversions.h"

std::ostream& operator<<(std::ostream& out, const std::u16string& str) {
  return out << base::UTF16ToUTF8(str);
}

#endif  // WCHAR_T_IS_UTF32

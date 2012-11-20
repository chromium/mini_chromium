// Copyright 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_UTF_STRING_CONVERSIONS_H_
#define MINI_CHROMIUM_BASE_UTF_STRING_CONVERSIONS_H_

#include <string>

#include "base/string16.h"

bool UTF8ToUTF16(const char* src, size_t src_len, string16* output);
string16 UTF8ToUTF16(const std::string& utf8);
bool UTF16ToUTF8(const char16* src, size_t src_len, std::string* output);
std::string UTF16ToUTF8(const string16& utf16);

#endif  // MINI_CHROMIUM_BASE_UTF_STRING_CONVERSIONS_H_

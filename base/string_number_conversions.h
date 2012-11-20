// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_STRING_NUMBER_CONVERSIONS_H_
#define MINI_CHROMIUM_BASE_STRING_NUMBER_CONVERSIONS_H_

#include <stdint.h>

#include <string>
#include <vector>

namespace base {

bool StringToInt(const std::string& input, int* output);
bool StringToInt(const char* begin, const char* end, int* output);

bool StringToUint(const std::string& input, unsigned int* output);
bool StringToUint(const char* begin, const char* end, unsigned int* output);

bool StringToInt64(const std::string& input, int64_t* output);
bool StringToInt64(const char* begin, const char* end, int64_t* output);

bool StringToUint64(const std::string& input, uint64_t* output);
bool StringToUint64(const char* begin, const char* end, uint64_t* output);

bool StringToSizeT(const std::string& input, size_t* output);
bool StringToSizeT(const char* begin, const char* end, size_t* output);

bool HexStringToInt(const std::string& input, int* output);
bool HexStringToBytes(const std::string& input, std::vector<uint8_t>* output);

}  // namespace base

#endif  // MINI_CHROMIUM_BASE_STRING_NUMBER_CONVERSIONS_H_

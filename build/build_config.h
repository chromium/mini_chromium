// Copyright 2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BUILD_BUILD_CONFIG_H_
#define MINI_CHROMIUM_BUILD_BUILD_CONFIG_H_

#if defined(__APPLE__)
#define OS_MACOSX 1
#elif defined(__linux__)
#define OS_LINUX 1
#elif defined(_WIN32)
#define OS_WIN 1
#else
#error Please add support for your platform in build/build_config.h
#endif

#if defined(OS_MACOSX) || defined(OS_LINUX)
#define OS_POSIX 1
#endif

// Compiler detection.
#if defined(__GNUC__)
#define COMPILER_GCC 1
#elif defined(_MSC_VER)
#define COMPILER_MSVC 1
#else
#error Please add support for your compiler in build/build_config.h
#endif

#if defined(_M_X64) || defined(__x86_64__)
#define ARCH_CPU_X86_FAMILY 1
#define ARCH_CPU_X86_64 1
#define ARCH_CPU_64_BITS 1
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(_M_IX86) || defined(__i386__)
#define ARCH_CPU_X86_FAMILY 1
#define ARCH_CPU_X86 1
#define ARCH_CPU_32_BITS 1
#define ARCH_CPU_LITTLE_ENDIAN 1
#else
#error Please add support for your architecture in build/build_config.h
#endif

#if defined(OS_POSIX) && defined(COMPILER_GCC) && \
    defined(__WCHAR_MAX__) && \
    (__WCHAR_MAX__ == 0x7fffffff || __WCHAR_MAX__ == 0xffffffff)
#define WCHAR_T_IS_UTF32
#elif defined(OS_WIN)
#define WCHAR_T_IS_UTF16
#else
#error Please add support for your compiler in build/build_config.h
#endif

#endif  // MINI_CHROMIUM_BUILD_BUILD_CONFIG_H_

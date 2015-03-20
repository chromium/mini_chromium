// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"

#include <stdio.h>
#include <stdlib.h>

#include <iomanip>

#if defined(OS_POSIX)
#include <sys/time.h>
#include <unistd.h>
#include "base/safe_strerror_posix.h"
#endif  // OS_POSIX

#if defined(OS_MACOSX)
#include <pthread.h>
#elif defined(OS_LINUX)
#include <sys/syscall.h>
#include <sys/types.h>
#elif defined(OS_WIN)
#include <windows.h>
#endif

#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"

namespace logging {

namespace {

const char* const log_severity_names[] = {
  "INFO",
  "WARNING",
  "ERROR",
  "ERROR_REPORT",
  "FATAL"
};

}  // namespace

#if defined(OS_WIN)
std::string SystemErrorCodeToString(unsigned long error_code) {
  char msgbuf[256];
  DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
                FORMAT_MESSAGE_MAX_WIDTH_MASK;
  DWORD len = FormatMessageA(flags, NULL, error_code, 0, msgbuf,
                             arraysize(msgbuf), NULL);
  if (len) {
    return msgbuf + base::StringPrintf(" (0x%X)", error_code);
  }
  return base::StringPrintf("Error (0x%X) while retrieving error. (0x%X)",
                            GetLastError(), error_code);
}
#endif  // OS_WIN

LogMessage::LogMessage(const char* function,
                       const char* file_path,
                       int line,
                       LogSeverity severity)
    : severity_(severity) {
  Init(function, file_path, line);
}

LogMessage::LogMessage(const char* function,
                       const char* file_path,
                       int line,
                       std::string* result)
    : severity_(LOG_FATAL) {
  Init(function, file_path, line);
  stream_ << "Check failed: " << *result << ". ";
  delete result;
}

LogMessage::~LogMessage() {
  stream_ << std::endl;
  std::string str_newline(stream_.str());
  fprintf(stderr, "%s", str_newline.c_str());
  fflush(stderr);
  if (severity_ == LOG_FATAL) {
#ifndef NDEBUG
    abort();
#else
#if defined(OS_WIN)
    __debugbreak();
#else
    __asm__("int3");
#endif
#endif
  }
}

void LogMessage::Init(const char* function,
                      const std::string& file_path,
                      int line) {
  std::string file_name;
  size_t last_slash = file_path.find_last_of('/');
  if (last_slash != std::string::npos) {
    file_name.assign(file_path.substr(last_slash + 1));
  } else {
    file_name.assign(file_path);
  }

#if defined(OS_POSIX)
  pid_t pid = getpid();
#elif defined(OS_WIN)
  DWORD pid = GetCurrentProcessId();
#endif

#if defined(OS_MACOSX)
  uint64_t thread;
  pthread_threadid_np(pthread_self(), &thread);
#elif defined(OS_LINUX)
  pid_t thread = syscall(__NR_gettid);
#elif defined(OS_WIN)
  DWORD thread = GetCurrentThreadId();
#endif

  stream_ << '['
          << pid
          << ':'
          << thread
          << ':'
          << std::setfill('0');

#if defined(OS_POSIX)
  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm local_time;
  localtime_r(&tv.tv_sec, &local_time);
  stream_ << std::setw(4) << local_time.tm_year + 1900
          << std::setw(2) << local_time.tm_mon + 1
          << std::setw(2) << local_time.tm_mday
          << ','
          << std::setw(2) << local_time.tm_hour
          << std::setw(2) << local_time.tm_min
          << std::setw(2) << local_time.tm_sec
          << '.'
          << std::setw(6) << tv.tv_usec;
#elif defined(OS_WIN)
  SYSTEMTIME local_time;
  GetLocalTime(&local_time);
  stream_ << std::setw(4) << local_time.wYear
          << std::setw(2) << local_time.wMonth
          << std::setw(2) << local_time.wDay
          << ','
          << std::setw(2) << local_time.wHour
          << std::setw(2) << local_time.wMinute
          << std::setw(2) << local_time.wSecond
          << '.'
          << std::setw(3) << local_time.wMilliseconds;
#endif

  stream_ << ':';

  if (severity_ >= 0) {
    stream_ << log_severity_names[severity_];
  } else {
    stream_ << "VERBOSE" << -severity_;
  }

  stream_ << ' '
          << file_name
          << ':'
          << line
          << "] ";
}

#if defined(OS_WIN)

unsigned long GetLastSystemErrorCode() {
  return ::GetLastError();
}

Win32ErrorLogMessage::Win32ErrorLogMessage(const char* function,
                                           const char* file_path,
                                           int line,
                                           LogSeverity severity,
                                           unsigned long err)
    : LogMessage(function, file_path, line, severity), err_(err) {
}


Win32ErrorLogMessage::~Win32ErrorLogMessage() {
  stream() << ": " << SystemErrorCodeToString(err_);
}

#elif defined(OS_POSIX)

ErrnoLogMessage::ErrnoLogMessage(const char* function,
                                 const char* file_path,
                                 int line,
                                 LogSeverity severity,
                                 int err)
    : LogMessage(function, file_path, line, severity),
      err_(err) {
}

ErrnoLogMessage::~ErrnoLogMessage() {
  stream() << ": "
           << safe_strerror(err_)
           << " ("
           << err_
           << ")";
}

#endif  // OS_POSIX

}  // namespace logging

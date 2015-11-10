// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"

#include <stdio.h>
#include <stdlib.h>

#include <iomanip>

#if defined(OS_POSIX)
#include <paths.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include "base/posix/safe_strerror.h"
#endif  // OS_POSIX

#if defined(OS_MACOSX)
#include <asl.h>
#include <CoreFoundation/CoreFoundation.h>
#include <pthread.h>
#elif defined(OS_LINUX)
#include <sys/syscall.h>
#include <sys/types.h>
#elif defined(OS_WIN)
#include <windows.h>
#endif

#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"

namespace logging {

namespace {

const char* const log_severity_names[] = {
  "INFO",
  "WARNING",
  "ERROR",
  "ERROR_REPORT",
  "FATAL"
};

LogMessageHandlerFunction g_log_message_handler = nullptr;

}  // namespace

void SetLogMessageHandler(LogMessageHandlerFunction log_message_handler) {
  g_log_message_handler = log_message_handler;
}

LogMessageHandlerFunction GetLogMessageHandler() {
  return g_log_message_handler;
}

#if defined(OS_WIN)
std::string SystemErrorCodeToString(unsigned long error_code) {
  wchar_t msgbuf[256];
  DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
                FORMAT_MESSAGE_MAX_WIDTH_MASK;
  DWORD len = FormatMessage(
      flags, nullptr, error_code, 0, msgbuf, arraysize(msgbuf), nullptr);
  if (len) {
    // Most system messages end in a period and a space. Remove the space if
    // it’s there, because the following StringPrintf() includes one.
    if (len >= 1 && msgbuf[len - 1] == ' ') {
      msgbuf[len - 1] = '\0';
    }
    return base::StringPrintf("%s (%u)",
                              base::UTF16ToUTF8(msgbuf).c_str(), error_code);
  }
  return base::StringPrintf("Error %u while retrieving error %u",
                            GetLastError(),
                            error_code);
}
#endif  // OS_WIN

LogMessage::LogMessage(const char* function,
                       const char* file_path,
                       int line,
                       LogSeverity severity)
    : stream_(),
      file_path_(file_path),
      message_start_(0),
      line_(line),
      severity_(severity) {
  Init(function);
}

LogMessage::LogMessage(const char* function,
                       const char* file_path,
                       int line,
                       std::string* result)
    : stream_(),
      file_path_(file_path),
      message_start_(0),
      line_(line),
      severity_(LOG_FATAL) {
  Init(function);
  stream_ << "Check failed: " << *result << ". ";
  delete result;
}

LogMessage::~LogMessage() {
  stream_ << std::endl;
  std::string str_newline(stream_.str());

  if (g_log_message_handler &&
      g_log_message_handler(
          severity_, file_path_, line_, message_start_, str_newline)) {
    return;
  }

  fprintf(stderr, "%s", str_newline.c_str());
  fflush(stderr);

#if defined(OS_MACOSX)
  const bool log_via_asl = []() {
    struct stat stderr_stat;
    if (fstat(fileno(stderr), &stderr_stat) == -1) {
      return true;
    }
    if (!S_ISCHR(stderr_stat.st_mode)) {
      return false;
    }

    struct stat dev_null_stat;
    if (stat(_PATH_DEVNULL, &dev_null_stat) == -1) {
      return true;
    }

    return !S_ISCHR(dev_null_stat.st_mode) ||
           stderr_stat.st_rdev == dev_null_stat.st_rdev;
  }();

  if (log_via_asl) {
    CFBundleRef main_bundle = CFBundleGetMainBundle();
    CFStringRef main_bundle_id_cf =
        main_bundle ? CFBundleGetIdentifier(main_bundle) : nullptr;

    std::string main_bundle_id_buf;
    const char* asl_facility = nullptr;

    if (main_bundle_id_cf) {
      asl_facility =
          CFStringGetCStringPtr(main_bundle_id_cf, kCFStringEncodingUTF8);
      if (!asl_facility) {
        // 1024 is from 10.10.5 CF-1153.18/CFBundle.c __CFBundleMainID__ (at
        // the point of use, not declaration).
        main_bundle_id_buf.resize(1024);
        if (!CFStringGetCString(main_bundle_id_cf,
                                &main_bundle_id_buf[0],
                                main_bundle_id_buf.size(),
                                kCFStringEncodingUTF8)) {
          main_bundle_id_buf.clear();
        } else {
          asl_facility = &main_bundle_id_buf[0];
        }
      }
    }

    if (!asl_facility) {
      asl_facility = "com.apple.console";
    }

    class ASLClient {
     public:
      explicit ASLClient(const char* asl_facility)
          : client_(asl_open(nullptr, asl_facility, ASL_OPT_NO_DELAY)) {}
      ~ASLClient() { asl_close(client_); }

      aslclient get() const { return client_; }

     private:
      aslclient client_;
      DISALLOW_COPY_AND_ASSIGN(ASLClient);
    } asl_client(asl_facility);

    class ASLMessage {
     public:
      ASLMessage() : message_(asl_new(ASL_TYPE_MSG)) {}
      ~ASLMessage() { asl_free(message_); }

      aslmsg get() const { return message_; }

     private:
      aslmsg message_;
      DISALLOW_COPY_AND_ASSIGN(ASLMessage);
    } asl_message;

    // By default, messages are only readable by the admin group. Explicitly
    // make them readable by the user generating the messages.
    char euid_string[12];
    snprintf(euid_string, arraysize(euid_string), "%d", geteuid());
    asl_set(asl_message.get(), ASL_KEY_READ_UID, euid_string);

    // Map Chrome log severities to ASL log levels.
    const char* const asl_level_string = [](LogSeverity severity) {
#define ASL_LEVEL_STR(level) ASL_LEVEL_STR_X(level)
#define ASL_LEVEL_STR_X(level) #level
        switch (severity) {
          case LOG_INFO:
            return ASL_LEVEL_STR(ASL_LEVEL_INFO);
          case LOG_WARNING:
            return ASL_LEVEL_STR(ASL_LEVEL_WARNING);
          case LOG_ERROR:
            return ASL_LEVEL_STR(ASL_LEVEL_ERR);
          case LOG_FATAL:
            return ASL_LEVEL_STR(ASL_LEVEL_CRIT);
          default:
            return severity < 0 ? ASL_LEVEL_STR(ASL_LEVEL_DEBUG)
                                : ASL_LEVEL_STR(ASL_LEVEL_NOTICE);
        }
#undef ASL_LEVEL_STR
#undef ASL_LEVEL_STR_X
    }(severity_);
    asl_set(asl_message.get(), ASL_KEY_LEVEL, asl_level_string);

    asl_set(asl_message.get(), ASL_KEY_MSG, str_newline.c_str());

    asl_send(asl_client.get(), asl_message.get());
  }
#elif defined(OS_WIN)
  OutputDebugString(base::UTF8ToUTF16(str_newline).c_str());
#endif  // OS_MACOSX

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

void LogMessage::Init(const char* function) {
  std::string file_name(file_path_);
#if defined(OS_WIN)
  size_t last_slash = file_name.find_last_of("\\/");
#else
  size_t last_slash = file_name.find_last_of('/');
#endif
  if (last_slash != std::string::npos) {
    file_name.assign(file_name.substr(last_slash + 1));
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
  timeval tv;
  gettimeofday(&tv, nullptr);
  tm local_time;
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
          << line_
          << "] ";

  message_start_ = stream_.str().size();
}

#if defined(OS_WIN)

unsigned long GetLastSystemErrorCode() {
  return GetLastError();
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
           << base::safe_strerror(err_)
           << " ("
           << err_
           << ")";
}

#endif  // OS_POSIX

}  // namespace logging

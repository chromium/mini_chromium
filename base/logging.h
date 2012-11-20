// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_LOGGING_H_
#define MINI_CHROMIUM_BASE_LOGGING_H_

#include <assert.h>
#include <errno.h>

#include <limits>
#include <sstream>
#include <string>

#include "base/basictypes.h"

namespace logging {

typedef int LogSeverity;
const LogSeverity LOG_VERBOSE = -1;
const LogSeverity LOG_INFO = 0;
const LogSeverity LOG_WARNING = 1;
const LogSeverity LOG_ERROR = 2;
const LogSeverity LOG_ERROR_REPORT = 3;
const LogSeverity LOG_FATAL = 4;
const LogSeverity LOG_NUM_SEVERITIES = 5;

#if defined(NDEBUG)
const LogSeverity LOG_DFATAL = LOG_ERROR;
#else
const LogSeverity LOG_DFATAL = LOG_FATAL;
#endif

static inline int GetMinLogLevel() {
  return LOG_INFO;
}

static inline int GetVlogLevel(const char* file) {
  return std::numeric_limits<int>::max();
}

static inline int GetLastSystemErrorCode() {
  return errno;
}

template<typename t1, typename t2>
std::string* MakeCheckOpString(const t1& v1, const t2& v2, const char* names) {
  std::ostringstream ss;
  ss << names << " (" << v1 << " vs. " << v2 << ")";
  std::string* msg = new std::string(ss.str());
  return msg;
}

#define DEFINE_CHECK_OP_IMPL(name, op) \
    template <typename t1, typename t2> \
    inline std::string* Check ## name ## Impl(const t1& v1, const t2& v2, \
                                              const char* names) { \
      if (v1 op v2) { \
        return NULL; \
      } else { \
        return MakeCheckOpString(v1, v2, names); \
      } \
    } \
    inline std::string* Check ## name ## Impl(int v1, int v2, \
                                              const char* names) { \
      if (v1 op v2) { \
        return NULL; \
      } else { \
        return MakeCheckOpString(v1, v2, names); \
      } \
    }

DEFINE_CHECK_OP_IMPL(EQ, ==)
DEFINE_CHECK_OP_IMPL(NE, !=)
DEFINE_CHECK_OP_IMPL(LE, <=)
DEFINE_CHECK_OP_IMPL(LT, <)
DEFINE_CHECK_OP_IMPL(GE, >=)
DEFINE_CHECK_OP_IMPL(GT, >)

#undef DEFINE_CHECK_OP_IMPL

class LogMessage {
 public:
  LogMessage(const char* function,
             const char* file_path,
             int line,
             LogSeverity severity);
  LogMessage(const char* function,
             const char* file_path,
             int line,
             std::string* result);
  ~LogMessage();

  std::ostream& stream() { return stream_; }

 private:
  void Init(const char* function, const std::string& file_path, int line);

  std::ostringstream stream_;
  LogSeverity severity_;

  DISALLOW_COPY_AND_ASSIGN(LogMessage);
};

class LogMessageVoidify {
 public:
  LogMessageVoidify() {}

  void operator&(const std::ostream&) const {}
};

class ErrnoLogMessage : public LogMessage {
 public:
  ErrnoLogMessage(const char* function,
                  const char* file_path,
                  int line,
                  LogSeverity severity,
                  int err);
  ~ErrnoLogMessage();

 private:
  int err_;

  DISALLOW_COPY_AND_ASSIGN(ErrnoLogMessage);
};

}  // namespace logging

#define COMPACT_GOOGLE_LOG_EX_INFO(ClassName, ...) \
    logging::ClassName(__PRETTY_FUNCTION__, __FILE__, __LINE__, \
                       logging::LOG_INFO, ## __VA_ARGS__)
#define COMPACT_GOOGLE_LOG_EX_WARNING(ClassName, ...) \
    logging::ClassName(__PRETTY_FUNCTION__, __FILE__, __LINE__, \
                       logging::LOG_WARNING, ## __VA_ARGS__)
#define COMPACT_GOOGLE_LOG_EX_ERROR(ClassName, ...) \
    logging::ClassName(__PRETTY_FUNCTION__, __FILE__, __LINE__, \
                       logging::LOG_ERROR, ## __VA_ARGS__)
#define COMPACT_GOOGLE_LOG_EX_ERROR_REPORT(ClassName, ...) \
    logging::ClassName(__PRETTY_FUNCTION__, __FILE__, __LINE__, \
                       logging::LOG_ERROR_REPORT, ## __VA_ARGS__)
#define COMPACT_GOOGLE_LOG_EX_FATAL(ClassName, ...) \
    logging::ClassName(__PRETTY_FUNCTION__, __FILE__, __LINE__, \
                       logging::LOG_FATAL, ## __VA_ARGS__)
#define COMPACT_GOOGLE_LOG_EX_DFATAL(ClassName, ...) \
    logging::ClassName(__PRETTY_FUNCTION__, __FILE__, __LINE__, \
                       logging::LOG_DFATAL, ## __VA_ARGS__)

#define COMPACT_GOOGLE_LOG_INFO \
    COMPACT_GOOGLE_LOG_EX_INFO(LogMessage)
#define COMPACT_GOOGLE_LOG_WARNING \
    COMPACT_GOOGLE_LOG_EX_WARNING(LogMessage)
#define COMPACT_GOOGLE_LOG_ERROR \
    COMPACT_GOOGLE_LOG_EX_ERROR(LogMessage)
#define COMPACT_GOOGLE_LOG_ERROR_REPORT \
    COMPACT_GOOGLE_LOG_EX_ERROR_REPORT(LogMessage)
#define COMPACT_GOOGLE_LOG_FATAL \
    COMPACT_GOOGLE_LOG_EX_FATAL(LogMessage)
#define COMPACT_GOOGLE_LOG_DFATAL \
    COMPACT_GOOGLE_LOG_EX_DFATAL(LogMessage)

#define LAZY_STREAM(stream, condition) \
    !(condition) ? (void) 0 : ::logging::LogMessageVoidify() & (stream)

#define LOG_IS_ON(severity) \
    ((::logging::LOG_ ## severity) >= ::logging::GetMinLogLevel())
#define VLOG_IS_ON(verbose_level) \
    ((verbose_level) <= ::logging::GetVlogLevel(__FILE__))

#define LOG_STREAM(severity) COMPACT_GOOGLE_LOG_ ## severity.stream()
#define VLOG_STREAM(verbose_level) \
    logging::LogMessage(__PRETTY_FUNCTION__, __FILE__, __LINE__, \
                        -verbose_level).stream()
#define PLOG_STREAM(severity) COMPACT_GOOGLE_LOG_EX_ ## severity( \
    ErrnoLogMessage, ::logging::GetLastSystemErrorCode()).stream()
#define VPLOG_STREAM(verbose_level) \
    logging::ErrnoLogMessage(__PRETTY_FUNCTION__, __FILE__, __LINE__, \
                             -verbose_level, \
                             ::logging::GetLastSystemErrorCode()).stream()

#define LOG(severity) LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity))
#define LOG_IF(severity, condition) \
    LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity) && (condition))
#define LOG_ASSERT(condition) \
    LOG_IF(FATAL, !(condition)) << "Assertion failed: " # condition ". "

#define VLOG(verbose_level) \
    LAZY_STREAM(VLOG_STREAM(verbose_level), VLOG_IS_ON(verbose_level))
#define VLOG_IF(verbose_level, condition) \
    LAZY_STREAM(VLOG_STREAM(verbose_level), \
                VLOG_IS_ON(verbose_level) && (condition))

#define PLOG(severity) LAZY_STREAM(PLOG_STREAM(severity), LOG_IS_ON(severity))
#define PLOG_IF(severity, condition) \
    LAZY_STREAM(PLOG_STREAM(severity), LOG_IS_ON(severity) && (condition))

#define VPLOG(verbose_level) \
    LAZY_STREAM(VPLOG_STREAM(verbose_level), VLOG_IS_ON(verbose_level))
#define VPLOG_IF(verbose_level, condition) \
    LAZY_STREAM(VPLOG_STREAM(verbose_level), \
                VLOG_IS_ON(verbose_level) && (condition))

#define CHECK(condition) \
    LAZY_STREAM(LOG_STREAM(FATAL), !(condition)) \
    << "Check failed: " # condition << ". "
#define PCHECK(condition) \
    LAZY_STREAM(PLOG_STREAM(FATAL), !(condition)) \
    << "Check failed: " # condition << ". "

#define CHECK_OP(name, op, val1, val2) \
    if (std::string* _result = \
          logging::Check ## name ## Impl((val1), (val2), \
                                         # val1 " " # op " " # val2)) \
      logging::LogMessage(__PRETTY_FUNCTION__, __FILE__, __LINE__, \
                          _result).stream()

#define CHECK_EQ(val1, val2) CHECK_OP(EQ, ==, val1, val2)
#define CHECK_NE(val1, val2) CHECK_OP(NE, !=, val1, val2)
#define CHECK_LE(val1, val2) CHECK_OP(LE, <=, val1, val2)
#define CHECK_LT(val1, val2) CHECK_OP(LT, <, val1, val2)
#define CHECK_GE(val1, val2) CHECK_OP(GE, >=, val1, val2)
#define CHECK_GT(val1, val2) CHECK_OP(GT, >, val1, val2)

#if defined(NDEBUG)
#define DLOG_IS_ON(severity) false
#define DVLOG_IS_ON(verbose_level) false
#define DCHECK_IS_ON() false
#else
#define DLOG_IS_ON(severity) LOG_IS_ON(severity)
#define DVLOG_IS_ON(verbose_level) VLOG_IS_ON(verbose_level)
#define DCHECK_IS_ON() true
#endif

#define DLOG(severity) LAZY_STREAM(LOG_STREAM(severity), DLOG_IS_ON(severity))
#define DLOG_IF(severity, condition) \
    LAZY_STREAM(LOG_STREAM(severity), DLOG_IS_ON(severity) && (condition))
#define DLOG_ASSERT(condition) \
    DLOG_IF(FATAL, !(condition)) << "Assertion failed: " # condition ". "

#define DVLOG(verbose_level) \
    LAZY_STREAM(VLOG_STREAM(verbose_level), DVLOG_IS_ON(verbose_level))
#define DVLOG_IF(verbose_level, condition) \
    LAZY_STREAM(VLOG_STREAM(verbose_level), \
                DVLOG_IS_ON(verbose_level) && (condition))

#define DPLOG(severity) LAZY_STREAM(PLOG_STREAM(severity), DLOG_IS_ON(severity))
#define DPLOG_IF(severity, condition) \
    LAZY_STREAM(PLOG_STREAM(severity), DLOG_IS_ON(severity) && (condition))

#define DVPLOG(verbose_level) \
    LAZY_STREAM(VPLOG_STREAM(verbose_level), DVLOG_IS_ON(verbose_level))
#define DVPLOG_IF(verbose_level, condition) \
    LAZY_STREAM(VPLOG_STREAM(verbose_level), \
                DVLOG_IS_ON(verbose_level) && (condition))

#define DCHECK(condition) \
    LAZY_STREAM(LOG_STREAM(FATAL), DCHECK_IS_ON() && !(condition)) \
    << "Check failed: " # condition << ". "
#define DPCHECK(condition) \
    LAZY_STREAM(PLOG_STREAM(FATAL), DCHECK_IS_ON() && !(condition)) \
    << "Check failed: " # condition << ". "

#define DCHECK_OP(name, op, val1, val2) \
    if (DCHECK_IS_ON()) \
      if (std::string* _result = \
          logging::Check ## name ## Impl((val1), (val2), \
                                         # val1 " " # op " " # val2)) \
        logging::LogMessage(__PRETTY_FUNCTION__, __FILE__, __LINE__, \
                            _result).stream()

#define DCHECK_EQ(val1, val2) DCHECK_OP(EQ, ==, val1, val2)
#define DCHECK_NE(val1, val2) DCHECK_OP(NE, !=, val1, val2)
#define DCHECK_LE(val1, val2) DCHECK_OP(LE, <=, val1, val2)
#define DCHECK_LT(val1, val2) DCHECK_OP(LT, <, val1, val2)
#define DCHECK_GE(val1, val2) DCHECK_OP(GE, >=, val1, val2)
#define DCHECK_GT(val1, val2) DCHECK_OP(GT, >, val1, val2)

#define NOTREACHED() DCHECK(false)

#undef assert
#define assert(condition) DLOG_ASSERT(condition)

#endif  // MINI_CHROMIUM_BASE_LOGGING_H_

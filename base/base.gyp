# Copyright 2009 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'includes': [
    '../build/common.gypi',
  ],

  'targets': [
    {
      'target_name': 'base',
      'type': 'static_library',
      'include_dirs': [
        '..',
      ],
      'sources': [
        'basictypes.h',
        'compiler_specific.h',
        'eintr_wrapper.h',
        'file_util.h',
        'logging.cc',
        'logging.h',
        'mac/foundation_util.h',
        'mac/foundation_util.mm',
        'mac/scoped_cftyperef.h',
        'mac/scoped_ioobject.h',
        'mac/scoped_launch_data.h',
        'mac/scoped_mach_port.cc',
        'mac/scoped_mach_port.h',
        'mac/scoped_nsautorelease_pool.h',
        'mac/scoped_nsautorelease_pool.mm',
        'mac/scoped_nsobject.h',
        'memory/scoped_policy.h',
        'memory/scoped_ptr.h',
        'move.h',
        'port.h',
        'safe_strerror_posix.cc',
        'safe_strerror_posix.h',
        'strings/string16.cc',
        'strings/string16.h',
        'strings/string_number_conversions.cc',
        'strings/string_number_conversions.h',
        'strings/string_util.h',
        'strings/string_util_posix.h',
        'strings/stringprintf.cc',
        'strings/stringprintf.h',
        'strings/utf_string_conversion_utils.cc',
        'strings/utf_string_conversion_utils.h',
        'strings/utf_string_conversions.cc',
        'strings/utf_string_conversions.h',
        'sys_byteorder.h',
        'sys_string_conversions.h',
        'sys_string_conversions_mac.mm',
        'third_party/icu/icu_utf.cc',
        'third_party/icu/icu_utf.h',
        '../build/build_config.h',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '..',
        ],
      },
    },
  ],
}

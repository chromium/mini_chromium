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
      'link_settings': {
        'conditions': [
          ['OS=="mac"', {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/ApplicationServices.framework',
              '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
              '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
              '$(SDKROOT)/System/Library/Frameworks/Security.framework',
            ],
          }],
        ],
      },
      'sources': [
        'basictypes.h',
        'compiler_specific.h',
        'file_util.cc',
        'file_util.h',
        'files/scoped_file.h',
        'logging.cc',
        'logging.h',
        'mac/foundation_util.h',
        'mac/foundation_util.mm',
        'mac/mach_logging.h',
        'mac/mach_logging.cc',
        'mac/scoped_cftyperef.h',
        'mac/scoped_ioobject.h',
        'mac/scoped_launch_data.h',
        'mac/scoped_mach_port.cc',
        'mac/scoped_mach_port.h',
        'mac/scoped_mach_vm.cc',
        'mac/scoped_mach_vm.h',
        'mac/scoped_nsautorelease_pool.h',
        'mac/scoped_nsautorelease_pool.mm',
        'mac/scoped_nsobject.h',
        'memory/scoped_policy.h',
        'memory/scoped_ptr.h',
        'move.h',
        'port.h',
        'posix/eintr_wrapper.h',
        'rand_util.cc',
        'rand_util.h',
        'safe_strerror_posix.cc',
        'safe_strerror_posix.h',
        'strings/string16.cc',
        'strings/string16.h',
        'strings/string_number_conversions.cc',
        'strings/string_number_conversions.h',
        'strings/string_piece.h',
        'strings/string_util.h',
        'strings/string_util_posix.h',
        'strings/stringprintf.cc',
        'strings/stringprintf.h',
        'strings/sys_string_conversions.h',
        'strings/sys_string_conversions_mac.mm',
        'strings/utf_string_conversion_utils.cc',
        'strings/utf_string_conversion_utils.h',
        'strings/utf_string_conversions.cc',
        'strings/utf_string_conversions.h',
        'sys_byteorder.h',
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

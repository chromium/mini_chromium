# Copyright 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'target_conditions': [
    ['OS!="mac"', {
      'sources/': [
        ['exclude', '_(cocoa|mac)(_unittest)?\\.(h|cc|mm?)$'],
        ['exclude', '(^|/)(cocoa|mac)/'],
      ],
    }],
    ['OS!="linux"', {
      'sources/': [
        ['exclude', '_linux(_unittest)?\\.(h|cc)$'],
        ['exclude', '(^|/)linux/'],
      ],
    }],
  ],
}

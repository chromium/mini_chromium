#!/usr/bin/env python

# Copyright 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from __future__ import print_function

import subprocess
import sys


def main(args):
  if len(args) != 0:
    print('usage: find_mac_sdk.py', file=sys.stderr)
    return 1

  subprocess.check_call(['xcrun', '--show-sdk-path'])
  return 0


if __name__ == '__main__':
  sys.exit(main(sys.argv[1:]))

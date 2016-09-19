// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_METRICS_SPARSE_HISTOGRAM_H_
#define MINI_CHROMIUM_BASE_METRICS_SPARSE_HISTOGRAM_H_

// This is a stub version of a macro in Chromium's
// base/metrics/sparse_histogram.h. This allows us to instrument the Crashpad
// code as necessary, while not affecting out-of-Chromium builds.
#define UMA_HISTOGRAM_SPARSE_SLOWLY(name, sample) \
  do {                                            \
    (void)(name);                                 \
    (void)(sample);                               \
  } while (false)

#endif  // MINI_CHROMIUM_BASE_METRICS_SPARSE_HISTOGRAM_H_

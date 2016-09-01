// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_METRICS_HISTOGRAM_MACROS_H_
#define MINI_CHROMIUM_BASE_METRICS_HISTOGRAM_MACROS_H_

// These are no-op stub versions of a subset of the macros from Chromium's
// base/metrics/histogram_macros.h. This allows us to instrument the Crashpad
// code as necessary, while not affecting out-of-Chromium builds.

#define UMA_HISTOGRAM_TIMES(name, sample)
#define UMA_HISTOGRAM_MEDIUM_TIMES(name, sample)
#define UMA_HISTOGRAM_LONG_TIMES(name, sample)
#define UMA_HISTOGRAM_LONG_TIMES_100(name, sample)
#define UMA_HISTOGRAM_CUSTOM_TIMES(name, sample, min, max, bucket_count)

#define UMA_HISTOGRAM_COUNTS(name, sample)
#define UMA_HISTOGRAM_COUNTS_100(name, sample)
#define UMA_HISTOGRAM_COUNTS_1000(name, sample)
#define UMA_HISTOGRAM_COUNTS_10000(name, sample)
#define UMA_HISTOGRAM_CUSTOM_COUNTS(name, sample, min, max, bucket_count)

#define UMA_HISTOGRAM_MEMORY_KB(name, sample)
#define UMA_HISTOGRAM_MEMORY_MB(name, sample)
#define UMA_HISTOGRAM_MEMORY_LARGE_MB(name, sample)

#define UMA_HISTOGRAM_PERCENTAGE(name, under_one_hundred)

#define UMA_HISTOGRAM_BOOLEAN(name, sample)

#define UMA_HISTOGRAM_ENUMERATION(name, sample, boundary_value)
#define UMA_STABILITY_HISTOGRAM_ENUMERATION(name, sample, boundary_value)
#define UMA_HISTOGRAM_CUSTOM_ENUMERATION(name, sample, custom_ranges)

#endif  // MINI_CHROMIUM_BASE_METRICS_HISTOGRAM_MACROS_H_

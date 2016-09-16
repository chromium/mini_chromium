// Copyright 2016 The Crashpad Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef MINI_CHROMIUM_BASE_METRICS_PERSISTENT_HISTOGRAM_ALLOCATOR_H_
#define MINI_CHROMIUM_BASE_METRICS_PERSISTENT_HISTOGRAM_ALLOCATOR_H_

#include <inttypes.h>
#include <sys/types.h>

#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/strings/string_piece.h"

// This file is a non-functional stub of the Chromium base interface to allow
// Crashpad to set up and tear down histogram storage when built against
// Chromium. When Crashpad is built standalone these stubs are used which
// silently do nothing.
namespace base {

class GlobalHistogramAllocator {
 public:
  static bool CreateWithActiveFileInDir(const base::FilePath&,
                                        size_t,
                                        uint64_t,
                                        base::StringPiece sp) {
    return false;
  }

  void CreateTrackingHistograms(base::StringPiece) {}
  void DeletePersistentLocation() {}

  static GlobalHistogramAllocator* Get() { return nullptr; }

 private:
  DISALLOW_COPY_AND_ASSIGN(GlobalHistogramAllocator);
};

}  // namespace base

#endif  // MINI_CHROMIUM_BASE_METRICS_PERSISTENT_HISTOGRAM_ALLOCATOR_H_

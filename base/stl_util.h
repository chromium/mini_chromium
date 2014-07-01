// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_STL_UTIL_H_
#define MINI_CHROMIUM_BASE_STL_UTIL_H_

#include <algorithm>
#include <functional>
#include <iterator>
#include <string>
#include <vector>

#include "base/logging.h"

template<class T>
void STLClearObject(T* obj) {
  T tmp;
  tmp.swap(*obj);
  obj->reserve(0);
}

template<class ForwardIterator>
void STLDeleteContainerPointers(ForwardIterator begin, ForwardIterator end) {
  while (begin != end) {
    ForwardIterator temp = begin;
    ++begin;
    delete *temp;
  }
}

template<class ForwardIterator>
void STLDeleteContainerPairPointers(ForwardIterator begin,
                                    ForwardIterator end) {
  while (begin != end) {
    ForwardIterator temp = begin;
    ++begin;
    delete temp->first;
    delete temp->second;
  }
}

template<class ForwardIterator>
void STLDeleteContainerPairFirstPointers(ForwardIterator begin,
                                         ForwardIterator end) {
  while (begin != end) {
    ForwardIterator temp = begin;
    ++begin;
    delete temp->first;
  }
}

template<class ForwardIterator>
void STLDeleteContainerPairSecondPointers(ForwardIterator begin,
                                          ForwardIterator end) {
  while (begin != end) {
    ForwardIterator temp = begin;
    ++begin;
    delete temp->second;
  }
}

template<typename T>
inline T* vector_as_array(std::vector<T>* v) {
  return v->empty() ? NULL : &*v->begin();
}

template<typename T>
inline const T* vector_as_array(const std::vector<T>* v) {
  return v->empty() ? NULL : &*v->begin();
}

inline char* string_as_array(std::string* str) {
  return str->empty() ? NULL : &*str->begin();
}

template<class T>
void STLDeleteElements(T* container) {
  if (!container) {
    return;
  }

  STLDeleteContainerPointers(container->begin(), container->end());
  container->clear();
}

template<class T>
void STLDeleteValues(T* container) {
  if (!container) {
    return;
  }

  for (typename T::iterator i(container->begin()); i != container->end(); ++i) {
    delete i->second;
  }

  container->clear();
}

template<class T>
class STLElementDeleter {
 public:
  STLElementDeleter<T>(T* container) : container_(container) {}
  ~STLElementDeleter<T>() { STLDeleteElements(container_); }

 private:
  T* container_;
};

template<class T>
class STLValueDeleter {
 public:
  STLValueDeleter<T>(T* container) : container_(container) {}
  ~STLValueDeleter<T>() { STLDeleteValues(container_); }

 private:
  T* container_;
};

template<typename Collection, typename Key>
bool ContainsKey(const Collection& collection, const Key& key) {
  return collection.find(key) != collection.end();
}

namespace base {

template<typename Container>
bool STLIsSorted(const Container& cont) {
  return std::adjacent_find(cont.rbegin(), cont.rend(),
                            std::less<typename Container::value_type>()) ==
         cont.rend();
}

template<typename ResultType, typename Arg1, typename Arg2>
ResultType STLSetDifference(const Arg1& a1, const Arg2& a2) {
  DCHECK(STLIsSorted(a1));
  DCHECK(STLIsSorted(a2));

  ResultType difference;
  std::set_difference(a1.begin(), a1.end(),
                      a2.begin(), a2.end(),
                      std::inserter(difference, difference.end()));
  return difference;
}

template<typename ResultType, typename Arg1, typename Arg2>
ResultType STLSetUnion(const Arg1& a1, const Arg2& a2) {
  DCHECK(STLIsSorted(a1));
  DCHECK(STLIsSorted(a2));

  ResultType result;
  std::set_union(a1.begin(), a1.end(),
                 a2.begin(), a2.end(),
                 std::inserter(result, result.end()));
  return result;
}

template<typename ResultType, typename Arg1, typename Arg2>
ResultType STLSetIntersection(const Arg1& a1, const Arg2& a2) {
  DCHECK(STLIsSorted(a1));
  DCHECK(STLIsSorted(a2));

  ResultType result;
  std::set_intersection(a1.begin(), a1.end(),
                        a2.begin(), a2.end(),
                        std::inserter(result, result.end()));
  return result;
}

template<typename Arg1, typename Arg2>
bool STLIncludes(const Arg1& a1, const Arg2& a2) {
  DCHECK(STLIsSorted(a1));
  DCHECK(STLIsSorted(a2));

  return std::includes(a1.begin(), a1.end(),
                       a2.begin(), a2.end());
}

}  // namespace base

#endif  // MINI_CHROMIUM_BASE_STL_UTIL_H_

// Copyright 2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// The LazyInstance<Type, Traits> class manages a single instance of Type,
// which will be lazily created on the first time it's accessed.  This class is
// useful for places you would normally use a function-level static, but you
// need to have guaranteed thread-safety.  The Type constructor will only ever
// be called once, even if two threads are racing to create the object.  Get()
// and Pointer() will always return the same, completely initialized instance.
//
// LazyInstance is completely thread safe, assuming that you create it safely.
// The class was designed to be POD initialized, so it shouldn't require a
// static constructor.  It really only makes sense to declare a LazyInstance as
// a global variable using the LAZY_INSTANCE_INITIALIZER initializer.
//
// LazyInstance is similar to Singleton, except it does not have the singleton
// property.  You can have multiple LazyInstance's of the same type, and each
// will manage a unique instance.  It also preallocates the space for Type, as
// to avoid allocating the Type instance on the heap.  This may help with the
// performance of creating the instance, and reducing heap fragmentation.  This
// requires that Type be a complete type so we can determine the size.
//
// Example usage:
//   static LazyInstance<MyClass> my_instance = LAZY_INSTANCE_INITIALIZER;
//   void SomeMethod() {
//     my_instance.Get().SomeMethod();  // MyClass::SomeMethod()
//
//     MyClass* ptr = my_instance.Pointer();
//     ptr->DoDoDo();  // MyClass::DoDoDo
//   }

#ifndef MINI_CHROMIUM_BASE_LAZY_INSTANCE_H_
#define MINI_CHROMIUM_BASE_LAZY_INSTANCE_H_

#include <new>

#include "base/atomicops.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/memory/aligned_memory.h"

// LazyInstance uses its own struct initializer-list style static
// initialization, as base's LINKER_INITIALIZED requires a constructor and on
// some compilers (notably gcc 4.4) this still ends up needing runtime
// initialization.
#define LAZY_INSTANCE_INITIALIZER {0}

namespace base {

template <typename Type>
struct DefaultLazyInstanceTraits {
  // mini_chromium does not support Traits::kRegisterOnExit = true because it
  // does not provide AtExitManager. Only LeakyLazyInstanceTraits is supported.
  static const bool kRegisterOnExit = true;

  static Type* New(void* instance) {
    DCHECK_EQ(reinterpret_cast<uintptr_t>(instance) & (ALIGNOF(Type) - 1), 0u);
    // Use placement new to initialize our instance in our preallocated space.
    // The parenthesis is very important here to force POD type initialization.
    return new (instance) Type();
  }
  static void Delete(Type* instance) {
    // Explicitly call the destructor.
    instance->~Type();
  }
};

// We pull out some of the functionality into non-templated functions, so we
// can implement the more complicated pieces out of line in the .cc file.
namespace internal {

// Use LazyInstance<T>::Leaky for a less-verbose call-site typedef; e.g.:
// base::LazyInstance<T>::Leaky my_leaky_lazy_instance;
// instead of:
// base::LazyInstance<T, base::internal::LeakyLazyInstanceTraits<T> >
// my_leaky_lazy_instance;
// (especially when T is MyLongTypeNameImplClientHolderFactory).
// Only use this internal::-qualified verbose form to extend this traits class
// (depending on its implementation details).
template <typename Type>
struct LeakyLazyInstanceTraits {
  static const bool kRegisterOnExit = false;

  static Type* New(void* instance) {
    return DefaultLazyInstanceTraits<Type>::New(instance);
  }
  static void Delete(Type* instance) {
  }
};

// Our AtomicWord doubles as a spinlock, where a value of
// kBeingCreatedMarker means the spinlock is being held for creation.
static const subtle::AtomicWord kLazyInstanceStateCreating = 1;

// Check if instance needs to be created. If so return true otherwise
// if another thread has beat us, wait for instance to be created and
// return false.
bool NeedsLazyInstance(subtle::AtomicWord* state);

// After creating an instance, call this to register the dtor to be called
// at program exit and to update the atomic state to hold the |new_instance|
void CompleteLazyInstance(subtle::AtomicWord* state,
                          subtle::AtomicWord new_instance,
                          void* lazy_instance);

}  // namespace internal

template <typename Type, typename Traits = DefaultLazyInstanceTraits<Type> >
class LazyInstance {
 public:
  // Do not define a destructor, as doing so makes LazyInstance a
  // non-POD-struct. We don't want that because then a static initializer will
  // be created to register the (empty) destructor with atexit() under MSVC, for
  // example.
  // ~LazyInstance() {}

  // Convenience typedef to avoid having to repeat Type for leaky lazy
  // instances.
  typedef LazyInstance<Type, internal::LeakyLazyInstanceTraits<Type> > Leaky;

  Type& Get() {
    return *Pointer();
  }

  Type* Pointer() {
    // If any bit in the created mask is true, the instance has already been
    // fully constructed.
    static const subtle::AtomicWord kLazyInstanceCreatedMask =
        ~internal::kLazyInstanceStateCreating;

    // We will hopefully have fast access when the instance is already created.
    // Since a thread sees private_instance_ == 0 or kLazyInstanceStateCreating
    // at most once, the load is taken out of NeedsInstance() as a fast-path.
    // The load has acquire memory ordering as a thread which sees
    // private_instance_ > creating needs to acquire visibility over
    // the associated data (private_buf_). Pairing Release_Store is in
    // CompleteLazyInstance().
    subtle::AtomicWord value = subtle::Acquire_Load(&private_instance_);
    if (!(value & kLazyInstanceCreatedMask) &&
        internal::NeedsLazyInstance(&private_instance_)) {
      // Create the instance in the space provided by |private_buf_|.
      value = reinterpret_cast<subtle::AtomicWord>(
          Traits::New(private_buf_.void_data()));
      internal::CompleteLazyInstance(&private_instance_, value, this);

      // mini_chromium does not support Traits::kRegisterOnExit = true because
      // it does not provide AtExitManager. Instead of LazyInstanceTraits, use
      // LeakyLazyInstanceTraits, and note that destructors will not run.
      static_assert(!Traits::kRegisterOnExit,
                    "destruction not supported in mini_chromium");
    }
    return instance();
  }

  bool operator==(Type* p) {
    switch (subtle::NoBarrier_Load(&private_instance_)) {
      case 0:
        return p == NULL;
      case internal::kLazyInstanceStateCreating:
        return static_cast<void*>(p) == private_buf_.void_data();
      default:
        return p == instance();
    }
  }

  // Effectively private: member data is only public to allow the linker to
  // statically initialize it and to maintain a POD class. DO NOT USE FROM
  // OUTSIDE THIS CLASS.

  subtle::AtomicWord private_instance_;
  // Preallocated space for the Type instance.
  base::AlignedMemory<sizeof(Type), ALIGNOF(Type)> private_buf_;

 private:
  Type* instance() {
    return reinterpret_cast<Type*>(subtle::NoBarrier_Load(&private_instance_));
  }
};

}  // namespace base

#endif  // MINI_CHROMIUM_BASE_LAZY_INSTANCE_H_

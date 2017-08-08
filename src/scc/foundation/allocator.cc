//===-- scc/foundation/allocator.cc ---------------------*- mode: C++11 -*-===//
//
//                             _____ _____ _____ 
//                            |   __|     |     |
//                            |__   |   --|   --|
//                            |_____|_____|_____|
//                          
//                           Shader Cross Compiler
//
//       This file is distributed under the terms described in LICENSE.
//
//===----------------------------------------------------------------------===//

#include "scc/foundation/allocator.h"

#include "scc/foundation/atomics.h"

SCC_BEGIN_EXTERN_C

// PERF(mtwilliams): Yield in spin-lock bodies?

#if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
  // Monotonically increase count of registered allocators.
  static scc_uint64_t registered_ = 0;

  // Linked-list of all allocators.
  static scc_allocator_t *allocators_ = NULL;

  // Signals to allocators that they should block while greater than zero.
  static scc_uint64_t pauses_ = 0;

  // Atomically counts operations in progress and operations that have been
  // completed. We need both to prevent mutations when paused.
  static scc_uint64_t ops_in_progress_ = 0;
  static scc_uint64_t ops_ = 0;
#endif

scc_bool_t scc_allocators_should_block(void) {
#if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
  return (scc_atomic_load_u64(&pauses_) > 0);
#else
  // No inspection functionality in release builds.
  return SCC_FALSE;
#endif
}

#if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
  static void *allocate_when_safe_(scc_allocator_t *allocator,
                                   scc_size_t size,
                                   scc_size_t alignment) {
    scc_atomic_increment_u64(&ops_in_progress_);
    while (scc_allocators_should_block());
    void *ptr = allocator->actual_allocate_(allocator, size, alignment);
    scc_atomic_increment_u64(&ops_);
    return ptr;
  }

  static void free_when_safe_(scc_allocator_t *allocator,
                              void *ptr) {
    scc_atomic_increment_u64(&ops_in_progress_);
    while (scc_allocators_should_block());
    allocator->actual_free_(allocator, ptr);
    scc_atomic_increment_u64(&ops_);
  }
#endif

void scc_allocator_register(scc_allocator_t *allocator) {
#if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
  allocator->id = scc_atomic_increment_u64(&registered_) + 1;

  // Wrap to hide blocking logic from allocators.
  allocator->actual_allocate_ = allocator->allocate;
  allocator->actual_free_ = allocator->free;
  allocator->allocate = &allocate_when_safe_;
  allocator->free = &free_when_safe_;

  scc_atomic_increment_u64(&ops_in_progress_);

  while (scc_allocators_should_block());

  scc_allocator_t **head = &allocators_;

  if (*head)
    (*head)->prev = allocator;
 
  allocator->prev = NULL;
  allocator->next = *head;

  *head = allocator;

  scc_atomic_increment_u64(&ops_);
#endif
}

void scc_allocator_deregister(scc_allocator_t *allocator) {
#if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
  scc_atomic_increment_u64(&ops_in_progress_);

  while (scc_allocators_should_block());

  if (allocator->prev)
    allocator->prev->next = allocator->next;
  if (allocator->next)
    allocator->next->prev = allocator->prev;

  scc_atomic_increment_u64(&ops_);
#endif
}

static void scc_allocators_pause(void) {
#if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
  scc_atomic_increment_u64(&pauses_);

  const scc_uint64_t outstanding = scc_atomic_load_u64(&ops_in_progress_);

  // Wait until any previous operations are complete, to guarantee immutability.
  while (scc_atomic_load_u64(&ops_) < outstanding);
#else
  SCC_TRAP();
#endif
}

static void scc_allocators_unpause(void) {
#if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
  scc_atomic_decrement_u64(&pauses_);
#else
  SCC_TRAP();
#endif
}

void scc_visit_each_allocator(scc_allocator_visitor_fn visitor,
                              void *context) {
#if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
  scc_allocators_pause();

  scc_allocator_t *allocator = allocators_;
  while (allocator) {
    visitor(allocator, context);
    allocator = allocator->next;
  }

  scc_allocators_unpause();
#endif
}

SCC_END_EXTERN_C

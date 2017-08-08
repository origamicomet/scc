//===-- scc/foundation/global_heap_allocator.cc ---------*- mode: C++11 -*-===//
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

#include "scc/foundation/global_heap_allocator.h"

#include "scc/foundation/atomics.h"
#include "scc/foundation/assert.h"

// REFACTOR(mtwilliams): Wrap `memset` et al.
#include <string.h>

#if SCC_COMPILER == SCC_COMPILER_MSVC
  #include <malloc.h>
#endif

SCC_BEGIN_EXTERN_C

static scc_allocator_t global_heap_allocator_;
static scc_uint32_t initialized_ = 0;


static void *allocate_from_global_heap_(scc_allocator_t *global_heap_allocator,
                                        scc_size_t size,
                                        scc_size_t alignment) {
#if SCC_COMPILER == SCC_COMPILER_MSVC
  void *ptr = _aligned_malloc(size, alignment);
#else
#endif

#if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
  global_heap_allocator->info.reserved += size;
  global_heap_allocator->info.committed += size;
  global_heap_allocator->info.allocated += size;
  global_heap_allocator->info.allocations += 1;
#endif

  // Will return `NULL` if out of memory.
  scc_assert_paranoid(ptr != NULL);

  // We always zero memory, as it prevents an entire class of errors.
  memset(ptr, 0, size);

  return ptr;
}

static void free_from_global_heap_(scc_allocator_t *global_heap_allocator,
                                   void *ptr) {
  scc_assert_paranoid(ptr != NULL);

#if SCC_COMPILER == SCC_COMPILER_MSVC
  _aligned_free(ptr);
#else
#endif

#if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
  // BUG(mtwilliams): No header to track size; breaks statistics.
#if 0
  global_heap_allocator->info.reserved -= size;
  global_heap_allocator->info.committed -= size;
  global_heap_allocator->info.freed += size;
#endif
  global_heap_allocator->info.frees += 1;
#endif
}

scc_allocator_t *scc_get_global_heap_allocator(void) {
  // REFACTOR(mtwilliams): Into initialization pattern into a `once` macro.
  const scc_uint32_t state = scc_atomic_cmp_and_xchg_u32(&initialized_, 0, 1);

  if (state == 0) {
  #if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
    strcpy(global_heap_allocator_.info.name, "global_heap_allocator");
    global_heap_allocator_.info.reserved = 0;
    global_heap_allocator_.info.committed = 0;
    global_heap_allocator_.info.allocated = 0;
    global_heap_allocator_.info.freed = 0;
    global_heap_allocator_.info.allocations = 0;
    global_heap_allocator_.info.frees = 0;
  #endif

    global_heap_allocator_.allocate = &allocate_from_global_heap_;
    global_heap_allocator_.free = &free_from_global_heap_;

    scc_allocator_register(&global_heap_allocator_);

    scc_atomic_store_u32(&initialized_, ~0);
  } else if (state == 1) {
    while (scc_atomic_load_u32(&initialized_) != ~0);
  }

  return &global_heap_allocator_;
}

SCC_END_EXTERN_C

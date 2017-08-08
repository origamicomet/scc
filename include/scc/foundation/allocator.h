//===-- scc/foundation/allocator.h ----------------------*- mode: C++11 -*-===//
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
///
/// \file
/// \brief
///
//===----------------------------------------------------------------------===//

#ifndef _SCC_FOUNDATION_ALLOCATOR_H_
#define _SCC_FOUNDATION_ALLOCATOR_H_

#include "scc/config.h"

#include "scc/foundation/types.h"

SCC_BEGIN_EXTERN_C

typedef struct scc_allocator_info {
  // Friendly name of the allocator.
  char name[256];

  // Number of bytes allocated from the backing allocator.
  scc_size_t reserved;

  // Number of bytes allocated from the allocator.
  scc_size_t committed;

  // Number of bytes allocated over the lifetime of the allocator.
  scc_size_t allocated;
  
  // Number of bytes freed over the  lifetime of the allocator.
  scc_size_t freed;

  // Allocations made over the lifetime of the allocator.
  scc_size_t allocations;

  // Frees made over the lifetime of the allocator.
  scc_size_t frees;
} scc_allocator_info_t;

typedef void *(*scc_allocator_allocate_fn)(struct scc_allocator *allocator,
                                           scc_size_t size,
                                           scc_size_t alignment);

typedef void (*scc_allocator_free_fn)(struct scc_allocator *allocator,
                                      void *ptr);

typedef struct scc_allocator {
#if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
  // Globally unique identifier.
  scc_uint64_t id;
  
  // Global linked-list of all registered allocators.
  struct scc_allocator *next, *prev;

  // Useful information and statistics.
  scc_allocator_info_t info;
#endif

  scc_allocator_allocate_fn allocate;
  scc_allocator_free_fn free;

#if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
  // Wrapped during registration to consolidate blocking logic.
  scc_allocator_allocate_fn actual_allocate_;
  scc_allocator_free_fn actual_free_;
#endif
} scc_allocator_t;

extern SCC_LOCAL
  void scc_allocator_register(scc_allocator_t *allocator);

extern SCC_LOCAL
  void scc_allocator_deregister(scc_allocator_t *allocator);

typedef void (*scc_allocator_visitor_fn)(scc_allocator_t *allocator,
                                         void *context);

extern SCC_LOCAL
  void scc_visit_each_allocator(scc_allocator_visitor_fn visitor,
                                void *context);

SCC_END_EXTERN_C

#endif // _SCC_FOUNDATION_ALLOCATOR_H_

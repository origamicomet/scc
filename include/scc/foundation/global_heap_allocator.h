//===-- scc/foundation/global_heap_allocator.h ----------*- mode: C++11 -*-===//
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

#ifndef _SCC_FOUNDATION_GLOBAL_HEAP_ALLOCATOR_H_
#define _SCC_FOUNDATION_GLOBAL_HEAP_ALLOCATOR_H_

#include "scc/foundation/allocator.h"

SCC_BEGIN_EXTERN_C

extern SCC_LOCAL
  scc_allocator_t *scc_get_global_heap_allocator(void);

SCC_END_EXTERN_C

#endif // _SCC_FOUNDATION_GLOBAL_HEAP_ALLOCATOR_H_

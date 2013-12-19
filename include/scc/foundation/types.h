//===-- scc/foundation/types.h --------------------------*- mode: C++11 -*-===//
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
//
// TODO(mtwilliams): Document the purpose of this file.
//
//===----------------------------------------------------------------------===//

#ifndef _SCC_FOUNDATION_TYPES_H_
#define _SCC_FOUNDATION_TYPES_H_

#include "scc/config.h"
#include "scc/linkage.h"

// TODO(mtwilliams): Don't rely on `stdint.h`.
// TODO(mtwilliams): Provide fallbacks for shitty compilers.
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <float.h>

SCC_BEGIN_EXTERN_C // {

#define SCC_TRUE (scc_bool_t)(true)
#define SCC_FALSE (scc_bool_t)(false)

typedef uintptr_t scc_uintptr_t;
typedef size_t scc_size_t;

typedef int8_t scc_int8_t;
typedef uint8_t scc_uint8_t;
typedef int16_t scc_int16_t;
typedef uint16_t scc_uint16_t;
typedef int32_t scc_int32_t;
typedef uint32_t scc_uint32_t;
typedef int64_t scc_int64_t;
typedef uint64_t scc_uint64_t;

typedef float scc_float32_t;
typedef double scc_float64_t;

typedef bool scc_bool_t;

SCC_END_EXTERN_C // }

SCC_BEGIN_NAMESPACE // {

typedef scc_int8_t i8;
typedef scc_uint8_t u8;
typedef scc_int16_t i16;
typedef scc_uint16_t u16;
typedef scc_int32_t i32;
typedef scc_uint32_t u32;
typedef scc_int64_t i64;
typedef scc_uint64_t u64;

typedef scc_float32_t f32;
typedef scc_float64_t f64;

SCC_END_NAMESPACE // }

#endif // _SCC_FOUNDATION_TYPES_H_

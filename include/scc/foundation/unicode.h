//===-- scc/foundation/unicode.h ------------------------*- mode: C++11 -*-===//
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

#ifndef _SCC_FOUNDATION_UNICODE_H_
#define _SCC_FOUNDATION_UNICODE_H_

#include "scc/config.h"

#if SCC_CHARACTER_SET == SCC_UNICODE

#include "scc/foundation/types.h"

typedef scc_uint32_t scc_character_t;

SCC_BEGIN_EXTERN_C

static SCC_INLINE scc_bool_t scc_is_eof(scc_character_t ch);
static SCC_INLINE scc_bool_t scc_is_whitespace(scc_character_t ch);
static SCC_INLINE scc_bool_t scc_is_lowercase(scc_character_t ch);
static SCC_INLINE scc_bool_t scc_is_uppercase(scc_character_t ch);
static SCC_INLINE scc_bool_t scc_is_alpha(scc_character_t ch);
static SCC_INLINE scc_bool_t scc_is_digit(scc_character_t ch);
static SCC_INLINE scc_bool_t scc_is_alphanumeric(scc_character_t ch);
static SCC_INLINE scc_bool_t scc_is_hex_digit(scc_character_t ch);

SCC_END_EXTERN_C

#endif // SCC_CHARACTER_SET == SCC_UNICODE

#endif // _SCC_FOUNDATION_UNICODE_H_

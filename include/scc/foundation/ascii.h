//===-- scc/foundation/ascii.h --------------------------*- mode: C++11 -*-===//
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

#ifndef _SCC_FOUNDATION_ASCII_H_
#define _SCC_FOUNDATION_ASCII_H_

#include "scc/config.h"

#if SCC_CHARACTER_SET == SCC_ASCII

#include "scc/foundation/types.h"

typedef scc_uint8_t scc_character_t;

SCC_BEGIN_EXTERN_C

static SCC_INLINE scc_bool_t scc_is_eof(scc_character_t ch) {
  return (ch == '\0');
}

static SCC_INLINE scc_bool_t scc_is_whitespace(scc_character_t ch) {
  return (ch ==  ' ')
      || (ch == '\t')
      || (ch == '\n')
      || (ch == '\r');
}

static SCC_INLINE scc_bool_t scc_is_lowercase(scc_character_t ch) {
  return ((ch >= 'a') && (ch <= 'z'));
}

static SCC_INLINE scc_bool_t scc_is_uppercase(scc_character_t ch) {
  return ((ch >= 'A') && (ch <= 'Z'));
}

static SCC_INLINE scc_bool_t scc_is_alpha(scc_character_t ch) {
  if ((ch >= 'a') && (ch <= 'z'))
    return SCC_TRUE;
  if ((ch >= 'A') && (ch <= 'Z'))
    return SCC_TRUE;
  return SCC_FALSE;
}


static SCC_INLINE scc_bool_t scc_is_digit(scc_character_t ch) {
  return ((ch >= '0') && (ch <= '9'));
}

static SCC_INLINE scc_bool_t scc_is_oct_digit(scc_character_t ch) {
  return ((ch >= '0') && (ch <= '7'));
}

static SCC_INLINE scc_bool_t scc_is_hex_digit(scc_character_t ch) {
  if ((ch >= '0') && (ch <= '9'))
    return SCC_TRUE;
  if (((ch ^ ' ') >= 'a') && ((ch ^ ' ') <= 'f'))
    return SCC_TRUE;
  return SCC_FALSE;
}

static SCC_INLINE scc_bool_t scc_is_alphanumeric(scc_character_t ch) {
  return scc_is_alpha(ch) || scc_is_digit(ch);
}

SCC_END_EXTERN_C

#endif // SCC_CHARACTER_SET == SCC_ASCII

#endif // _SCC_FOUNDATION_ASCII_H_

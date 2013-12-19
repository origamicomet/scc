//===-- scc/ir/parser.h ---------------------------------*- mode: C++11 -*-===//
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

#ifndef _SCC_IR_PARSER_H_
#define _SCC_IR_PARSER_H_

#include "scc/foundation.h"

#include "scc/feed.h"

SCC_BEGIN_EXTERN_C

typedef struct scc_ir_parse_options {
} scc_ir_parse_options_t;

extern SCC_PUBLIC
  void scc_ir_parse(scc_feed_t *feed,
                    const scc_ir_parse_options_t *options);

SCC_END_EXTERN_C

#endif // _SCC_IR_PARSER_H_

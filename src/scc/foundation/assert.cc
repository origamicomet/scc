//===-- scc/foundation/assert.cc ------------------------*- mode: C++11 -*-===//
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

#include "scc/foundation/assert.h"

SCC_BEGIN_EXTERN_C

static scc_assertion_handler_t assertion_handler_ = NULL;
static void *assertion_handler_ctx_ = NULL;

void scc_assertion_handler(
  scc_assertion_handler_t *assertion_handler,
  void **ctx)
{
  *assertion_handler = assertion_handler_;
  *ctx = assertion_handler_ctx_;
}

void scc_set_assertion_handler(
  scc_assertion_handler_t new_assertion_handler,
  void *ctx)
{
  assertion_handler_ = new_assertion_handler;
  assertion_handler_ctx_ = ctx;
}

SCC_END_EXTERN_C

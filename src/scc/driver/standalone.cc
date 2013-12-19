//===-- scc/driver/standalone.cc ------------------------*- mode: C++11 -*-===//
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

#if defined(__SCC_IS_STANDALONE__)

#include <stdlib.h>
#include <stdio.h>

#include "scc.h"

SCC_BEGIN_EXTERN_C

int main(unsigned argc, const char *argv[]) {
  // Parse arugments.
   // Handle response files.
  // Default to STDIN and STDOUT.
  // Setup driver.
  // Compile!

  const char *path = "tests/basic_vertex_shader.ir";
  scc_feed_t *feed = scc_feed_from_path(path);

  scc_ir_parse_options_t parse_options;
  scc_ir_parse(feed, &parse_options);

  return EXIT_SUCCESS;
}

SCC_END_EXTERN_C

#endif

//===-- scc/feed.h --------------------------------------*- mode: C++11 -*-===//
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

#ifndef _SCC_FEED_H_
#define _SCC_FEED_H_

#include "scc/foundation.h"

SCC_BEGIN_EXTERN_C

typedef scc_size_t (*scc_feed_fetch_fn)(struct scc_feed *feed,
                                        scc_character_t *buffer,
                                        scc_size_t count);

typedef struct scc_feed {
  scc_feed_fetch_fn fetch;
} scc_feed_t;

extern SCC_PUBLIC
  scc_feed_t *scc_feed_from_path(const char *path);

extern SCC_PUBLIC
  scc_feed_t *scc_feed_from_file(FILE *file);

SCC_END_EXTERN_C

#endif // _SCC_FEED_H_

//===-- scc/feed.cc -------------------------------------*- mode: C++11 -*-===//
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

#include "scc/feed.h"

SCC_BEGIN_EXTERN_C

typedef struct scc_feed_from_file {
  scc_feed_t feed;
  FILE *file;
} scc_feed_from_file_t;

static scc_size_t scc_read_from_file(scc_feed_from_file_t *feed,
                                     scc_character_t *buffer,
                                     scc_size_t count) {
  // TODO(mtwilliams): In place decode according to `SCC_CHARACTER_SET`.
  const scc_size_t read = fread((void *)buffer, 1, count, feed->file);

  if (read > 0) {
    return read;
  } else {
    // End of feed. 
    buffer[0] = '\0';
    fclose(feed->file);
    free((void *)feed);
    return 1;
  }
}

scc_feed_t *scc_feed_from_path(const char *path) {
  scc_assert_paranoid(path != NULL);

  // REFACTOR(mtwilliams): Explicit error handling.
   // * Check that file at `path` exists.
   // * Check that we have read permissions.
   // * Open exclusive?

  if (FILE *file = fopen(path, "rb"))
    return scc_feed_from_file(file);

  return NULL;
}

scc_feed_t *scc_feed_from_file(FILE *file) {
  scc_assert_paranoid(file != NULL);

  scc_feed_from_file_t *feed =
    (scc_feed_from_file_t *)calloc(sizeof(scc_feed_from_file_t), 1);

  feed->feed.fetch = (scc_feed_fetch_fn)&scc_read_from_file;
  feed->file = file;

  return &feed->feed;

  return NULL;
}

SCC_END_EXTERN_C

//===-- scc/lexer.h -------------------------------------*- mode: C++11 -*-===//
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

#ifndef _SCC_LEXER_H_
#define _SCC_LEXER_H_

#include "scc/foundation.h"

#include "scc/feed.h"

SCC_BEGIN_EXTERN_C

#ifndef SCC_COLUMNS_PER_TAB
  #define SCC_COLUMNS_PER_TAB 2
#endif

// TODO(mtwilliams): Rename `scc_lexer_position_t`?
typedef struct scc_position {
  scc_size_t absolute;
  scc_size_t line;
  scc_size_t character;
  scc_size_t column;
} scc_position_t;

typedef struct scc_span {
  scc_position_t start;
  scc_position_t end;
} scc_span_t;

typedef struct scc_lexer_error {
  struct scc_lexer_error *next;

  // TODO(mtwilliams): Rename something clearer.
  scc_span_t context;

  // TODO(mtwilliams): Dynamically allocate through a common buddy allocator.
  char message[4096];
} scc_lexer_error_t;

typedef struct scc_lexer_options {
  // Number of characters to buffer internally.
  scc_size_t buffer;
} scc_lexer_options_t;

typedef struct scc_lexer {
  scc_feed_t *feed;

  // Internal buffer.
  scc_character_t *buffer;

  // Size of internal buffer in characters.
  scc_size_t size_of_buffer;

  // Half the size of internal buffer.
  // See `scc_lexer_get_next_chunk`.
  scc_size_t half_of_buffer;

  scc_size_t position_in_buffer;
  scc_size_t end_of_buffer;

  // Position of current character.
  scc_position_t position;

  // Position of next character.
  scc_position_t next;

  // Current (potentially Unicode) character.
  scc_character_t character;

  // Rewinding trickery.
  scc_position_t bookmark;
  scc_size_t backtrack;

  // List of delimiter tokens, i.e. a valid character to end a token.
  const scc_character_t *delimiters;
  scc_size_t num_of_delimiters;

  // Indicates if the current character is whitespace.
  scc_bool_t whitespace;

  // Indicates if the current character is a delimiter.
  // See `delimiters`.
  scc_bool_t delimiter;

  // Indicates if lexer is at the end of the feed.
  scc_bool_t eof;

  // Linked-list of errors raised.
  scc_lexer_error_t *errors;

  // Indicates if `buffer` was allocated during initialization and should be
  // freed during finalization.
  scc_bool_t free_after_finalize;
} scc_lexer_t;

extern SCC_LOCAL
  scc_bool_t scc_lexer_is_delimiter(const scc_lexer_t *lexer,
                                    scc_character_t character);

extern SCC_LOCAL
  void scc_lexer_initialize(scc_lexer_t *lexer,
                            const scc_lexer_options_t *options);

extern SCC_LOCAL
  void scc_lexer_finalize(scc_lexer_t *lexer);

extern SCC_LOCAL
  scc_character_t scc_lexer_peek_next_character(scc_lexer_t *lexer);

extern SCC_LOCAL
  scc_character_t scc_lexer_get_next_character(scc_lexer_t *lexer);

extern SCC_LOCAL
  void scc_lexer_swallow_next_character(scc_lexer_t *lexer);

extern SCC_LOCAL
  void scc_lexer_bookmark(scc_lexer_t *lexer);

extern SCC_LOCAL
  void scc_lexer_rewind(scc_lexer_t *lexer);

extern SCC_LOCAL
  void scc_lexer_commit(scc_lexer_t *lexer);

extern SCC_LOCAL
  scc_character_t scc_lexer_skip_any_whitespace(scc_lexer_t *lexer);

extern SCC_LOCAL
  scc_character_t scc_lexer_skip_to_next_line(scc_lexer_t *lexer);

extern SCC_LOCAL
  void scc_lexer_get_up_to_delimiter(scc_lexer_t *lexer,
                                     scc_character_t *buffer,
                                     scc_size_t limit);

extern SCC_LOCAL
  void scc_lexer_error(scc_lexer_t *lexer, const char *format, ...);

SCC_END_EXTERN_C

#endif // _SCC_LEXER_H_

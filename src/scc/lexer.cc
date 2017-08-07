//===-- scc/lexer.cc ------------------------------------*- mode: C++11 -*-===//
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

#include "scc/lexer.h"

// REFACTOR(mtwilliams): Move into header.
#include <stdarg.h>

SCC_BEGIN_EXTERN_C

scc_bool_t scc_lexer_is_delimiter(const scc_lexer_t *lexer,
                                  scc_character_t character) {
  scc_assert_paranoid(lexer != NULL);

  if (lexer->num_of_delimiters > 0) {
    scc_assert_paranoid(lexer->delimiters != NULL);
  }

  for (scc_uint32_t delimiter = 0; delimiter < lexer->num_of_delimiters; ++delimiter)
    if (character == lexer->delimiters[delimiter])
      return SCC_TRUE;

  return SCC_FALSE;
}

void scc_lexer_initialize(scc_lexer_t *lexer,
                          const scc_lexer_options_t *options) {
  scc_assert_paranoid(lexer != NULL);
  scc_assert_paranoid(options != NULL);

  // We divide the internal buffer in two to implement backtracking, so we
  // check if the given size is divisible by two. This isn't strictly
  // necessary as we could round down, but eh.
  scc_assert_paranoid(SCC_IS_EVEN(options->buffer));
  
  lexer->size_of_buffer = options->buffer;
  lexer->half_of_buffer = lexer->size_of_buffer / 2;

  if (lexer->buffer) {
    lexer->free_after_finalize = SCC_FALSE;
  } else {
    lexer->buffer = (scc_character_t *)calloc(sizeof(scc_character_t),
                                              lexer->size_of_buffer);
    lexer->free_after_finalize = SCC_TRUE;
  }

  lexer->position_in_buffer = 0;
  lexer->end_of_buffer = 0;

  lexer->position.absolute = 0;
  lexer->position.line = 0;
  lexer->position.character = 0;
  lexer->position.column = 0;

  lexer->next.absolute = 0;
  lexer->next.line = 0;
  lexer->next.character = 0;
  lexer->next.column = 0;

  lexer->character = '\0';

  lexer->bookmark.absolute = 0;
  lexer->bookmark.line = 0;
  lexer->bookmark.character = 0;
  lexer->bookmark.column = 0;

  lexer->backtrack = 0;

  lexer->whitespace = SCC_FALSE;
  lexer->delimiter = SCC_FALSE;
  lexer->eof = SCC_FALSE;

  lexer->errors = NULL;
}

void scc_lexer_finalize(scc_lexer_t *lexer) {
  if (lexer->free_after_finalize)
    free((void *)lexer->buffer);

  scc_lexer_error_t *error = lexer->errors;
  while (error) {
    scc_lexer_error_t *next = error->next;
    free((void *)error);
    error = next;
  }
}

static scc_bool_t scc_lexer_get_next_chunk(scc_lexer_t *lexer) {
  scc_assert_paranoid(lexer->feed != NULL);

  // Make sure we've exhausted our internal buffer.
  scc_assert_paranoid(lexer->position_in_buffer == lexer->end_of_buffer);

  if (!lexer->eof) {
    lexer->end_of_buffer = (lexer->position.absolute + lexer->half_of_buffer) % lexer->size_of_buffer;
    lexer->position_in_buffer = lexer->end_of_buffer - lexer->half_of_buffer;

    const scc_size_t fetched =
      lexer->feed->fetch(lexer->feed,
                         &lexer->buffer[lexer->position_in_buffer],
                         lexer->half_of_buffer);

    lexer->end_of_buffer = lexer->position_in_buffer + fetched;

    if (fetched > 0) {
      lexer->eof = scc_is_eof(lexer->buffer[fetched - 1]);
    } else {
      lexer->eof = SCC_TRUE;
    }
  }

  return !(lexer->eof);
}

scc_character_t scc_lexer_peek_next_character(scc_lexer_t *lexer) {
  if (lexer->eof)
    return '\0';

  const scc_size_t position = lexer->position_in_buffer - lexer->backtrack;

  if (position == lexer->end_of_buffer) {
    scc_lexer_get_next_chunk(lexer);
    return scc_lexer_get_next_character(lexer);
  }

  return lexer->buffer[position % lexer->size_of_buffer];
}

scc_character_t scc_lexer_get_next_character(scc_lexer_t *lexer) {
  if (lexer->eof)
    return '\0';

  const scc_size_t position = lexer->position_in_buffer - lexer->backtrack;
  
  if (position == lexer->end_of_buffer) {
    scc_lexer_get_next_chunk(lexer);
    return scc_lexer_get_next_character(lexer);
  }
  
  lexer->character = lexer->buffer[position % lexer->size_of_buffer];

  if (scc_is_eof(lexer->character)) {
    lexer->whitespace = SCC_FALSE;
    lexer->delimiter  = SCC_TRUE;
    lexer->eof        = SCC_TRUE;
  } else {
    lexer->whitespace = scc_is_whitespace(lexer->character);
    lexer->delimiter  = scc_lexer_is_delimiter(lexer, lexer->character);
    lexer->eof        = SCC_FALSE;
  }

  if (lexer->backtrack) {
    lexer->backtrack -= 1;
  } else {
    lexer->position_in_buffer += 1;
  }

  lexer->position.absolute  = lexer->next.absolute;
  lexer->position.line      = lexer->next.line;
  lexer->position.character = lexer->next.character;
  lexer->position.column    = lexer->next.column;

  lexer->next.absolute += 1;

  if (lexer->character == '\n') {
    lexer->next.line      += 1;
    lexer->next.character = 0;
    lexer->next.column    = 0;
  } else if (lexer->character == '\r') {
    // TODO(mtwilliams): Don't count any non-visible characters.
    // We don't count carriage returns.
  } else if (lexer->character == '\t') {
    lexer->next.character += 1;
    lexer->next.column    += SCC_COLUMNS_PER_TAB;
  } else {
    lexer->next.character += 1;
    lexer->next.column    += 1;
  }

  return lexer->character;
}

// This only exists to convey semantic meaning.
void scc_lexer_swallow_next_character(scc_lexer_t *lexer) {
  scc_lexer_get_next_character(lexer);
}

void scc_lexer_bookmark(scc_lexer_t *lexer) {
  lexer->bookmark.absolute  = lexer->position.absolute;
  lexer->bookmark.line      = lexer->position.line;
  lexer->bookmark.character = lexer->position.character;
  lexer->bookmark.column    = lexer->position.column;
}

void scc_lexer_rewind(scc_lexer_t *lexer) {
  // Add, as we may already be rewinding.
  lexer->backtrack += lexer->position.absolute - lexer->bookmark.absolute;

  // Can't rewind more than halfway back, as previous characters will no longer
  // be in the buffer. See `scc_lexer_get_next_chunk`.
  scc_assert_paranoid(lexer->backtrack <= lexer->half_of_buffer);

  lexer->character = lexer->buffer[lexer->bookmark.absolute % lexer->size_of_buffer];

  // TODO(mtwilliams): Set `lexer->next`.
  lexer->position.absolute  = lexer->bookmark.absolute;
  lexer->position.line      = lexer->bookmark.line;
  lexer->position.character = lexer->bookmark.character;
  lexer->position.column    = lexer->bookmark.column;

  const scc_bool_t newline = lexer->character == '\n';
  const scc_bool_t visible = lexer->character != '\r';
  const scc_bool_t tab     = lexer->character == '\t';

  lexer->next.absolute  = lexer->bookmark.absolute + 1;
  lexer->next.line      = lexer->bookmark.line + (newline ? 1 : 0);
  lexer->next.character = newline ? 0 : lexer->bookmark.character + (visible ? 1 : 0);
  lexer->next.column    = lexer->bookmark.column + (visible ? (tab ? SCC_COLUMNS_PER_TAB : 1) : 0);

  lexer->whitespace = scc_is_whitespace(lexer->character);
  lexer->delimiter  = scc_lexer_is_delimiter(lexer, lexer->character);
  lexer->eof        = scc_is_eof(lexer->character);
}

void scc_lexer_commit(scc_lexer_t *lexer) {
  scc_assert_paranoid(lexer->backtrack == 0);

  lexer->bookmark.absolute  = lexer->position.absolute;
  lexer->bookmark.line      = lexer->position.line;
  lexer->bookmark.character = lexer->position.character;
  lexer->bookmark.column    = lexer->position.column;
}

scc_character_t scc_lexer_skip_any_whitespace(scc_lexer_t *lexer) {
  while (lexer->whitespace)
    scc_lexer_get_next_character(lexer);

  return lexer->character;
}

scc_character_t scc_lexer_skip_to_next_line(scc_lexer_t *lexer) {
  do {
    scc_lexer_get_next_character(lexer);
  } while (lexer->character != '\n');

  return lexer->character;
}

void scc_lexer_get_up_to_delimiter(scc_lexer_t *lexer,
                                   scc_character_t *buffer,
                                   scc_size_t limit) {
  scc_assert_paranoid(buffer != NULL);

  scc_size_t offset = 0;

  buffer[offset++] = lexer->character;

  while (const scc_character_t character = scc_lexer_peek_next_character(lexer)) {
    if (scc_is_whitespace(character))
      break;

    if (scc_lexer_is_delimiter(lexer, character))
      break;

    // TODO(mtwilliams): Ensure we never exceed |limit|.
    buffer[offset++] = character;

    scc_lexer_swallow_next_character(lexer);
  }

  // Null terminate.
  buffer[offset] = '\0';
}

void scc_lexer_error(scc_lexer_t *lexer,
                     const char *format,
                     ...) {
  scc_lexer_error_t *error =
    (scc_lexer_error_t *)calloc(1, sizeof(scc_lexer_error_t));

  // TODO(mtwilliams): Improve contextualization.
  error->context.start = lexer->position;
  error->context.end   = lexer->next;

  static const scc_size_t limit = sizeof(error->message) - 1;

  va_list va;
  va_start(va, format);
  vsnprintf(&error->message[0], limit, format, va);
  va_end(va);

  // Ensure null terminated.
  error->message[limit] = '\0';

  // PERF(mtwilliams): Appends in `O(n)` time.
  scc_lexer_error_t **prev = &lexer->errors;
  while (*prev)
    prev = &(*prev)->next;
  *prev = error;
}

SCC_END_EXTERN_C

//===-- scc/ir/parser.cc --------------------------------*- mode: C++11 -*-===//
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

#include "scc/ir/parser.h"

// Common scanning logic is factored out.
#include "scc/lexer.h"

SCC_BEGIN_EXTERN_C

// REFACTOR(mtwilliams): Move to appropriate header.
typedef enum scc_ir_scope {
  SCC_IR_SCOPE_NONE   = 0,
  SCC_IR_SCOPE_LOCAL  = 1,
  SCC_IR_SCOPE_GLOBAL = 2,
} scc_ir_scope_t;

typedef enum scc_ir_token_type {
  // Unknown. Should never encounter.
  SCC_IR_TOKEN_UNKNOWN = 255,

  // End of feed.
  SCC_IR_TOKEN_EOF = 0,

  // A line comment.
  SCC_IR_TOKEN_COMMENT,

  // Keywords
  SCC_IR_TOKEN_PROGRAM,
  SCC_IR_TOKEN_TYPEDEF,
  SCC_IR_TOKEN_INPUTS,
  SCC_IR_TOKEN_OUTPUTS,
  SCC_IR_TOKEN_CONSTANTS,
  SCC_IR_TOKEN_DEFINE,
  SCC_IR_TOKEN_CALL,
  SCC_IR_TOKEN_RETURN,

  // First class types.
  SCC_IR_TOKEN_TYPE,

  // Operations.
  SCC_IR_TOKEN_OPERATION,

  // Global, local, unnamed, or undecorated identifier.
  SCC_IR_TOKEN_IDENTIFIER,

  // Label.
  SCC_IR_TOKEN_LABEL,
  
  // Integer or floating-point constant.
  SCC_IR_TOKEN_NUMBER,

  // Punctuation.
  SCC_IR_TOKEN_L_PARENTHESIS,
  SCC_IR_TOKEN_R_PARENTHESIS,
  SCC_IR_TOKEN_L_BRACE,
  SCC_IR_TOKEN_R_BRACE,
  SCC_IR_TOKEN_L_BRACKET,
  SCC_IR_TOKEN_R_BRACKET,
  SCC_IR_TOKEN_COMMA,
  SCC_IR_TOKEN_EQUALS
} scc_ir_token_type_t;

typedef struct scc_ir_keyword_def {
  const char *keyword;
  scc_ir_token_type_t token;
} scc_ir_keyword_def_t;

// REFACTOR(mtwilliams): Use a macros.
// PERF(mtwilliams): Build a radix tree to improve match performance.
static const scc_ir_keyword_def_t KEYWORDS[] = {
  { "program",   SCC_IR_TOKEN_PROGRAM   },
  { "type",      SCC_IR_TOKEN_TYPEDEF   },
  { "inputs",    SCC_IR_TOKEN_INPUTS    },
  { "ouputs",    SCC_IR_TOKEN_OUTPUTS   },
  { "constants", SCC_IR_TOKEN_CONSTANTS },
  { "def",       SCC_IR_TOKEN_DEFINE    },
  { "call",      SCC_IR_TOKEN_CALL      },
  { "ret",       SCC_IR_TOKEN_RETURN    }
};

static const scc_uint32_t NUM_OF_KEYWORDS =
  sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

typedef struct scc_ir_type_def {
  const char *name;
#if 0
  const scc_ir_type_t *type;
#else
  const void *type;
#endif

  scc_bool_t internal;
} scc_ir_type_def_t;

// TODO(mtwilliams): Booleans.
// TODO(mtwilliams): Pointers to fulfill memory model.
// REFACTOR(mtwilliams): Use a macros.
// PERF(mtwilliams): Build a radix tree to improve match performance.
static const scc_ir_type_def_t TYPES[] = {
  //
  // Special
  //
  { "void",     /*&scc_ir_void*/ NULL,    SCC_TRUE },

  //
  // Signed
  //
  { "i8",       /*&scc_ir_i8*/ NULL,      SCC_TRUE },
  { "i8<2x1>",  /*&scc_ir_i8_2x1*/ NULL,  SCC_TRUE },
  { "i8<3x1>",  /*&scc_ir_i8_3x1*/ NULL,  SCC_TRUE },
  { "i8<4x1>",  /*&scc_ir_i8_4x1*/ NULL,  SCC_TRUE },
  { "i16",      /*&scc_ir_i16*/ NULL,     SCC_TRUE },
  { "i16<2x1>", /*&scc_ir_i16_2x1*/ NULL, SCC_TRUE },
  { "i16<3x1>", /*&scc_ir_i16_3x1*/ NULL, SCC_TRUE },
  { "i16<4x1>", /*&scc_ir_i16_4x1*/ NULL, SCC_TRUE },
  { "i16",      /*&scc_ir_i16*/ NULL,     SCC_TRUE },
  { "i32<2x1>", /*&scc_ir_i32_2x1*/ NULL, SCC_TRUE },
  { "i32<3x1>", /*&scc_ir_i32_3x1*/ NULL, SCC_TRUE },
  { "i32<4x1>", /*&scc_ir_i32_4x1*/ NULL, SCC_TRUE },
  { "i64",      /*&scc_ir_i64*/ NULL,     SCC_TRUE },
  { "i64<2x1>", /*&scc_ir_i64_2x1*/ NULL, SCC_TRUE },
  { "i64<3x1>", /*&scc_ir_i64_3x1*/ NULL, SCC_TRUE },
  { "i64<4x1>", /*&scc_ir_i64_4x1*/ NULL, SCC_TRUE },

  //
  // Unsigned
  //
  { "u8",       /*&scc_ir_u8*/ NULL,      SCC_TRUE },
  { "u8<2x1>",  /*&scc_ir_u8_2x1*/ NULL,  SCC_TRUE },
  { "u8<3x1>",  /*&scc_ir_u8_3x1*/ NULL,  SCC_TRUE },
  { "u8<4x1>",  /*&scc_ir_u8_4x1*/ NULL,  SCC_TRUE },
  { "u16",      /*&scc_ir_u16*/ NULL,     SCC_TRUE },
  { "u16<2x1>", /*&scc_ir_u16_2x1*/ NULL, SCC_TRUE },
  { "u16<3x1>", /*&scc_ir_u16_3x1*/ NULL, SCC_TRUE },
  { "u16<4x1>", /*&scc_ir_u16_4x1*/ NULL, SCC_TRUE },
  { "u16",      /*&scc_ir_u16*/ NULL,     SCC_TRUE },
  { "u32<2x1>", /*&scc_ir_u32_2x1*/ NULL, SCC_TRUE },
  { "u32<3x1>", /*&scc_ir_u32_3x1*/ NULL, SCC_TRUE },
  { "u32<4x1>", /*&scc_ir_u32_4x1*/ NULL, SCC_TRUE },
  { "u64",      /*&scc_ir_u64*/ NULL,     SCC_TRUE },
  { "u64<2x1>", /*&scc_ir_u64_2x1*/ NULL, SCC_TRUE },
  { "u64<3x1>", /*&scc_ir_u64_3x1*/ NULL, SCC_TRUE },
  { "u64<4x1>", /*&scc_ir_u64_4x1*/ NULL, SCC_TRUE },

  //
  // Floating-point
  //
  { "f32",      /*&scc_ir_f32*/ NULL,     SCC_TRUE },
  { "f32<2x1>", /*&scc_ir_f32_2x1*/ NULL, SCC_TRUE },
  { "f32<3x1>", /*&scc_ir_f32_3x1*/ NULL, SCC_TRUE },
  { "f32<4x1>", /*&scc_ir_f32_4x1*/ NULL, SCC_TRUE },

  { "f64",      /*&scc_ir_f64*/ NULL,     SCC_TRUE },
  { "f64<2x1>", /*&scc_ir_f64_2x1*/ NULL, SCC_TRUE },
  { "f64<3x1>", /*&scc_ir_f64_3x1*/ NULL, SCC_TRUE },
  { "f64<4x1>", /*&scc_ir_f64_4x1*/ NULL, SCC_TRUE },

  //
  // Matrices
  //
  { "f32<3x3>", /*&scc_ir_f32_3x3*/ NULL, SCC_TRUE },
  { "f32<4x4>", /*&scc_ir_f32_4x4*/ NULL, SCC_TRUE }
};

static const scc_uint32_t NUM_OF_TYPES =
  sizeof(TYPES) / sizeof(TYPES[0]);

// REFACTOR(mtwilliams): Move into header.
typedef enum scc_ir_operation {
  #define OP(Mnemonic, Code, Inputs, Returns, Description) \
    SCC_IR_OPERATION_##Code,

    #include "scc/ir/operations.inl"

  #undef OP
} scc_ir_operation_t;

typedef struct scc_ir_operation_def {
  const char *mnemonic;
  scc_ir_operation_t op;
} scc_ir_operation_def_t;

static const scc_ir_operation_def_t OPERATIONS[] = {
  #define OP(Mnemonic, Code, Inputs, Returns, Description) \
    { #Mnemonic, SCC_IR_OPERATION_##Code },

    #include "scc/ir/operations.inl"

  #undef OP
};

static const scc_uint32_t NUM_OF_OPERATIONS =
  sizeof(OPERATIONS) / sizeof(OPERATIONS[0]);

typedef struct scc_ir_token {
  scc_ir_token_type_t type;

  scc_position_t position;
  scc_size_t length;

  union {
    // First class type.
    const scc_ir_type_def_t *type_def;

    // (Most) operations.
    scc_ir_operation_t op;

    struct {
      scc_ir_scope_t scope;
      char identifier[256];
    };

    char label[256];

    struct {
      scc_bool_t is_integer        : 1;
      scc_bool_t is_floating_point : 1;
      scc_int64_t integer;
      scc_float64_t floating_point;
    } constant;
  };
} scc_ir_token_t;

typedef struct scc_ir_lexer_options {
  // Size, in characters, of internal buffer.
  scc_size_t buffer;
} scc_ir_lexer_options_t;

typedef struct scc_ir_lexer {
  // REFACTOR(mtwilliams): Rename `scc_lexer_t` to `scc_scanner_t`?
  scc_lexer_t scanner;

  // ...
  scc_ir_token_t token;

  // Indicates if this was allocated during initialization and should be freed
  // during finalization.
  scc_bool_t free_after_finalize;
} scc_ir_lexer_t;

// Characters (outside of whitespace) that end a token.
static const scc_character_t DELIMITERS[] = {
  '(', ')', '{', '}', '[', ']', ';', ',', '=' 
};

static const scc_size_t NUM_OF_DELIMITERS =
  sizeof(DELIMITERS) / sizeof(DELIMITERS[0]);

static scc_ir_lexer_t *scc_ir_lexer_initialize(scc_ir_lexer_t *lexer,
                                               const scc_ir_lexer_options *options,
                                               scc_feed_t *feed) {
  scc_assert_paranoid(options != NULL);
  scc_assert_paranoid(feed != NULL);
  
  if (lexer) {
    lexer->free_after_finalize = SCC_FALSE;
  } else {
    lexer = (scc_ir_lexer_t *)calloc(1, sizeof(scc_ir_lexer_t));
    lexer->free_after_finalize = SCC_TRUE;
  }

  scc_lexer_options_t scanner_options;
  scanner_options.buffer = options->buffer;

  scc_lexer_initialize(&lexer->scanner, &scanner_options);

  lexer->scanner.feed = feed;

  lexer->scanner.delimiters = DELIMITERS;
  lexer->scanner.num_of_delimiters = NUM_OF_DELIMITERS;

  lexer->token.type = SCC_IR_TOKEN_UNKNOWN;

  return lexer;
}

static void scc_ir_lexer_finalize(scc_ir_lexer_t *lexer) {
  scc_lexer_finalize(&lexer->scanner);

  if (lexer->free_after_finalize)
    free((void *)lexer);
}

static const scc_ir_token_t *scc_ir_lexer_handle_comment(scc_ir_lexer_t *lexer) {
  lexer->token.type = SCC_IR_TOKEN_COMMENT;
  lexer->token.position = lexer->scanner.position;

  scc_lexer_skip_to_next_line(&lexer->scanner);

  lexer->token.length = lexer->scanner.position.absolute - lexer->token.position.absolute - 1;

  return &lexer->token;
}

static const scc_ir_token_t *scc_ir_lexer_handle_bracket(scc_ir_lexer_t *lexer) {
  switch (lexer->scanner.character) {
    case '(': lexer->token.type = SCC_IR_TOKEN_L_PARENTHESIS; break;
    case ')': lexer->token.type = SCC_IR_TOKEN_R_PARENTHESIS; break;
    case '{': lexer->token.type = SCC_IR_TOKEN_L_BRACE; break;
    case '}': lexer->token.type = SCC_IR_TOKEN_R_BRACE; break;
    case '[': lexer->token.type = SCC_IR_TOKEN_L_BRACKET; break;
    case ']': lexer->token.type = SCC_IR_TOKEN_R_BRACKET; break;
  }
  
  lexer->token.position = lexer->scanner.position;
  lexer->token.length = 1;

  return &lexer->token;
}

static const scc_ir_token_t *scc_ir_lexer_try_match_keyword(scc_ir_lexer_t *lexer,
                                                            const scc_character_t *lexeme) {
  for (scc_size_t candidate = 0; candidate < NUM_OF_KEYWORDS; ++candidate) {
    const scc_ir_keyword_def_t *def = &KEYWORDS[candidate];

    if (strcmp(def->keyword, (const char *)lexeme) != 0)
      continue;

    lexer->token.type = def->token;

    return &lexer->token;
  }

  return NULL;
}

static const scc_ir_token_t *scc_ir_lexer_try_match_operation(scc_ir_lexer_t *lexer,
                                                              const scc_character_t *lexeme) {
  for (scc_size_t candidate = 0; candidate < NUM_OF_OPERATIONS; ++candidate) {
    const scc_ir_operation_def_t *def = &OPERATIONS[candidate];

    if (strcmp(def->mnemonic, (const char *)lexeme) != 0)
      continue;

    lexer->token.type = SCC_IR_TOKEN_OPERATION;
    lexer->token.op = def->op;

    return &lexer->token;
  }

  return NULL;
}

static const scc_ir_token_t *scc_ir_lexer_try_match_type(scc_ir_lexer_t *lexer,
                                                         const scc_character_t *lexeme) {
  for (scc_size_t candidate = 0; candidate < NUM_OF_TYPES; ++candidate) {
    const scc_ir_type_def_t *def = &TYPES[candidate];

    if (strcmp(def->name, (const char *)lexeme) != 0)
      continue;

    lexer->token.type = SCC_IR_TOKEN_TYPE;
    lexer->token.type_def = def;

    return &lexer->token;
  }

  return NULL;
}

// TODO(mtwilliams): Check if a label (by presence of colon) prior to sanity checks.
static const scc_ir_token_t *scc_ir_lexer_try_match_identifier(scc_ir_lexer_t *lexer,
                                                               const scc_character_t *lexeme) {
  scc_ir_scope_t scope = SCC_IR_SCOPE_NONE;

  switch (lexeme[0]) {
    case '@': scope = SCC_IR_SCOPE_GLOBAL; break;
    case '%': scope = SCC_IR_SCOPE_LOCAL; break;
  }

  const scc_bool_t scoped = (scope != SCC_IR_SCOPE_NONE);

  if (!scoped) {
    if (!scc_is_alpha(lexeme[0]) && (lexeme[0] != '_')) {
      switch (lexeme[0]) {
        case '+':
        case '-':
        case '.':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          // Possibly a number; fall through.
          return NULL;

        default:
          scc_lexer_error(&lexer->scanner,
                          "Undecorated identifiers must start with a letter or underscore.");
          return &lexer->token;
      }
    }
  } else {
    if (lexeme[1] == '\0') {
      scc_lexer_error(&lexer->scanner,
                      "Identifiers must contain one or more characters.");
      return &lexer->token;
    }
  }

  for (const scc_character_t *ch = &lexeme[scoped ? 1 : 0]; *ch; ++ch) {
    if (!scc_is_alphanumeric(*ch) && (*ch != '_')) {
      if (*ch == ':') {
        if (!scoped) {
          // A label.
          return NULL;
        } else {
          scc_lexer_error(&lexer->scanner,
                          "Only identifiers can be scoped.");
          return &lexer->token; 
        }
      } else {
        scc_lexer_error(&lexer->scanner,
                        "Identifiers can only be composed of alphanumerics and underscores.");
        return &lexer->token;
      }
    }
  }

  lexer->token.type = SCC_IR_TOKEN_IDENTIFIER;

  lexer->token.scope = scope;
  
  // BUG(mtwilliams): Does not work when `SCC_CHARACTER_SET` isn't ASCII.
  strcpy(&lexer->token.identifier[0], (const char *)&lexeme[scoped ? 1 : 0]);
  
  return &lexer->token;
}

static const scc_ir_token_t *scc_ir_lexer_try_match_label(scc_ir_lexer_t *lexer,
                                                          const scc_character_t *lexeme) {
  if (!scc_is_alpha(lexeme[0]) && (lexeme[0] != '_')) {
    switch (lexeme[0]) {
      case '+':
      case '-':
      case '.':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        // Possibly a number; fall through.
        return NULL;

      default:
        if (lexeme[lexer->token.length - 1] == ':') {
          scc_lexer_error(&lexer->scanner,
                          "Labels must start with a letter or an underscore.");
          return &lexer->token;
        }
    }
  }

  // REFACTOR(mtwilliams): Treat colon as delimiter?
  for (const scc_character_t *ch = &lexeme[0]; *ch; ++ch) {
    if (!scc_is_alphanumeric(*ch) && (*ch != '_')) {
      if (*ch == ':') {
        if (*(ch+1) != '\0')
          // Really messed up.
          return NULL;
      } else {
        scc_lexer_error(&lexer->scanner,
                        "Labels can only be composed of alphanumerics and underscores.");
        return &lexer->token;
      }
    }
  }

  lexer->token.type = SCC_IR_TOKEN_LABEL;

  // BUG(mtwilliams): Does not work when `SCC_CHARACTER_SET` isn't ASCII.
  strcpy(&lexer->token.label[0], (const char *)&lexeme[0]);
  lexer->token.label[lexer->token.length] = '\0';
  
  return &lexer->token;
}

// HACK(mtwilliams): This is nasty. We can do better.
static const scc_ir_token_t *scc_ir_lexer_try_match_number(scc_ir_lexer_t *lexer,
                                                           const scc_character_t *lexeme) {
  
  signed   sign = +1;
  unsigned base = 10;
  
  unsigned offset = 0;
  
  scc_bool_t fp = SCC_FALSE;

  if (lexeme[0] == '+') {
    sign = +1;
    offset += 1;
  } else if (lexeme[0] == '-') {
    sign = -1;
    offset += 1;
  } else if (lexeme[0] == '0') {
    if (lexeme[1] == '\0') {
      lexer->token.type = SCC_IR_TOKEN_NUMBER;
      
      lexer->token.constant.is_integer = SCC_TRUE;
      lexer->token.constant.is_floating_point = SCC_TRUE;
      lexer->token.constant.integer = 0;
      
      return &lexer->token;
    } else if (lexeme[1] == '.') {
      // Floating-point; fall through.
    } else if (lexeme[1] == 'b') {
      base = 2;
      offset += 2;
    } else if (lexeme[1] == 'x') {
      base = 16;
      offset += 2;
    } else if (scc_is_digit(lexeme[1])) {
      base = 8;
      offset += 1;
    } else {
      scc_lexer_error(&lexer->scanner,
                      "Do not understand `0%c` prefix.",
                      lexeme[1]);

      return &lexer->token;
    }
  }

  char buffer[256];
  scc_size_t length = 0;

  for (const scc_character_t *ch = &lexeme[offset]; *ch; ++ch) {
    if (*ch == '\'')
      // Delimiter to improve readability.
      continue;

    switch (base) {
      case 2: {
        if ((*ch != '0') && (*ch != '1')) {
          scc_lexer_error(&lexer->scanner,
                          "Unexpected character `%c` in binary constant.",
                          *ch);

          return &lexer->token;
        }
      } break;

      case 8: {
        if ((*ch < '0') || (*ch > '7')) {
          scc_lexer_error(&lexer->scanner,
                          "Unexpected character `%c` in octal constant.",
                          *ch);

          return &lexer->token;
        }
      } break;

      case 10: {
        if ((*ch < '0') || (*ch > '9')) {
          if (fp) {
            if (*ch == '.') {
              scc_lexer_error(&lexer->scanner,
                              "Duplicate `.` in floating-point constant.",
                              *ch);

              return &lexer->token;
            }

            scc_lexer_error(&lexer->scanner,
                            "Unexpected character `%c` in floating-point constant.",
                            *ch);
            
            return &lexer->token;
          } else {
            if (*ch == '.') {
              fp = SCC_TRUE;
              break;
            }

            scc_lexer_error(&lexer->scanner,
                            "Unexpected character `%c` in decimal constant.",
                            *ch);
            
            return &lexer->token;
          }
        }
      } break;

      case 16: {
        if (((*ch < '0') || (*ch > '9')) &&
            ((*ch < 'a') || (*ch > 'f')) &&
            ((*ch < 'A') || (*ch > 'F'))) {
          scc_lexer_error(&lexer->scanner,
                          "Unexpected character `%c` in hexadecimal constant.",
                          *ch);
          return &lexer->token;
        }
      } break;
    }

    buffer[length++] = *ch;
  }

  // Null terminate.
  buffer[length] = '\0';

  switch (base) {
    case 2: {
      if ((length != 8) && (length != 16) && (length != 32) && (length != 64)) {
        scc_lexer_error(&lexer->scanner,
                        "Binary constants must specify 8, 16, 32, or 64 bits.");
        return &lexer->token;
      }

      lexer->token.type = SCC_IR_TOKEN_NUMBER;
      
      lexer->token.constant.is_integer = SCC_TRUE;
      lexer->token.constant.is_floating_point = SCC_FALSE;
      lexer->token.constant.integer = 0;

      for (scc_size_t bit = 0; bit < length; ++bit) {
        lexer->token.constant.integer |=
          ((buffer[bit] - '0') & 0x1) << (length - bit - 1);
      }

      return &lexer->token;
    }

    case 8: {
      lexer->token.type = SCC_IR_TOKEN_NUMBER;
      
      lexer->token.constant.is_integer = SCC_TRUE;
      lexer->token.constant.is_floating_point = SCC_FALSE;
      lexer->token.constant.integer = strtoll(&buffer[0], NULL, 8);

      return &lexer->token;
    }
    
    case 10: {
      lexer->token.type = SCC_IR_TOKEN_NUMBER;
      
      lexer->token.constant.is_integer = !fp;
      lexer->token.constant.is_floating_point = fp;

      if (fp)
        lexer->token.constant.floating_point = sign * strtod(&buffer[0], NULL);
      else
        lexer->token.constant.integer = sign * strtoll(&buffer[0], NULL, 16);

      return &lexer->token;
    }

    case 16: {
      lexer->token.type = SCC_IR_TOKEN_NUMBER;
      
      lexer->token.constant.is_integer = SCC_TRUE;
      lexer->token.constant.is_floating_point = SCC_FALSE;
      lexer->token.constant.integer = strtoll(&buffer[0], NULL, 16);

      return &lexer->token;
    }
  }
 
  return &lexer->token;
}

static const scc_ir_token_t *scc_ir_lexer_handle_other(scc_ir_lexer_t *lexer) {
  lexer->token.type = SCC_IR_TOKEN_UNKNOWN;
  lexer->token.position = lexer->scanner.position;

  scc_character_t buffer[256];
  scc_lexer_get_up_to_delimiter(&lexer->scanner, &buffer[0], 256);

  lexer->token.length = lexer->scanner.position.absolute - lexer->token.position.absolute - 1;

  // A keyword?
  if (const scc_ir_token_t *token = scc_ir_lexer_try_match_keyword(lexer, &buffer[0]))
    return token;

  // A type?
  if (const scc_ir_token_t *token = scc_ir_lexer_try_match_type(lexer, &buffer[0]))
    return token;

  // An operation?
  if (const scc_ir_token_t *token = scc_ir_lexer_try_match_operation(lexer, &buffer[0]))
    return token;

  // A global, local, unnamed, or undecorated identifier?
  if (const scc_ir_token_t *token = scc_ir_lexer_try_match_identifier(lexer, &buffer[0]))
    return token;

  // A label?
  if (const scc_ir_token_t *token = scc_ir_lexer_try_match_label(lexer, &buffer[0]))
    return token;

  // An integer or floating-pointer number?
  if (const scc_ir_token_t *token = scc_ir_lexer_try_match_number(lexer, &buffer[0]))
    return token;

  // BUG(mtwilliams): Does not work when `SCC_CHARACTER_SET` isn't ASCII. 
  scc_lexer_error(&lexer->scanner, "Unknown token `%s'!", buffer);

  return &lexer->token;
}

static const scc_ir_token_t *scc_ir_lexer_get_next_token(scc_ir_lexer_t *lexer) {
  scc_character_t character;

  character = scc_lexer_get_next_character(&lexer->scanner);
  character = scc_lexer_skip_any_whitespace(&lexer->scanner);

  if (lexer->scanner.eof) {
    lexer->token.type = SCC_IR_TOKEN_EOF;
    lexer->token.position = lexer->scanner.position;
    return &lexer->token;
  }

  // TODO(mtwilliams): Evaluator.
  switch (character) {
    // A comment.
    case ';':
      return scc_ir_lexer_handle_comment(lexer);

    // Parentheses, braces, and brackets.
    case '(': case ')':
    case '{': case '}':
    case '[': case ']':
      return scc_ir_lexer_handle_bracket(lexer);

    // Punctuation.
    case ',':
      lexer->token.type = SCC_IR_TOKEN_COMMA;
      lexer->token.position = lexer->scanner.position;
      lexer->token.length = 1;
      return &lexer->token;
    case '=':
      lexer->token.type = SCC_IR_TOKEN_EQUALS;
      lexer->token.position = lexer->scanner.position;
      lexer->token.length = 1;
      return &lexer->token;

    default:
      return scc_ir_lexer_handle_other(lexer);
  }
}

typedef struct scc_ir_parser {
  scc_ir_lexer_t lexer;

  // TODO(mtwilliams): Buffer tokens?
  const scc_ir_token_t *token;

  scc_bool_t failed;
} scc_ir_parser_t;

static scc_ir_parser_t *scc_ir_parser_create(scc_feed_t *feed,
                                             const scc_ir_parse_options_t *options) {
  scc_assert_paranoid(feed != NULL);

  scc_ir_parser_t *parser =
    (scc_ir_parser_t *)calloc(1, sizeof(scc_ir_parser_t));

  scc_ir_lexer_options_t lexer_options;
  lexer_options.buffer = 8192;

  scc_ir_lexer_initialize(&parser->lexer,
                          &lexer_options,
                          feed);

  return parser;
}

static void scc_ir_parser_destroy(scc_ir_parser_t *parser) {
  scc_assert_paranoid(parser != NULL);

  scc_ir_lexer_finalize(&parser->lexer);

  free((void *)parser);
}

static const scc_ir_token_t *scc_ir_parser_get_next_token(scc_ir_parser *parser) {
  if (parser->lexer.scanner.eof)
    return NULL;

  parser->token = scc_ir_lexer_get_next_token(&parser->lexer);

  return parser->token;
}

scc_bool_t scc_ir_parser_parse(scc_ir_parser_t *parser) {
  // HACK(mtwilliams): Temporary inspection. Remove later.
  while (const scc_ir_token_t *token = scc_ir_parser_get_next_token(parser)) {
    switch (token->type) {
      case SCC_IR_TOKEN_COMMENT:
        printf("COMMENT @ %u:%u (%u)\n",
               token->position.line,
               token->position.character,
               token->length);
        break;

      case SCC_IR_TOKEN_PROGRAM:
      case SCC_IR_TOKEN_TYPEDEF:
      case SCC_IR_TOKEN_INPUTS:
      case SCC_IR_TOKEN_OUTPUTS:
      case SCC_IR_TOKEN_CONSTANTS:
      case SCC_IR_TOKEN_DEFINE:
      case SCC_IR_TOKEN_CALL:
      case SCC_IR_TOKEN_RETURN:
        printf("KEYWORD @ %u:%u\n",
               token->position.line,
               token->position.character);
        break;

      case SCC_IR_TOKEN_TYPE:
        printf("TYPE @ %u:%u (%s)\n",
               token->position.line,
               token->position.character,
               token->type_def->name);
        break;

      case SCC_IR_TOKEN_OPERATION:
        printf("OPERATION @ %u:%u %s (%u)\n",
               token->position.line,
               token->position.character,
               OPERATIONS[token->op].mnemonic,
               token->op);
        break;

      case SCC_IR_TOKEN_IDENTIFIER:
        printf("IDENTIFIER @ %u:%u (%s)\n",
               token->position.line,
               token->position.character,
               &token->identifier[0]);
        break;

      case SCC_IR_TOKEN_LABEL:
        printf("LABEL @ %u:%u (%s)\n",
               token->position.line,
               token->position.character,
               &token->label[0]);
        break;

      case SCC_IR_TOKEN_NUMBER:
        if (token->constant.is_integer) {
          printf("NUMBER @ %u:%u (%lld)\n",
                 token->position.line,
                 token->position.character,
                 token->constant.integer);
        } else if (token->constant.is_floating_point) {
          printf("NUMBER @ %u:%u (%f)\n",
                 token->position.line,
                 token->position.character,
                 token->constant.floating_point);
        }

        break;

      case SCC_IR_TOKEN_L_PARENTHESIS:
      case SCC_IR_TOKEN_R_PARENTHESIS:
        printf("PARENTHESIS @ %u:%u (%s)\n",
               token->position.line,
               token->position.character,
               (token->type == SCC_IR_TOKEN_L_PARENTHESIS) ? "OPEN" : "CLOSE");
        break;
      case SCC_IR_TOKEN_L_BRACE:
      case SCC_IR_TOKEN_R_BRACE:
        printf("BRACE @ %u:%u (%s)\n",
               token->position.line,
               token->position.character,
               (token->type == SCC_IR_TOKEN_L_BRACE) ? "OPEN" : "CLOSE");
        break;
      case SCC_IR_TOKEN_L_BRACKET:
      case SCC_IR_TOKEN_R_BRACKET:
        printf("BRACKET @ %u:%u (%s)\n",
               token->position.line,
               token->position.character,
               (token->type == SCC_IR_TOKEN_L_BRACKET) ? "OPEN" : "CLOSE");
        break;

      case SCC_IR_TOKEN_COMMA:
        printf("COMMA @ %u:%u\n",
               token->position.line,
               token->position.character);
        break;
      case SCC_IR_TOKEN_EQUALS:
        printf("EQUALS @ %u:%u\n",
               token->position.line,
               token->position.character);
        break;

      case SCC_IR_TOKEN_UNKNOWN:
        printf("UNKNOWN @ %u:%u (%u)\n",
               token->position.line,
               token->position.character,
               token->length);
        break;

      default:
        break;
    }
  }

  if (parser->lexer.scanner.errors) {
    parser->failed = SCC_TRUE;
  } else {
    // TODO(mtwilliams): Parse tokens.
  }

  return !(parser->failed);
}

void scc_ir_parser_report_all_errors(const scc_ir_parser_t *parser) {
  const scc_lexer_error_t *error = parser->lexer.scanner.errors;

  // TODO(mtwilliams): Better error reporting.
  while (error) {
    fprintf(stderr, "Error: %s\n", error->message);
    error = error->next;
  }
}

void scc_ir_parse(scc_feed_t *feed,
                  const scc_ir_parse_options_t *options) {
  scc_assert_paranoid(feed != NULL);
  scc_assert_paranoid(options != NULL);

  // TODO(mtwilliams): Wrap |feed| for error reporting.
  scc_feed_t *wrapped = feed;

  scc_ir_parser_t *parser = scc_ir_parser_create(wrapped, options);

  const scc_bool_t success = scc_ir_parser_parse(parser);

  if (!success) {
    scc_ir_parser_report_all_errors(parser);

    // TODO(mtwilliams): Error handling.
    SCC_TRAP();
  }

  scc_ir_parser_destroy(parser);
}

SCC_END_EXTERN_C

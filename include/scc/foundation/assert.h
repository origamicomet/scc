//===-- scc/foundation/assert.h ------------------------*- mode: C++11 -*-===//
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
//
// TODO(mtwilliams): Document the purpose of this file.
//
//===----------------------------------------------------------------------===//

#ifndef _SCC_FOUNDATION_ASSERT_H_
#define _SCC_FOUNDATION_ASSERT_H_

#include "scc/config.h"
#include "scc/linkage.h"

#include "scc/foundation/types.h"
#include "scc/foundation/support.h"

SCC_BEGIN_EXTERN_C

/// Describes a (failed) assertion.
typedef struct scc_assertion {
  /// The predicate that failed.
  const char *predicate;

  /// A custom, more friendly description of why the predicate failed.
  const char *reason;

  /// Describes where the assertion is in the codebase.
  struct {
    const char *file;
    size_t line;
  } location;
} scc_assertion_t;

///
typedef void (*scc_assertion_handler_t)(const scc_assertion_t *assertion, void *ctx);

///
extern SCC_PUBLIC
void scc_assertion_handler(
  scc_assertion_handler_t *assertion_handler,
  void **ctx);

///
extern SCC_PUBLIC
void scc_set_assertion_handler(
  scc_assertion_handler_t new_assertion_handler,
  void *ctx = NULL);

SCC_END_EXTERN_C

#include <stdio.h>

// REFACTOR(mtwilliams): Extract into a common header?
#if SCC_COMPILER == SCC_COMPILER_MSVC
  #include <malloc.h>
  #ifndef alloca
    #define alloca _alloca
  #endif
#endif

/// \def scc_assert
/// \brief Raises a fuss if @_Predicate doesn't evaluate to true.
///
#define scc_assert(_Predicate) \
  do { \
    if (!(_Predicate)) { \
      scc_assertion_t assertion; \
      assertion.predicate = #_Predicate; \
      assertion.reason = NULL; \
      assertion.location.file = __FILE__ ; \
      assertion.location.line = (size_t)__LINE__; \
      scc_assertion_handler_t assertion_handler; \
      void *assertion_handler_ctx; \
      scc_assertion_handler(&assertion_handler, &assertion_handler_ctx); \
      if (assertion_handler != NULL) \
        assertion_handler(&assertion, assertion_handler_ctx); \
      SCC_TRAP(); \
    } \
  } while (0,0)

/// \def scc_assertf
/// \brief Raises a fuss if @_Predicate doesn't evaluate to true, providing
/// user-specified, formatted reasoning.
///
#define scc_assertf(_Predicate, _Reason_format, ...) \
  do { \
    if (!(_Predicate)) { \
      scc_assertion_t assertion; \
      assertion.predicate = #_Predicate; \
      const size_t reason_len = snprintf(NULL, 0, _Reason_format, ##__VA_ARGS__) + 1; \
      const char *reason = (const char *)alloca(reason_len); \
      snprintf((char *)reason, reason_len, _Reason_format, ##__VA_ARGS__); \
      assertion.reason = reason; \
      assertion.location.file = __FILE__; \
      assertion.location.line = (size_t)__LINE__; \
      scc_assertion_handler_t assertion_handler; \
      void *assertion_handler_ctx; \
      scc_assertion_handler(&assertion_handler, &assertion_handler_ctx); \
      if (assertion_handler != NULL) \
        assertion_handler(&assertion, assertion_handler_ctx); \
      SCC_TRAP(); \
    } \
  } while (0,0)

#if defined(DOXYGEN)
  #define scc_assert_debug scc_assert
  #define scc_assertf_debug scc_assertf
  #define scc_assert_development scc_assert
  #define scc_assertf_development scc_assertf
  #define scc_assert_release scc_assert
  #define scc_assertf_release scc_assertf
#else
  #if SCC_CONFIGURATION == SCC_CONFIGURATION_DEBUG
    #define scc_assert_paranoid scc_assert
    #define scc_assertf_paranoid scc_assertf
    #define scc_assert_debug scc_assert
    #define scc_assertf_debug scc_assertf
  #else
    #define scc_assert_paranoid(...)
    #define scc_assertf_paranoid(...)
    #define scc_assert_debug(...)
    #define scc_assertf_debug(...)
  #endif

  #if SCC_CONFIGURATION <= SCC_CONFIGURATION_RELEASE
    #define scc_assert_release scc_assert
    #define scc_assertf_release scc_assertf
  #else
    #define scc_assert_release(...)
    #define scc_assertf_release(...)
  #endif
#endif

#endif // _SCC_FOUNDATION_ASSERT_H_

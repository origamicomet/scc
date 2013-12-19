//===-- scc/foundation/support/inlining.h ---------------*- mode: C++11 -*-===//
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
/// \brief Defines pre-processor macros that assist in conveying to the
/// compiler when code should be inlined.
///
//===----------------------------------------------------------------------===//

#ifndef _SCC_FOUNDATION_SUPPORT_INLINING_H_
#define _SCC_FOUNDATION_SUPPORT_INLINING_H_

/// \def SCC_INLINE
/// \brief Code should be inlined.
#if defined(DOXYGEN)
  #define SCC_INLINE
#else // !defined(DOXYGEN)
  #if defined(_MSC_VER)
    #define SCC_INLINE __forceinline
  #elif defined(__clang__) || defined(__GNUC__)
    #define SCC_INLINE __inline __attribute__ ((always_inline))
  #endif
#endif

#endif // _SCC_FOUNDATION_SUPPORT_INLINING_H_

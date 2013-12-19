//===-- scc/foundation/support/reachability.h -----------*- mode: C++11 -*-===//
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
/// compiler the reachability of code.
///
//===----------------------------------------------------------------------===//

#ifndef _SCC_FOUNDATION_SUPPORT_REACHABILITY_H_
#define _SCC_FOUNDATION_SUPPORT_REACHABILITY_H_

/// \def SCC_TRAP
/// \brief Errant, but reachable, code path.
#if defined(DOXYGEN)
  #define SCC_TRAP()
#else // !defined(DOXYGEN)
  #if defined(_MSC_VER)
    #define SCC_TRAP() __debugbreak()
  #elif defined(__GNUC__)
    #define SCC_TRAP() __builtin_trap()
  #endif
#endif

/// \def SCC_UNREACHABLE
/// \brief Code is unreachable.
#if defined(DOXYGEN)
  #define SCC_UNREACHABLE()
#else // !defined(SCC_UNREACHABLE)
  #if defined(_MSC_VER)
    #define SCC_UNREACHABLE() __assume(0)
  #elif defined(__clang__)
    #define SCC_UNREACHABLE() __builtin_unreachable()
  #elif defined(__GNUC__)
    #if __GNUC_VERSION__ >= 40500
      #define SCC_UNREACHABLE() __builtin_unreachable()
    #else // __GNUC_VERSION__ < 40500
      #include <signal.h>
      #define SCC_UNREACHABLE() do { ::signal(SIGTRAP); } while(0, 0)
    #endif
  #endif
#endif

#endif // _SCC_FOUNDATION_SUPPORT_REACHABILITY_H_

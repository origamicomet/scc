//===-- scc/linkage.h -----------------------------------*- mode: C++11 -*-===//
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
/// \brief Defines pre-processor macros that specify symbol visibility.
///
//===----------------------------------------------------------------------===//

#ifndef _SCC_LINKAGE_H_
#define _SCC_LINKAGE_H_

/// \def SCC_LINKAGE_INTERNAL
/// \brief Exports symbols for inclusion of SCC via objects.
#define SCC_LINKAGE_INTERNAL 0

/// \def SCC_LINKAGE_STATIC
/// \brief Exports symbols for inclusion of SCC via static library.
#define SCC_LINKAGE_STATIC 1

/// \def SCC_LINKAGE_DYNAMIC
/// \brief Exports symbols for inclusion of SCC via dynamic library.
#define SCC_LINKAGE_DYNAMIC 2

/// \def SCC_LINKAGE
/// \brief Specifies how you intend to link to SCC.
#if !defined(SCC_LINKAGE)
  #error ("Please specify how you intend on linking to SCC by defining `SCC_LINKAGE'.")
#endif

/// \def SCC_LOCAL
/// \brief Marks a symbol for internal usage.
#if defined (DOXYGEN)
  #define SCC_LOCAL
#else // #if !defined(DOXYGEN)
  #if SCC_LINKAGE == SCC_LINKAGE_STATIC
    #define SCC_LOCAL
  #elif SCC_LINKAGE == SCC_LINKAGE_DYNAMIC
    #if defined (__SCC_IS_BEING_COMPILED__)
      #if defined(__GNUC__)
        #if __GNUC__ >= 4
          #define SCC_LOCAL __attribute__ ((visibility ("hidden")))
        #else
          #define SCC_LOCAL
        #endif
      #elif defined(__clang__)
        #define SCC_LOCAL __attribute__ ((visibility ("hidden")))
      #elif defined(_MSC_VER) || defined(__CYGWIN__)
        #define SCC_LOCAL
      #else
        #error ("Unknown or unsupported toolchain!")
      #endif
    #else // #if !defined(__SCC_IS_BEING_COMPILED__)
      #define SCC_LOCAL
    #endif
  #endif
#endif

/// \def SCC_PUBLIC
/// \brief Marks a symbol for public usage.
#if defined (DOXYGEN)
  #define SCC_PUBLIC
#else // #if !defined(DOXYGEN)
  #if SCC_LINKAGE == SCC_LINKAGE_STATIC
    #define SCC_PUBLIC
  #elif SCC_LINKAGE == SCC_LINKAGE_DYNAMIC
    #if defined (__SCC_IS_BEING_COMPILED__)
      #if defined(__GNUC__)
        #if __GNUC__ >= 4
          #define SCC_PUBLIC __attribute__ ((visibility ("default")))
        #else
          #define SCC_PUBLIC
        #endif
      #elif defined(__clang__)
        #define SCC_PUBLIC __attribute__ ((visibility ("default")))
      #elif defined(_MSC_VER) || defined(__CYGWIN__)
        #define SCC_PUBLIC __declspec(dllexport)
      #else
        #error ("Unknown or unsupported toolchain!")
      #endif
    #else // #if !defined(__SCC_IS_BEING_COMPILED__)
      #if (defined(__GNUC__) && (__GNUC__ >= 4))
        #define SCC_PUBLIC
      #elif defined(__clang__)
        #define SCC_PUBLIC
      #elif defined(_MSC_VER) || defined(__CYGWIN__)
        #define SCC_PUBLIC __declspec(dllimport)
      #else
        #error ("Unknown or unsupported toolchain!")
      #endif
    #endif
  #endif
#endif

/// \def SCC_BEGIN_EXTERN_C
/// \internal
/// \def SCC_END_EXTERN_C
/// \internal
#if defined (DOXYGEN)
  #define SCC_BEGIN_EXTERN_C
  #define SCC_END_EXTERN_C
#else // #if !defined(DOXYGEN)
  #if defined(__cplusplus)
    #define SCC_BEGIN_EXTERN_C extern "C" {
    #define SCC_END_EXTERN_C }
  #else // #if !defined(__cplusplus)
    #define SCC_BEGIN_EXTERN_C
    #define SCC_END_EXTERN_C
  #endif
#endif

/// \def SCC_BEGIN_NAMESPACE
/// \internal
/// \def SCC_END_NAMESPACE
/// \internal
#define SCC_BEGIN_NAMESPACE namespace scc {
#define SCC_END_NAMESPACE } // scc

#endif // _SCC_LINKAGE_H_

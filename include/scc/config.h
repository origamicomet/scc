//===-- scc/config.h ------------------------------------*- mode: C++11 -*-===//
//
//                             _____     _
//                            |   __|___| |___
//                            |  |  | .'| | .'|
//                            |_____|__,|_|__,|
//
//       This file is distributed under the terms described in LICENSE.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines and checks the sanity of various pre-processor macros that
/// control the building and behaviour of SCC.
///
//===----------------------------------------------------------------------===//

#ifndef _SCC_CONFIG_H_
#define _SCC_CONFIG_H_

/// \def SCC_CONFIGURATION_DEBUG
/// \brief Debug build.
#define SCC_CONFIGURATION_DEBUG 1

/// \def SCC_CONFIGURATION_RELEASE
/// \brief Release build.
#define SCC_CONFIGURATION_RELEASE 2

/// \def SCC_CONFIGURATION
/// \brief Specifies how "loose and fast" SCC is.
#if defined(DOXYGEN)
  #define SCC_CONFIGURATION
#else
  #if !defined(SCC_CONFIGURATION)
    #error ("No configuration specified! Please specify a configuration by defining `SCC_CONFIGURATION'.")
  #else
    #if (SCC_CONFIGURATION != SCC_CONFIGURATION_DEBUG) && \
        (SCC_CONFIGURATION != SCC_CONFIGURATION_RELEASE)
      #error ("Unknown configuration specified. See include/scc/config.h for a list of possible configurations.")
    #endif
  #endif
#endif

/// \def SCC_PLATFORM_WINDOWS
/// \brief Windows
#define SCC_PLATFORM_WINDOWS 1

/// \def SCC_PLATFORM_MAC
/// \brief macOS
#define SCC_PLATFORM_MAC 2

/// \def SCC_PLATFORM_LINUX
/// \brief Linux
#define SCC_PLATFORM_LINUX 3

/// \def SCC_PLATFORM
/// \brief Target platform.
#if defined(DOXYGEN)
  #define SCC_PLATFORM
#else // !defined(DOXYGEN)
  #if defined(_WIN32) || defined(_WIN64)
    #define SCC_PLATFORM SCC_PLATFORM_WINDOWS
  #elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if defined(TARGET_OS_IPHONE)
      #define SCC_PLATFORM SCC_PLATFORM_IOS
    #else
      #define SCC_PLATFORM SCC_PLATFORM_MAC
    #endif
  #elif defined(__linux__)
    #define SCC_PLATFORM SCC_PLATFORM_LINUX
  #else
    #error ("Unknown or unsupported platform!")
  #endif
#endif

/// \def SCC_COMPILER_MSVC
/// \brief Microsoft Visual C/C++
#define SCC_COMPILER_MSVC 1

/// \def SCC_COMPILER_GCC
/// \brief GNU Compiler Collection
#define SCC_COMPILER_GCC 2

/// \def SCC_COMPILER_CLANG
/// \brief LLVM/Clang
#define SCC_COMPILER_CLANG 3

/// \def SCC_COMPILER
/// \brief Host compiler.
#if defined(DOXYGEN)
  #define SCC_COMPILER
#else // !defined(DOXYGEN)
  #if defined(_MSC_VER)
    #define SCC_COMPILER SCC_COMPILER_MSVC
    #define SCC_MICROSOFT_VISUAL_STUDIO_2013 1800
    #define SCC_MICROSOFT_VISUAL_STUDIO_2012 1700
    #define SCC_MICROSOFT_VISUAL_STUDIO_2010 1600
    #define SCC_MICROSOFT_VISUAL_STUDIO_2008 1500
    #define SCC_MICROSOFT_VISUAL_STUDIO_2005 1400
    #define SCC_MICROSOFT_VISUAL_STUDIO_2003 1310
    #define SCC_MICROSOFT_VISUAL_STUDIO_2002 1300
    #define SCC_MICROSOFT_VISUAL_C_6 1200
    #if (_MSC_VER >= MICROSOFT_VISUAL_STUDIO_2005)
      // ASSUME(mtwilliams): Microsoft Visual Studio 2005 or newer.
    #elif (_MSC_VER == MICROSOFT_VISUAL_STUDIO_2003)
      #error ("Microsoft Visual C/C++ .NET 2003 is unsupported! Please upgrade to Microsoft Visual C/C++ 2005 or newer.")
    #elif (_MSC_VER == MICROSOFT_VISUAL_STUDIO_2002)
      #error ("Microsoft Visual C/C++ .NET 2002 is unsupported! Please upgrade to Microsoft Visual C/C++ 2005 or newer.")
      // Not to mention that it has major bugs so annoying that Microsoft provides
      // complementary upgrades to Microsoft Visual C/C++ .NET 2003!
    #elif (_MSC_VER == SCC_MICROSOFT_VISUAL_C_6)
      #error ("Microsoft Visual C/C++ 6 is unsupported! Please upgrade to Microsoft Visual C/C++ 2005 or newer.")
      // If we did ever want to support it, we should check for Service Pack 6 by
      // checking if _MSC_FULL_VER is less than 12008804.
    #else
      #error ("Your version of Microsoft Visual C/C++ is unsupported! Please upgrade to Microsoft Visual C/C++ 2005 or newer.")
    #endif
  #elif defined(__GNUC__)
    #if defined(__clang__)
      #define SCC_COMPILER SCC_COMPILER_CLANG
      #define __CLANG_VERSION__ (__clang_major__ * 10000 \
                                 + __clang_minor__ * 100 \
                                 + __clang_patchlevel__)
    #else // !defined(__clang__)
      // ASSUME(mtwilliams): That we're being compiled with GCC.
      #define SCC_COMPILER SCC_COMPILER_GCC
      #if defined(__GNUC_PATCHLEVEL__)
        #define __GNUC_VERSION__ (__GNUC__ * 10000 \
                                  + __GNUC_MINOR__ * 100 \
                                  + __GNUC_PATCHLEVEL__)
      #else
        #define __GNUC_VERSION__ (__GNUC__ * 10000 \
                                  + __GNUC_MINOR__ * 100)
      #endif
    #endif
  #else
    #error ("Unknown or unsupported compiler!")
  #endif
#endif

/// \def SCC_ARCHITECTURE_X86
/// \brief Intel/AMD x86.
#define SCC_ARCHITECTURE_X86 1

/// \def SCC_ARCHITECTURE_X86_64
/// \brief Intel/AMD x86_64.
#define SCC_ARCHITECTURE_X86_64 2

/// \def SCC_ARCHITECTURE
/// \brief Target architecture.
#if defined(DOXYGEN)
  #define SCC_COMPILER
#else // !defined(DOXYGEN)
  #if defined(_M_IX86) || defined(__i386__)
    #define SCC_ARCHITECTURE SCC_ARCHITECTURE_X86
  #elif defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(__amd64__)
    #define SCC_ARCHITECTURE SCC_ARCHITECTURE_X86_64
  #else
    #error ("Unknown or unsupported architecture!")
  #endif
#endif

#define SCC_ASCII 1
#define SCC_UNICODE 2

/// \def SCC_CHARACTER_SET
/// \brief Compile to handle a particular character set.
#if !defined(DOXYGEN)
  #define SCC_CHARACTER_SET SCC_ASCII
#endif

#endif // _SCC_CONFIG_H_

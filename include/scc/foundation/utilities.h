//===-- scc/foundation/utilities.h ----------------------*- mode: C++11 -*-===//
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
/// \brief A collection of utilities.
///
//===----------------------------------------------------------------------===//

#ifndef _SCC_FOUNDATION_UTILITIES_H_
#define _SCC_FOUNDATION_UTILITIES_H_

#if defined(_MSC_VER)
  #include <intrin.h>

  #pragma intrinsic(_BitScanForward)
  #pragma intrinsic(_BitScanForward64)
  #pragma intrinsic(_BitScanReverse)
  #pragma intrinsic(_BitScanReverse64)
#endif

#include "scc/foundation/types.h"
#include "scc/foundation/support.h"

/// \def SCC_MIN
/// \brief Computes minimum of `a` and `b`.
#define SCC_MIN(a, b) \
  (((a) > (b)) ? (b) : (a))

/// \def SCC_MAX
/// \brief Computes maximum of `a` and `b`.
#define SCC_MAX(a, b) \
  (((a) > (b)) ? (a) : (b))

/// \def SCC_BITS_TO_BYTES
/// \brief Computes the number of bytes required to represent `n` bits.
#define SCC_BITS_TO_BYTES(n) \
  (((n) + 7) / 8)

/// \def SCC_BYTES_TO_BITS
/// \brief Computes the number of bits represented by `n` bytes.
#define SCC_BYTES_TO_BITS(n) \
  ((n) * 8)

/// \def SCC_ALIGN_TO_BOUNDARY
/// \brief Aligns `n` to `alignment` bytes boundary.
#define SCC_ALIGN_TO_BOUNDARY(n, alignment) \
  ((((n) + (alignment) - 1) / (alignment)) * (alignment))

/// Counts number of leading zeros.
static SCC_INLINE scc_uint32_t scc_clzul(scc_uint32_t n) {
#if defined(_MSC_VER)
  scc_uint32_t bit;
  return _BitScanReverse((unsigned long *)&bit, n) ? (31 - bit) : 32;
#elif defined(__clang__) || defined(__GNUC__)
  return n ? __builtin_clzl(n) : 32;
#endif
}

/// Counts number of trailing zeros.
static SCC_INLINE scc_uint32_t scc_ctzul(scc_uint32_t n) {
#if defined(_MSC_VER)
  scc_uint32_t bit;
  return _BitScanForward((unsigned long *)&bit, n) ? bit : 32;
#elif defined(__clang__) || defined(__GNUC__)
  return n ? __builtin_ctzl(n) : 32;
#endif
}

#if defined(_MSC_VER)
  extern SCC_LOCAL const scc_uint8_t SCC_POPCNTUB_TABLE[256];
#endif

/// Counts the number of set bits.
static SCC_INLINE scc_uint32_t scc_popcntul(scc_uint32_t n) {
#if defined(_MSC_VER)
  const scc_uint8_t *bytes = (const scc_uint8_t *)&n;
  return SCC_POPCNTUB_TABLE[bytes[0]] + 
         SCC_POPCNTUB_TABLE[bytes[1]] + 
         SCC_POPCNTUB_TABLE[bytes[2]] + 
         SCC_POPCNTUB_TABLE[bytes[3]];
#elif defined(__clang__) || defined(__GNUC__)
  return __builtin_popcountl(n);
#endif
}

/// Counts the number of set bits.
static SCC_INLINE scc_uint32_t scc_popcntull(scc_uint64_t n) {
#if defined(_MSC_VER)
  const scc_uint8_t *bytes = (const scc_uint8_t *)&n;
  return SCC_POPCNTUB_TABLE[bytes[0]] + 
         SCC_POPCNTUB_TABLE[bytes[1]] + 
         SCC_POPCNTUB_TABLE[bytes[2]] + 
         SCC_POPCNTUB_TABLE[bytes[3]] +
         SCC_POPCNTUB_TABLE[bytes[4]] + 
         SCC_POPCNTUB_TABLE[bytes[5]] + 
         SCC_POPCNTUB_TABLE[bytes[6]] + 
         SCC_POPCNTUB_TABLE[bytes[7]];
#elif defined(__clang__) || defined(__GNUC__)
  return __builtin_popcountll(n);
#endif
}

/// \def SCC_IS_EVEN
/// \brief Quickly determines if `n` is an even number.
#define SCC_IS_EVEN(n) \
  (((n) % 2) == 0)

/// \def SCC_IS_ODD
/// \brief Quickly determines if `n` is an odd number.
#define SCC_IS_ODD(n) \
  (((n) % 2) == 1)

/// \def SCC_IS_POWER_OF_TWO
/// \brief Quickly determines if `n` is a power of two.
#define SCC_IS_POWER_OF_TWO(n) \
  ((n) && !((n) & ((n) - 1)))

/// Quickly computes the base-2 logarithm of `n`.
static SCC_INLINE scc_uint32_t scc_log2ul(scc_uint32_t n) {
  return n ? (31 - scc_clzul(n)) : 0;
}

/// Quickly computes the base-2 logarithm with `n` rounded up to the nearest
/// power of two.
static SCC_INLINE scc_uint32_t scc_log2ul_ceil(scc_uint32_t n) {
  const scc_uint32_t logarithm = scc_log2ul(n);
  const scc_uint32_t non_power_of_two = (n & (n - 1));
  return logarithm + (non_power_of_two ? 1 : 0);
}

// TODO(mtwilliams): Fallback for _BitScanReverse64 on x86.
#if 0
  /// Quickly computes the base-2 logarithim of `n`.
  static SCC_INLINE scc_uint64_t scc_log2ull(scc_uint64_t n) {
  }
#endif

#endif // _SCC_FOUNDATION_UTILITIES_H_

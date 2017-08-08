//===-- scc/foundation/atomics.h ------------------------*- mode: C++11 -*-===//
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

#ifndef _SCC_FOUNDATION_UTILITIES_H_
#define _SCC_FOUNDATION_UTILITIES_H_

#include "scc/foundation/types.h"
#include "scc/foundation/support.h"

#if SCC_COMPILER == SCC_COMPILER_MSVC
  #include <intrin.h>
 
  #if SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86
    #pragma intrinsic(_InterlockedExchangeAdd)
    #pragma intrinsic(_InterlockedCompareExchange)
    #pragma intrinsic(_InterlockedCompareExchange64)
  #elif SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64
    #pragma intrinsic(_InterlockedExchangeAdd)
    #pragma intrinsic(_InterlockedExchangeAdd64)
    #pragma intrinsic(_InterlockedCompareExchange)
    #pragma intrinsic(_InterlockedCompareExchange64)
  #endif
#elif (SCC_COMPILER == SCC_COMPILER_GCC) || \
      (SCC_COMPILER == SCC_COMPILER_CLANG)
#endif

SCC_BEGIN_EXTERN_C

SCC_INLINE scc_int32_t scc_atomic_load_i32(const volatile scc_int32_t *v) {
#if (SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86) || \
    (SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64)
  return *v;
#endif
}

SCC_INLINE scc_uint32_t scc_atomic_load_u32(const volatile scc_uint32_t *v) {
#if (SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86) || \
    (SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64)
  return *v;
#endif
}

SCC_INLINE scc_int64_t scc_atomic_load_i64(const volatile scc_int64_t *v) {
#if SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86
  #if SCC_COMPILER == SCC_COMPILER_MSVC
    scc_int64_t r;
    __asm {
      mov esi, v;
      mov ebx, eax;
      mov ecx, edx;
      lock cmpxchg8b [esi];
      mov dword ptr r, eax;
      mov dword ptr r[4], edx;
    }
    return r;
  #elif (SCC_COMPILER == SCC_COMPILER_GCC) || \
        (SCC_COMPILER == SCC_COMPILER_CLANG)
    scc_int64_t r;
    asm volatile("movl %%ebx, %%eax\n"
                 "movl %%ecx, %%edx\n"
                 "lock; cmpxchg8b %1"
                 : "=&A"(r)
                 : "m"(v));
    return r;
  #endif
#elif SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64
  return *v;
#endif
}

SCC_INLINE scc_uint64_t scc_atomic_load_u64(const volatile scc_uint64_t *v) {
#if SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86
  #if SCC_COMPILER == SCC_COMPILER_MSVC
    scc_uint64_t r;
    __asm {
      mov esi, v;
      mov ebx, eax;
      mov ecx, edx;
      lock cmpxchg8b [esi];
      mov dword ptr r, eax;
      mov dword ptr r[4], edx;
    }
    return r;
  #elif (SCC_COMPILER == SCC_COMPILER_GCC) || \
        (SCC_COMPILER == SCC_COMPILER_CLANG)
    scc_uint64_t r;
    asm volatile("movl %%ebx, %%eax\n"
                 "movl %%ecx, %%edx\n"
                 "lock; cmpxchg8b %1"
                 : "=&A"(r)
                 : "m"(v));
    return r;
  #endif
#elif SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64
  return *v;
#endif
}

SCC_INLINE void *scc_atomic_load_ptr(void ** const volatile v) {
#if (SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86) || \
    (SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64)
  return *v;
#endif
}

SCC_INLINE void scc_atomic_store_i32(volatile scc_int32_t *v, const scc_int32_t desired) {
#if (SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86) || \
    (SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64)
  *v = desired;
#endif
}

SCC_INLINE void scc_atomic_store_u32(volatile scc_uint32_t *v, const scc_uint32_t desired) {
#if (SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86) || \
    (SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64)
  *v = desired;
#endif
}

SCC_INLINE void scc_atomic_store_i64(volatile scc_int64_t *v, const scc_int64_t desired) {
#if SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86
  #if SCC_COMPILER == SCC_COMPILER_MSVC
    __asm {
      mov esi, v;
      mov ebx, dword ptr desired;
      mov ecx, dword ptr desired[4];
    retry:
      cmpxchg8b [esi];
      jne retry;
    }
  #elif (SCC_COMPILER == SCC_COMPILER_GCC) || \
        (SCC_COMPILER == SCC_COMPILER_CLANG)
    scc_int64_t expected = *v;
    asm volatile("1:  cmpxchg8b %0\n"
                 "    jne 1b"
                 : "=m"(*v)
                 : "b"((u32)desired), "c"((u32)(desired >> 32)), "A"(expected));
  #endif
#elif SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64
  *v = desired;
#endif
}

SCC_INLINE void scc_atomic_store_u64(volatile scc_uint64_t *v, const scc_uint64_t desired) {
  scc_atomic_store_i64((scc_int64_t *)v, (scc_int64_t)desired);
}

SCC_INLINE void scc_atomic_store_ptr(void ** volatile v, void * const desired) {
#if (SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86) || \
    (SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64)
  *v = desired;
#endif
}

SCC_INLINE scc_int32_t scc_atomic_add_i32(volatile scc_int32_t *lhs, const scc_int32_t rhs) {
#if SCC_COMPILER == SCC_COMPILER_MSVC
  return _InterlockedExchangeAdd((volatile long *)lhs, (long)rhs);
#elif (SCC_COMPILER == SCC_COMPILER_GCC) || \
      (SCC_COMPILER == SCC_COMPILER_CLANG)
  return __sync_fetch_and_add(lhs, rhs);
#endif
}

SCC_INLINE scc_uint32_t scc_atomic_add_u32(volatile scc_uint32_t *lhs, const scc_uint32_t rhs) {
  return (scc_uint32_t)scc_atomic_add_i32((scc_int32_t *)lhs, (scc_int32_t)rhs);
}

SCC_INLINE scc_int64_t scc_atomic_add_i64(volatile scc_int64_t *lhs, const scc_int64_t rhs) {
#if SCC_COMPILER == SCC_COMPILER_MSVC
  #if SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86
    // TODO(mtwilliams): Use XADD?
    // http://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/tree/arch/x86/include/asm/atomic.h?id=HEAD
    scc_int64_t r;
    __asm {
    retry:
      mov esi, lhs;
      mov eax, [esi];
      mov edx, [esi+4];
      mov ebx, dword ptr rhs;
      mov ecx, dword ptr rhs[4];
      add ebx, eax;
      adc ecx, edx;
      lock cmpxchg8b [esi];
      jne retry;
      mov dword ptr r, eax;
      mov dword ptr r[4], edx;
    }
    return r;
  #elif SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64
    return _InterlockedExchangeAdd64((volatile __int64 *)lhs, (__int64)rhs);
  #endif
#elif (SCC_COMPILER == SCC_COMPILER_GCC) || \
      (SCC_COMPILER == SCC_COMPILER_CLANG)
  return __sync_fetch_and_add(lhs, rhs);
#endif
}

SCC_INLINE scc_uint64_t scc_atomic_add_u64(volatile scc_uint64_t *lhs, const scc_uint64_t rhs) {
  return (scc_uint64_t)scc_atomic_add_i64((scc_int64_t *)lhs, (scc_int64_t)rhs);
}

SCC_INLINE scc_int32_t scc_atomic_sub_i32(volatile scc_int32_t *lhs, const scc_int32_t rhs) {
#if SCC_COMPILER == SCC_COMPILER_MSVC
  return _InterlockedExchangeAdd((volatile long *)lhs, -((long)rhs));
#elif (SCC_COMPILER == SCC_COMPILER_GCC) || \
      (SCC_COMPILER == SCC_COMPILER_CLANG)
  return __sync_fetch_and_sub(lhs, rhs);
#endif
}

SCC_INLINE scc_uint32_t scc_atomic_sub_u32(volatile scc_uint32_t *lhs, const scc_uint32_t rhs) {
  return (scc_uint32_t)scc_atomic_sub_i32((scc_int32_t *)lhs, (scc_int32_t)rhs);
}

SCC_INLINE scc_int64_t scc_atomic_sub_i64(volatile scc_int64_t *lhs, const scc_int64_t rhs) {
#if SCC_COMPILER == SCC_COMPILER_MSVC
  #if SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86
    scc_int64_t r;
    __asm {
    retry:
      mov esi, lhs;
      mov eax, [esi];
      mov edx, [esi+4];
      mov ebx, dword ptr rhs;
      mov ecx, dword ptr rhs[4];
      sub ebx, eax;
      sbb ecx, edx;
      lock cmpxchg8b [esi];
      jne retry;
      mov dword ptr r, eax;
      mov dword ptr r[4], edx;
    }
    return r;
  #elif SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64
    return _InterlockedExchangeAdd64((volatile __int64 *)lhs, -((__int64)rhs));
  #endif
#elif (SCC_COMPILER == SCC_COMPILER_GCC) || \
      (SCC_COMPILER == SCC_COMPILER_CLANG)
  return __sync_fetch_and_sub(lhs, rhs);
#endif
}

SCC_INLINE scc_uint64_t scc_atomic_sub_u64(volatile scc_uint64_t *lhs, const scc_uint64_t rhs) {
  return (scc_uint64_t)scc_atomic_sub_i64((scc_int64_t *)lhs, (scc_int64_t)rhs);
}

SCC_INLINE scc_int32_t scc_atomic_increment_i32(volatile scc_int32_t *v) {
  return scc_atomic_add_i32(v, 1);
}

SCC_INLINE scc_uint32_t scc_atomic_increment_u32(volatile scc_uint32_t *v) {
  return scc_atomic_add_u32(v, 1);
}

SCC_INLINE scc_int64_t scc_atomic_increment_i64(volatile scc_int64_t *v) {
  return scc_atomic_add_i64(v, 1);
}

SCC_INLINE scc_uint64_t scc_atomic_increment_u64(volatile scc_uint64_t *v) {
  return scc_atomic_add_u64(v, 1);
}

SCC_INLINE scc_int32_t scc_atomic_decrement_i32(volatile scc_int32_t *v) {
  return scc_atomic_sub_i32(v, 1);
}

SCC_INLINE scc_uint32_t scc_atomic_decrement_u32(volatile scc_uint32_t *v) {
  return scc_atomic_sub_u32(v, 1);
}

SCC_INLINE scc_int64_t scc_atomic_decrement_i64(volatile scc_int64_t *v) {
  return scc_atomic_sub_i64(v, 1);
}

SCC_INLINE scc_uint64_t scc_atomic_decrement_u64(volatile scc_uint64_t *v) {
  return scc_atomic_sub_u64(v, 1);
}

SCC_INLINE scc_int32_t scc_atomic_cmp_and_xchg_i32(volatile scc_int32_t *v, const scc_int32_t expected, const scc_int32_t desired) {
#if SCC_COMPILER == SCC_COMPILER_MSVC
  return _InterlockedCompareExchange((volatile long *)v, (long)desired, (long)expected);
#elif (SCC_COMPILER == SCC_COMPILER_GCC) || \
      (SCC_COMPILER == SCC_COMPILER_CLANG)
  return __sync_val_compare_and_swap(v, expected, desired);
#endif
}

SCC_INLINE scc_uint32_t scc_atomic_cmp_and_xchg_u32(volatile scc_uint32_t *v, const scc_uint32_t expected, const scc_uint32_t desired) {
  return (scc_uint32_t)scc_atomic_cmp_and_xchg_i32((scc_int32_t *)v, (scc_int32_t)expected, (scc_int32_t)desired);
}

SCC_INLINE scc_int64_t scc_atomic_cmp_and_xchg_i64(volatile scc_int64_t *v, const scc_int64_t expected, const scc_int64_t desired) {
#if SCC_COMPILER == SCC_COMPILER_MSVC
  return _InterlockedCompareExchange64((volatile __int64 *)v, (__int64)desired, (__int64)expected);
#elif (SCC_COMPILER == SCC_COMPILER_GCC) || \
      (SCC_COMPILER == SCC_COMPILER_CLANG)
  return __sync_val_compare_and_swap(v, expected, desired);
#endif
}

SCC_INLINE scc_uint64_t scc_atomic_cmp_and_xchg_u64(volatile scc_uint64_t *v, const scc_uint64_t expected, const scc_uint64_t desired) {
  return (scc_uint64_t)scc_atomic_cmp_and_xchg_i64((scc_int64_t *)v, (scc_int64_t)expected, (scc_int64_t)desired);
}

SCC_INLINE void *scc_atomic_cmp_and_xchg_ptr(void ** volatile v, const void *expected, const void *desired) {
#if SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86
  return (void *)scc_atomic_cmp_and_xchg_u32((scc_uint32_t *)v, (scc_uint32_t)expected, (scc_uint32_t)desired);
#elif SCC_ARCHITECTURE == SCC_ARCHITECTURE_X86_64
  return (void *)scc_atomic_cmp_and_xchg_u64((scc_uint64_t *)v, (scc_uint64_t)expected, (scc_uint64_t)desired);
#endif
}

SCC_END_EXTERN_C

#endif // _SCC_FOUNDATION_UTILITIES_H_

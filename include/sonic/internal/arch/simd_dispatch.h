#pragma once
#include "sonic_cpu_feature.h"

#ifndef SONIC_DYNAMIC_DISPATCH
#define SONIC_STATIC_DISPATCH
#endif

#ifndef SONIC_STRINGIFY
#define SONIC_STRINGIFY(s) SONIC_STRINGIFY2(s)
#define SONIC_STRINGIFY2(s) #s
#endif

#if defined(SONIC_STATIC_DISPATCH)

  #if defined(SONIC_HAVE_AVX512)
    #define DISPATCH(func, ...) func##_avx512(__VA_ARGS__)
    #define INCLUDE_ARCH_FILE(file) SONIC_STRINGIFY(avx512/file)
  #elif defined(SONIC_HAVE_AVX2)
    #define DISPATCH(func, ...) func##_avx2(__VA_ARGS__)
    #define INCLUDE_ARCH_FILE(file) SONIC_STRINGIFY(avx2/file)
  #elif defined(SONIC_HAVE_SSE)
    #define DISPATCH(func, ...) func##_sse(__VA_ARGS__)
    #define INCLUDE_ARCH_FILE(file) SONIC_STRINGIFY(sse/file)
  #endif

  #if defined(SONIC_HAVE_NEON)
    #define DISPATCH(func, ...) func##_neon(__VA_ARGS__)
    #define INCLUDE_ARCH_FILE(file) SONIC_STRINGIFY(neon/file)
  #endif

#endif

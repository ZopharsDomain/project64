#pragma once
#define ASMJIT_EXPORTS

// Only turn-off these warnings when building asmjit itself.
#ifdef _MSC_VER
  #ifndef _CRT_SECURE_NO_DEPRECATE
    #define _CRT_SECURE_NO_DEPRECATE
  #endif
  #ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
  #endif
#endif

// Dependencies only required for asmjit build, but never exposed through public headers.
#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #include <windows.h>
#endif

#include "./api-config.h"

#if !defined(ASMJIT_BUILD_DEBUG) && defined(__GNUC__) && !defined(__clang__)
  #define ASMJIT_FAVOR_SIZE  __attribute__((__optimize__("Os")))
  #define ASMJIT_FAVOR_SPEED __attribute__((__optimize__("O3")))
#elif ASMJIT_CXX_HAS_ATTRIBUTE(__minsize__, 0)
  #define ASMJIT_FAVOR_SIZE __attribute__((__minsize__))
  #define ASMJIT_FAVOR_SPEED
#else
  #define ASMJIT_FAVOR_SIZE
  #define ASMJIT_FAVOR_SPEED
#endif

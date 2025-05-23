#pragma once

#if defined(__amd64__) || defined(_M_X64)
#define USE_ASMJITLITE
#endif

#ifdef USE_ASMJITLITE
#include <AsmJitLite\asmjit.h>
#else
#define ASMJIT_STATIC

#ifdef new
#pragma push_macro("new")
#undef new
#include <asmjit\asmjit.h>
#pragma pop_macro("new")
#else
#include <asmjit\asmjit.h>
#endif
#include <asmjit\a64.h>
#include <asmjit\arm.h>
#include <asmjit\arm\a64assembler.h>
#endif
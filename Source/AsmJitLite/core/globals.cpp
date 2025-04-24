#include "../core/api-build_p.h"
#include "../core/globals.h"
#include <stdio.h>

ASMJIT_BEGIN_NAMESPACE

// DebugUtils - Error As String
// ============================

ASMJIT_FAVOR_SIZE const char* DebugUtils::errorAsString(Error err) noexcept {
  DebugUtils::unused(err);
  static const char noMessage[] = "";
  return noMessage;
}

// DebugUtils - Debug Output
// =========================

ASMJIT_FAVOR_SIZE void DebugUtils::debugOutput(const char* str) noexcept {
#if defined(_WIN32)
  ::OutputDebugStringA(str);
#else
  ::fputs(str, stderr);
#endif
}

// DebugUtils - Fatal Errors
// =========================

ASMJIT_FAVOR_SIZE void DebugUtils::assertionFailed(const char* file, int line, const char* msg) noexcept {
  char str[1024];

  snprintf(str, 1024,
    "[asmjit] Assertion failed at %s (line %d):\n"
    "[asmjit] %s\n", file, line, msg);

  debugOutput(str);
  ::abort();
}

ASMJIT_END_NAMESPACE

#include "../core/api-build_p.h"
#ifndef ASMJIT_NO_LOGGING

#include "../core/logger.h"
#include "../core/string.h"
#include "../core/support.h"

ASMJIT_BEGIN_NAMESPACE

// Logger - Implementation
// =======================

Logger::Logger() noexcept
  : _options() {}
Logger::~Logger() noexcept {}
ASMJIT_END_NAMESPACE

#endif

#pragma once
#include "api-config.h"
#include "../core/string.h"
#include "../core/formatter.h"

#ifndef ASMJIT_NO_LOGGING

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_logging
//! \{

//! Logging interface.
//!
//! This class can be inherited and reimplemented to fit into your own logging needs. When reimplementing a logger
//! use \ref Logger::_log() method to log customize the output.
//!
//! There are two `Logger` implementations offered by AsmJit:
//!   - \ref FileLogger - logs into a `FILE*`.
//!   - \ref StringLogger - concatenates all logs into a \ref String.
class ASMJIT_VIRTAPI Logger {
public:
  ASMJIT_BASE_CLASS(Logger)
  ASMJIT_NONCOPYABLE(Logger)

  //! Format options.
  FormatOptions _options;

  //! \name Construction & Destruction
  //! \{

  //! Creates a `Logger` instance.
  ASMJIT_API Logger() noexcept;
  //! Destroys the `Logger` instance.
  ASMJIT_API virtual ~Logger() noexcept;

  //! \}

  //! \name Format Options
  //! \{

  //! Returns \ref FormatOptions of this logger.
  inline FormatOptions& options() noexcept { return _options; }
  //! Returns formatting flags.
  inline FormatFlags flags() const noexcept { return _options.flags(); }
  //! Tests whether the logger has the given `flag` enabled.
  inline bool hasFlag(FormatFlags flag) const noexcept { return _options.hasFlag(flag); }
  //! Enables the given formatting `flags`.
  inline void addFlags(FormatFlags flags) noexcept { _options.addFlags(flags); }

  //! Returns indentation of a given indentation `group`.
  inline uint32_t indentation(FormatIndentationGroup type) const noexcept { return _options.indentation(type); }
  //! \}

  //! \name Logging Interface
  //! \{

  //! Logs `str` - must be reimplemented.
  //!
  //! The function can accept either a null terminated string if `size` is `SIZE_MAX` or a non-null terminated
  //! string of the given `size`. The function cannot assume that the data is null terminated and must handle
  //! non-null terminated inputs.
  virtual Error _log(const char* data, size_t size) noexcept = 0;

  //! Logs string `str`, which is either null terminated or having size `size`.
  inline Error log(const char* data, size_t size = SIZE_MAX) noexcept { return _log(data, size); }
  //! Logs content of a string `str`.
};

ASMJIT_END_NAMESPACE

#endif

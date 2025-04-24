// This file is part of AsmJit project <https://asmjit.com>
//
// See asmjit.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#ifndef ASMJIT_CORE_FORMATTER_H_INCLUDED
#define ASMJIT_CORE_FORMATTER_H_INCLUDED

#include "../core/globals.h"
#include "../core/inst.h"
#include "../core/string.h"
#include "../core/support.h"

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_logging
//! \{

class BaseEmitter;

//! Format flags used by \ref Logger and \ref FormatOptions.
enum class FormatFlags : uint32_t {
  //! No formatting flags.
  kNone = 0u,

  //! Show also binary form of each logged instruction (Assembler).
  kMachineCode = 0x00000001u,
  //! Show a text explanation of some immediate values.
  kExplainImms = 0x00000002u,
  //! Use hexadecimal notation of immediate values.
  kHexImms = 0x00000004u,
  //! Use hexadecimal notation of addresses and offsets in addresses.
  kHexOffsets = 0x00000008u,
  //! Show casts between virtual register types (Compiler output).
  kRegCasts = 0x00000010u,
  //! Show positions associated with nodes (Compiler output).
  kPositions = 0x00000020u,
  //! Always format a register type (Compiler output).
  kRegType = 0x00000040u
};
ASMJIT_DEFINE_ENUM_FLAGS(FormatFlags)

//! Format indentation group, used by \ref FormatOptions.
enum class FormatIndentationGroup : uint32_t {
  //! Indentation used for instructions and directives.
  kCode = 0u,
  //! Indentation used for labels and function nodes.
  kLabel = 1u,
  //! Indentation used for comments (not inline comments).
  kComment = 2u,

  //! \cond INTERNAL
  //! Reserved for future use.
  kReserved = 3u,
  //! \endcond

  //! Maximum value of `FormatIndentationGroup`.
  kMaxValue = kReserved
};

//! Format padding group, used by \ref FormatOptions.
enum class FormatPaddingGroup : uint32_t {
  //! Describes padding of a regular line, which can represent instruction, data, or assembler directives.
  kRegularLine = 0,
  //! Describes padding of machine code dump that is visible next to the instruction, if enabled.
  kMachineCode = 1,

  //! Maximum value of `FormatPaddingGroup`.
  kMaxValue = kMachineCode
};

//! Formatting options used by \ref Logger and \ref Formatter.
class FormatOptions {
public:
  //! \name Members
  //! \{

  //! Format flags.
  FormatFlags _flags = FormatFlags::kNone;
  //! Indentations for each indentation group.
  Support::Array<uint8_t, uint32_t(FormatIndentationGroup::kMaxValue) + 1> _indentation {};
  //! Paddings for each padding group.
  Support::Array<uint16_t, uint32_t(FormatPaddingGroup::kMaxValue) + 1> _padding {};

  //! \}

  //! \name Accessors
  //! \{

  //! Returns format flags.
  inline FormatFlags flags() const noexcept { return _flags; }
  //! Tests whether the given `flag` is set in format flags.
  inline bool hasFlag(FormatFlags flag) const noexcept { return Support::test(_flags, flag); }
  //! Adds `flags` to format flags.
  inline void addFlags(FormatFlags flags) noexcept { _flags |= flags; }

  //! Returns indentation for the given indentation `group`.
  inline uint8_t indentation(FormatIndentationGroup group) const noexcept { return _indentation[group]; }

  //! Returns pading for the given padding `group`.
  inline size_t padding(FormatPaddingGroup group) const noexcept { return _padding[group]; }
};

namespace Formatter {

#ifndef ASMJIT_NO_LOGGING

//! Appends a formatted label to the output string `sb`.
//!
//! \note Emitter is optional, but it's required to format named labels properly, otherwise the formatted as
//! it is an anonymous label.
ASMJIT_API Error formatLabel(
  String& sb,
  FormatFlags formatFlags,
  const BaseEmitter* emitter,
  uint32_t labelId) noexcept;

#endif

} // {Formatter}

//! \}

ASMJIT_END_NAMESPACE

#endif // ASMJIT_CORE_FORMATTER_H_INCLUDED

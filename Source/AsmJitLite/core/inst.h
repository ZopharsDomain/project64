#pragma once

#include "../core/operand.h"
#include "../core/string.h"
#include "../core/support.h"

ASMJIT_BEGIN_NAMESPACE

//! Instruction options.
//!
//! Instruction options complement instruction identifier and attributes.
enum class InstOptions : uint32_t {
  //! No options.
  kNone = 0,

  //! Used internally by emitters for handling errors and rare cases.
  kReserved = 0x00000001u,

  //! Prevents following a jump during compilation (Compiler).
  kUnfollow = 0x00000002u,

  //! Overwrite the destination operand(s) (Compiler).
  //!
  //! Hint that is important for register liveness analysis. It tells the compiler that the destination operand will
  //! be overwritten now or by adjacent instructions. Compiler knows when a register is completely overwritten by a
  //! single instruction, for example you don't have to mark "movaps" or "pxor x, x", however, if a pair of
  //! instructions is used and the first of them doesn't completely overwrite the content of the destination,
  //! Compiler fails to mark that register as dead.
  //!
  //! X86 Specific
  //! ------------
  //!
  //!   - All instructions that always overwrite at least the size of the register the virtual-register uses, for
  //!     example "mov", "movq", "movaps" don't need the overwrite option to be used - conversion, shuffle, and
  //!     other miscellaneous instructions included.
  //!
  //!   - All instructions that clear the destination register if all operands are the same, for example "xor x, x",
  //!     "pcmpeqb x x", etc...
  //!
  //!   - Consecutive instructions that partially overwrite the variable until there is no old content require
  //!     `BaseCompiler::overwrite()` to be used. Some examples (not always the best use cases thought):
  //!
  //!     - `movlps xmm0, ?` followed by `movhps xmm0, ?` and vice versa
  //!     - `movlpd xmm0, ?` followed by `movhpd xmm0, ?` and vice versa
  //!     - `mov al, ?` followed by `and ax, 0xFF`
  //!     - `mov al, ?` followed by `mov ah, al`
  //!     - `pinsrq xmm0, ?, 0` followed by `pinsrq xmm0, ?, 1`
  //!
  //!   - If the allocated virtual register is used temporarily for scalar operations. For example if you allocate a
  //!     full vector like `x86::Compiler::newXmm()` and then use that vector for scalar operations you should use
  //!     `overwrite()` directive:
  //!
  //!     - `sqrtss x, y` - only LO element of `x` is changed, if you don't
  //!       use HI elements, use `compiler.overwrite().sqrtss(x, y)`.
  kOverwrite = 0x00000004u,

  //! Emit short-form of the instruction.
  kShortForm = 0x00000010u,
  //! Emit long-form of the instruction.
  kLongForm = 0x00000020u,

  //! Conditional jump is likely to be taken.
  kTaken = 0x00000040u,
  //! Conditional jump is unlikely to be taken.
  kNotTaken = 0x00000080u,

  // X86 & X64 Options
  // -----------------

  //! Use ModMR instead of ModRM if applicable.
  kX86_ModMR = 0x00000100u,
  //! Use ModRM instead of ModMR if applicable.
  kX86_ModRM = 0x00000200u,
  //! Use 3-byte VEX prefix if possible (AVX) (must be 0x00000400).
  kX86_Vex3 = 0x00000400u,
  //! Use VEX prefix when both VEX|EVEX prefixes are available (HINT: AVX_VNNI).
  kX86_Vex = 0x00000800u,
  //! Use 4-byte EVEX prefix if possible (AVX-512) (must be 0x00001000).
  kX86_Evex = 0x00001000u,

  //! LOCK prefix (lock-enabled instructions only).
  kX86_Lock = 0x00002000u,
  //! REP prefix (string instructions only).
  kX86_Rep = 0x00004000u,
  //! REPNE prefix (string instructions only).
  kX86_Repne = 0x00008000u,

  //! XACQUIRE prefix (only allowed instructions).
  kX86_XAcquire = 0x00010000u,
  //! XRELEASE prefix (only allowed instructions).
  kX86_XRelease = 0x00020000u,

  //! AVX-512: embedded-rounding {er} and implicit {sae}.
  kX86_ER = 0x00040000u,
  //! AVX-512: suppress-all-exceptions {sae}.
  kX86_SAE = 0x00080000u,
  //! AVX-512: round-to-nearest (even) {rn-sae} (bits 00).
  kX86_RN_SAE = 0x00000000u,
  //! AVX-512: round-down (toward -inf) {rd-sae} (bits 01).
  kX86_RD_SAE = 0x00200000u,
  //! AVX-512: round-up (toward +inf) {ru-sae} (bits 10).
  kX86_RU_SAE = 0x00400000u,
  //! AVX-512: round-toward-zero (truncate) {rz-sae} (bits 11).
  kX86_RZ_SAE = 0x00600000u,
  //! AVX-512: Use zeroing {k}{z} instead of merging {k}.
  kX86_ZMask = 0x00800000u,

  //! AVX-512: Mask to get embedded rounding bits (2 bits).
  kX86_ERMask = kX86_RZ_SAE,
  //! AVX-512: Mask of all possible AVX-512 options except EVEX prefix flag.
  kX86_AVX512Mask = 0x00FC0000u,

  //! Force REX.B and/or VEX.B field (X64 only).
  kX86_OpCodeB = 0x01000000u,
  //! Force REX.X and/or VEX.X field (X64 only).
  kX86_OpCodeX = 0x02000000u,
  //! Force REX.R and/or VEX.R field (X64 only).
  kX86_OpCodeR = 0x04000000u,
  //! Force REX.W and/or VEX.W field (X64 only).
  kX86_OpCodeW = 0x08000000u,
  //! Force REX prefix (X64 only).
  kX86_Rex = 0x40000000u,
  //! Invalid REX prefix (set by X86 or when AH|BH|CH|DH regs are used on X64).
  kX86_InvalidRex = 0x80000000u
};
ASMJIT_DEFINE_ENUM_FLAGS(InstOptions)

ASMJIT_END_NAMESPACE

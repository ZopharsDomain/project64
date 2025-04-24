#include "../x86/x86assembler.h"
#include "../x86/x86emithelper_p.h"

ASMJIT_BEGIN_SUB_NAMESPACE(x86)


// x86::Assembler - X86MemInfo | X86VEXPrefix | X86LLByRegType | X86CDisp8Table
// ============================================================================

//! Memory operand's info bits.
//!
//! A lookup table that contains various information based on the BASE and INDEX information of a memory operand. This
//! is much better and safer than playing with IFs in the code and can check for errors must faster and better.
enum X86MemInfo_Enum {
  kX86MemInfo_0         = 0x00,

  kX86MemInfo_BaseGp    = 0x01, //!< Has BASE reg, REX.B can be 1, compatible with REX.B byte.
  kX86MemInfo_Index     = 0x02, //!< Has INDEX reg, REX.X can be 1, compatible with REX.X byte.

  kX86MemInfo_BaseLabel = 0x10, //!< Base is Label.
  kX86MemInfo_BaseRip   = 0x20, //!< Base is RIP.

  kX86MemInfo_67H_X86   = 0x40, //!< Address-size override in 32-bit mode.
  kX86MemInfo_67H_X64   = 0x80, //!< Address-size override in 64-bit mode.
  kX86MemInfo_67H_Mask  = 0xC0  //!< Contains all address-size override bits.
};


// x86::Assembler - Construction & Destruction
// ===========================================

Assembler::Assembler(CodeHolder * code) noexcept :
    EmitterImplicitT<Assembler>(this)
{
  _archMask = (uint64_t(1) << uint32_t(Arch::kX86)) |
              (uint64_t(1) << uint32_t(Arch::kX64)) ;

  if (code)
    code->attach(this);
}
Assembler::~Assembler() noexcept {}

// x86::Assembler - Emit (Low-Level)
// =================================

Error Assembler::onAttach(CodeHolder* code) noexcept {
  Arch arch = code->arch();
  ASMJIT_PROPAGATE(Base::onAttach(code));

  if (Environment::is32Bit(arch)) {
    // 32 bit architecture - X86.
    _forcedInstOptions |= InstOptions::kX86_InvalidRex;
    _setAddressOverrideMask(kX86MemInfo_67H_X86);
  }
  else {
    // 64 bit architecture - X64.
    _forcedInstOptions &= ~InstOptions::kX86_InvalidRex;
    _setAddressOverrideMask(kX86MemInfo_67H_X64);
  }

  return kErrorOk;
}

Error Assembler::onDetach(CodeHolder* code) noexcept {
  _forcedInstOptions &= ~InstOptions::kX86_InvalidRex;
  _setAddressOverrideMask(0);
  return Base::onDetach(code);
}

ASMJIT_END_SUB_NAMESPACE
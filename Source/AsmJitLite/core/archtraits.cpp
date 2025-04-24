#include "../core/api-build_p.h"
#include "../core/archtraits.h"
#include "../core/misc_p.h"

#if !defined(ASMJIT_NO_X86)
  #include "../x86/x86archtraits_p.h"
#endif

#if !defined(ASMJIT_NO_AARCH64)
  #include "../arm/a64archtraits_p.h"
#endif

ASMJIT_BEGIN_NAMESPACE

static const constexpr ArchTraits noArchTraits = {
  // SP/FP/LR/PC.
  0xFF, 0xFF, 0xFF, 0xFF,

  // Reserved,
  { 0, 0, 0 },

  // HW stack alignment.
  0,

  // Min/Max stack offset.
  0, 0,

  // ISA features [Gp, Vec, Other0, Other1].
  {{
    InstHints::kNoHints,
    InstHints::kNoHints,
    InstHints::kNoHints,
    InstHints::kNoHints
  }},

  // RegTypeToSignature.
  #define V(index) OperandSignature{0}
  {{ ASMJIT_LOOKUP_TABLE_32(V, 0) }},
  #undef V

  // RegTypeToTypeId.
  #define V(index) TypeId::kVoid
  {{ ASMJIT_LOOKUP_TABLE_32(V, 0) }},
  #undef V

  // TypeIdToRegType.
  #define V(index) RegType::kNone
  {{ ASMJIT_LOOKUP_TABLE_32(V, 0) }},
  #undef V

  // Word names of 8-bit, 16-bit, 32-bit, and 64-bit quantities.
  {
    ArchTypeNameId::kByte,
    ArchTypeNameId::kHalf,
    ArchTypeNameId::kWord,
    ArchTypeNameId::kQuad
  }
};

ASMJIT_VARAPI const ArchTraits _archTraits[uint32_t(Arch::kMaxValue) + 1] = {
  // No architecture.
  noArchTraits,

  // X86/X86 architectures.
#if !defined(ASMJIT_NO_X86)
  x86::x86ArchTraits,
  x86::x64ArchTraits,
#else
  noArchTraits,
  noArchTraits,
#endif

  // RISCV32/RISCV64 architectures.
  noArchTraits,
  noArchTraits,

  // ARM architecture
  noArchTraits,

  // AArch64 architecture.
#if !defined(ASMJIT_NO_AARCH64)
  a64::a64ArchTraits,
#else
  noArchTraits,
#endif

  // ARM/Thumb architecture.
  noArchTraits,

  // Reserved.
  noArchTraits,

  // MIPS32/MIPS64
  noArchTraits,
  noArchTraits
};
ASMJIT_END_NAMESPACE

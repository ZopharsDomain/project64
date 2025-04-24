#pragma once
#include "../core/archtraits.h"
#include "../core/api-config.h"
#include "../core/globals.h"

ASMJIT_BEGIN_SUB_NAMESPACE(x86)

class Reg;
class Gp;
class GpbLo;
class GpbHi;
class Gpw;
class Gpd;
class Gpq;
class St;

template<RegType kRegType>
struct RegTraits : public BaseRegTraits {};

//! \cond
// <--------------------+-----+-------------------------+------------------------+---+---+------------------+
//                      | Reg |        Reg-Type         |        Reg-Group       |Sz |Cnt|      TypeId      |
// <--------------------+-----+-------------------------+------------------------+---+---+------------------+
ASMJIT_DEFINE_REG_TRAITS(GpbLo, RegType::kX86_GpbLo     , RegGroup::kGp          , 1 , 16, TypeId::kInt8    );
ASMJIT_DEFINE_REG_TRAITS(GpbHi, RegType::kX86_GpbHi     , RegGroup::kGp          , 1 , 4 , TypeId::kInt8    );
ASMJIT_DEFINE_REG_TRAITS(Gpw  , RegType::kX86_Gpw       , RegGroup::kGp          , 2 , 16, TypeId::kInt16   );
ASMJIT_DEFINE_REG_TRAITS(Gpd  , RegType::kX86_Gpd       , RegGroup::kGp          , 4 , 16, TypeId::kInt32   );
ASMJIT_DEFINE_REG_TRAITS(Gpq  , RegType::kX86_Gpq       , RegGroup::kGp          , 8 , 16, TypeId::kInt64   );

class Reg : public BaseReg {
public:
  ASMJIT_DEFINE_ABSTRACT_REG(Reg, BaseReg)

  //! Tests whether the register is a GPB register (8-bit).
  inline constexpr bool isGpb() const noexcept { return size() == 1; }
  //! Tests whether the register is a low GPB register (8-bit).
  inline constexpr bool isGpbLo() const noexcept { return hasBaseSignature(RegTraits<RegType::kX86_GpbLo>::kSignature); }

  static inline OperandSignature signatureOf(RegType type) noexcept { return ArchTraits::byArch(Arch::kX86).regTypeToSignature(type); }
};

//! General purpose register (X86).
class Gp : public Reg {
public:
  ASMJIT_DEFINE_ABSTRACT_REG(Gp, Reg)
  enum Id : uint32_t {
    kIdAx  = 0,  //!< Physical id of AL|AH|AX|EAX|RAX registers.
    kIdCx  = 1,  //!< Physical id of CL|CH|CX|ECX|RCX registers.
    kIdDx  = 2,  //!< Physical id of DL|DH|DX|EDX|RDX registers.
    kIdBx  = 3,  //!< Physical id of BL|BH|BX|EBX|RBX registers.
    kIdSp  = 4,  //!< Physical id of SPL|SP|ESP|RSP registers.
    kIdBp  = 5,  //!< Physical id of BPL|BP|EBP|RBP registers.
    kIdSi  = 6,  //!< Physical id of SIL|SI|ESI|RSI registers.
    kIdDi  = 7,  //!< Physical id of DIL|DI|EDI|RDI registers.
    kIdR8  = 8,  //!< Physical id of R8B|R8W|R8D|R8 registers (64-bit only).
    kIdR9  = 9,  //!< Physical id of R9B|R9W|R9D|R9 registers (64-bit only).
    kIdR10 = 10, //!< Physical id of R10B|R10W|R10D|R10 registers (64-bit only).
    kIdR11 = 11, //!< Physical id of R11B|R11W|R11D|R11 registers (64-bit only).
    kIdR12 = 12, //!< Physical id of R12B|R12W|R12D|R12 registers (64-bit only).
    kIdR13 = 13, //!< Physical id of R13B|R13W|R13D|R13 registers (64-bit only).
    kIdR14 = 14, //!< Physical id of R14B|R14W|R14D|R14 registers (64-bit only).
    kIdR15 = 15  //!< Physical id of R15B|R15W|R15D|R15 registers (64-bit only).
  };

  //! Casts this register to 8-bit (LO) part.
  inline GpbLo r8() const noexcept;
  //! Casts this register to 8-bit (LO) part.
  inline GpbLo r8Lo() const noexcept;
  //! Casts this register to 16-bit.
  inline Gpw r16() const noexcept;
};

//! GPB low or high register (X86).
class Gpb : public Gp { ASMJIT_DEFINE_ABSTRACT_REG(Gpb, Gp) };
//! GPB low register (X86).
class GpbLo : public Gpb { ASMJIT_DEFINE_FINAL_REG(GpbLo, Gpb, RegTraits<RegType::kX86_GpbLo>) };
//! GPB high register (X86).
class GpbHi : public Gpb { ASMJIT_DEFINE_FINAL_REG(GpbHi, Gpb, RegTraits<RegType::kX86_GpbHi>) };
//! GPW register (X86).
class Gpw : public Gp { ASMJIT_DEFINE_FINAL_REG(Gpw, Gp, RegTraits<RegType::kX86_Gpw>) };
//! GPD register (X86).
class Gpd : public Gp { ASMJIT_DEFINE_FINAL_REG(Gpd, Gp, RegTraits<RegType::kX86_Gpd>) };
//! GPQ register (X86_64).
class Gpq : public Gp { ASMJIT_DEFINE_FINAL_REG(Gpq, Gp, RegTraits<RegType::kX86_Gpq>) };

class St : public Reg { ASMJIT_DEFINE_FINAL_REG(St, Reg, RegTraits<RegType::kX86_St>) };

//! \cond
inline GpbLo Gp::r8() const noexcept { return GpbLo(id()); }
inline GpbLo Gp::r8Lo() const noexcept { return GpbLo(id()); }
inline Gpw Gp::r16() const noexcept { return Gpw(id()); }
//! \endcond
static constexpr GpbLo al = GpbLo(Gp::kIdAx);
static constexpr GpbLo bl = GpbLo(Gp::kIdBx);
static constexpr GpbLo cl = GpbLo(Gp::kIdCx);
static constexpr GpbLo dl = GpbLo(Gp::kIdDx);

static constexpr GpbHi ah = GpbHi(Gp::kIdAx);
static constexpr GpbHi bh = GpbHi(Gp::kIdBx);
static constexpr GpbHi ch = GpbHi(Gp::kIdCx);
static constexpr GpbHi dh = GpbHi(Gp::kIdDx);

static constexpr Gpw ax = Gpw(Gp::kIdAx);
static constexpr Gpw bx = Gpw(Gp::kIdBx);
static constexpr Gpw cx = Gpw(Gp::kIdCx);
static constexpr Gpw dx = Gpw(Gp::kIdDx);
static constexpr Gpw sp = Gpw(Gp::kIdSp);
static constexpr Gpw bp = Gpw(Gp::kIdBp);
static constexpr Gpw si = Gpw(Gp::kIdSi);
static constexpr Gpw di = Gpw(Gp::kIdDi);

static constexpr Gpd eax = Gpd(Gp::kIdAx);
static constexpr Gpd ebx = Gpd(Gp::kIdBx);
static constexpr Gpd ecx = Gpd(Gp::kIdCx);
static constexpr Gpd edx = Gpd(Gp::kIdDx);
static constexpr Gpd esp = Gpd(Gp::kIdSp);
static constexpr Gpd ebp = Gpd(Gp::kIdBp);
static constexpr Gpd esi = Gpd(Gp::kIdSi);
static constexpr Gpd edi = Gpd(Gp::kIdDi);
static constexpr Gpd r8d = Gpd(Gp::kIdR8);
static constexpr Gpd r9d = Gpd(Gp::kIdR9);
static constexpr Gpd r10d = Gpd(Gp::kIdR10);
static constexpr Gpd r11d = Gpd(Gp::kIdR11);
static constexpr Gpd r12d = Gpd(Gp::kIdR12);
static constexpr Gpd r13d = Gpd(Gp::kIdR13);
static constexpr Gpd r14d = Gpd(Gp::kIdR14);
static constexpr Gpd r15d = Gpd(Gp::kIdR15);

static constexpr Gpq rax = Gpq(Gp::kIdAx);
static constexpr Gpq rbx = Gpq(Gp::kIdBx);
static constexpr Gpq rcx = Gpq(Gp::kIdCx);
static constexpr Gpq rdx = Gpq(Gp::kIdDx);
static constexpr Gpq rsp = Gpq(Gp::kIdSp);
static constexpr Gpq rbp = Gpq(Gp::kIdBp);
static constexpr Gpq rsi = Gpq(Gp::kIdSi);
static constexpr Gpq rdi = Gpq(Gp::kIdDi);
static constexpr Gpq r8 = Gpq(Gp::kIdR8);
static constexpr Gpq r9 = Gpq(Gp::kIdR9);
static constexpr Gpq r10 = Gpq(Gp::kIdR10);
static constexpr Gpq r11 = Gpq(Gp::kIdR11);
static constexpr Gpq r12 = Gpq(Gp::kIdR12);
static constexpr Gpq r13 = Gpq(Gp::kIdR13);
static constexpr Gpq r14 = Gpq(Gp::kIdR14);
static constexpr Gpq r15 = Gpq(Gp::kIdR15);

static constexpr St st0 = St(0);
static constexpr St st1 = St(1);
static constexpr St st2 = St(2);
static constexpr St st3 = St(3);
static constexpr St st4 = St(4);
static constexpr St st5 = St(5);
static constexpr St st6 = St(6);
static constexpr St st7 = St(7);


//! Memory operand specific to X86 and X86_64 architecture.
class Mem : public BaseMem {
public:
  //! \name Constants
  //! \{

  //! Additional bits of operand's signature used by `x86::Mem`.
  enum AdditionalBits : uint32_t {
    // Memory shift amount (2 bits).
    // |........|......XX|........|........|
    kSignatureMemShiftValueShift = 16,
    kSignatureMemShiftValueMask = 0x03u << kSignatureMemShiftValueShift,
  };
  //! \}

  //! \name Construction & Destruction
  //! \{

  //! Creates a default `Mem` operand that points to [0].
  inline constexpr Mem() noexcept
    : BaseMem() {}

  inline constexpr Mem(const Mem& other) noexcept
    : BaseMem(other) {}

  inline explicit Mem(Globals::NoInit_) noexcept
    : BaseMem(Globals::NoInit) {}

  inline constexpr Mem(const Signature& signature, uint32_t baseId, uint32_t indexId, int32_t offset) noexcept
    : BaseMem(signature, baseId, indexId, offset) {}

  inline constexpr Mem(const Label& base, int32_t off, uint32_t size = 0, Signature signature = OperandSignature{0}) noexcept
    : BaseMem(Signature::fromOpType(OperandType::kMem) |
              Signature::fromMemBaseType(RegType::kLabelTag) |
              Signature::fromSize(size) |
              signature, base.id(), 0, off) {}

  inline constexpr Mem(const BaseReg& base, int32_t off, uint32_t size = 0, Signature signature = OperandSignature{0}) noexcept
    : BaseMem(Signature::fromOpType(OperandType::kMem) |
              Signature::fromMemBaseType(base.type()) |
              Signature::fromSize(size) |
              signature, base.id(), 0, off) {}

  inline constexpr Mem(const BaseReg& base, const BaseReg& index, uint32_t shift, int32_t off, uint32_t size = 0, Signature signature = OperandSignature{0}) noexcept
    : BaseMem(Signature::fromOpType(OperandType::kMem) |
              Signature::fromMemBaseType(base.type()) |
              Signature::fromMemIndexType(index.type()) |
              Signature::fromValue<kSignatureMemShiftValueMask>(shift) |
              Signature::fromSize(size) |
              signature, base.id(), index.id(), off) {}

  inline constexpr explicit Mem(uint64_t base, uint32_t size = 0, Signature signature = OperandSignature{0}) noexcept
    : BaseMem(Signature::fromOpType(OperandType::kMem) |
              Signature::fromSize(size) |
              signature, uint32_t(base >> 32), 0, int32_t(uint32_t(base & 0xFFFFFFFFu))) {}

  inline constexpr Mem(uint64_t base, const BaseReg& index, uint32_t shift = 0, uint32_t size = 0, Signature signature = OperandSignature{0}) noexcept
    : BaseMem(Signature::fromOpType(OperandType::kMem) |
              Signature::fromMemIndexType(index.type()) |
              Signature::fromValue<kSignatureMemShiftValueMask>(shift) |
              Signature::fromSize(size) |
              signature, uint32_t(base >> 32), index.id(), int32_t(uint32_t(base & 0xFFFFFFFFu))) {}

  //! \name Shift
  //! \{

  //! Tests whether the memory operand has shift (aka scale) value.
  inline constexpr bool hasShift() const noexcept { return _signature.hasField<kSignatureMemShiftValueMask>(); }
  //! Returns the memory operand's shift (aka scale) value.
  inline constexpr uint32_t shift() const noexcept { return _signature.getField<kSignatureMemShiftValueMask>(); }

  //! \}
};

//! Creates `[base.reg + offset]` memory operand.
static inline constexpr Mem ptr(const Gp& base, int32_t offset = 0, uint32_t size = 0) noexcept {
  return Mem(base, offset, size);
}
//! Creates `[base.reg + (index << shift) + offset]` memory operand (scalar index).
static inline constexpr Mem ptr(const Gp& base, const Gp& index, uint32_t shift = 0, int32_t offset = 0, uint32_t size = 0) noexcept {
  return Mem(base, index, shift, offset, size);
}

static inline constexpr Mem ptr(uint64_t base, uint32_t size = 0) noexcept {
  return Mem(base, size);
}
static inline constexpr Mem ptr(uint64_t base, const Reg& index, uint32_t shift = 0, uint32_t size = 0) noexcept {
  return Mem(base, index, shift, size);
}

#define ASMJIT_MEM_PTR(FUNC, SIZE)                                                    \
  static constexpr Mem FUNC(const Gp& base, int32_t offset = 0) noexcept {            \
    return Mem(base, offset, SIZE);                                                   \
  }                                                                                   \
  static constexpr Mem FUNC(const Gp& base, const Gp& index, uint32_t shift = 0, int32_t offset = 0) noexcept { \
    return Mem(base, index, shift, offset, SIZE);                                     \
  }                                                                                   \
  static constexpr Mem FUNC(uint64_t base) noexcept {                                 \
    return Mem(base, SIZE);                                                           \
  }                                                                                   \
  static constexpr Mem FUNC(uint64_t base, const Gp& index, uint32_t shift = 0) noexcept { \
    return Mem(base, index, shift, SIZE);                                             \
  }                                                                                   \

ASMJIT_MEM_PTR(byte_ptr, 1)
ASMJIT_MEM_PTR(word_ptr, 2)
ASMJIT_MEM_PTR(dword_ptr, 4)
ASMJIT_MEM_PTR(qword_ptr, 8)

ASMJIT_END_SUB_NAMESPACE
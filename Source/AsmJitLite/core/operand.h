#pragma once
#include "globals.h"
#include "../core/support.h"
#include "../core/type.h"

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_assembler
//! \{

//! Operand type used by \ref Operand_.
enum class OperandType : uint32_t {
  //! Not an operand or not initialized.
  kNone = 0,
  //! Operand is a register.
  kReg = 1,
  //! Operand is a memory.
  kMem = 2,
  //! Operand is an immediate value.
  kImm = 3,
  //! Operand is a label.
  kLabel = 4,

  //! Maximum value of `OperandType`.
  kMaxValue = kLabel
};

//! Register type.
//!
//! Provides a unique type that can be used to identify a register or its view.
enum class RegType : uint8_t {
  //! No register - unused, invalid, multiple meanings.
  kNone = 0,

  //! This is not a register type. This value is reserved for a \ref Label that used in \ref BaseMem as a base.
  //!
  //! Label tag is used as a sub-type, forming a unique signature across all operand types as 0x1 is never associated
  //! with any register type. This means that a memory operand's BASE register can be constructed from virtually any
  //! operand (register vs. label) by just assigning its type (register type or label-tag) and operand id.
  kLabelTag = 1,

  //! Universal type describing program counter (PC) or instruction pointer (IP) register, if the target architecture
  //! actually exposes it as a separate register type, which most modern targets do.
  kPC = 2,

  //! 8-bit low general purpose register (X86).
  kGp8Lo = 3,
  //! 8-bit high general purpose register (X86).
  kGp8Hi = 4,
  //! 16-bit general purpose register (X86).
  kGp16 = 5,
  //! 32-bit general purpose register (X86|ARM).
  kGp32 = 6,
  //! 64-bit general purpose register (X86|ARM).
  kGp64 = 7,
  //! 8-bit view of a vector register (ARM).
  kVec8 = 8,
  //! 16-bit view of a vector register (ARM).
  kVec16 = 9,
  //! 32-bit view of a vector register (ARM).
  kVec32 = 10,
  //! 64-bit view of a vector register (ARM).
  //!
  //! \note This is never used for MMX registers on X86, MMX registers have its own category.
  kVec64 = 11,
  //! 128-bit view of a vector register (X86|ARM).
  kVec128 = 12,
  //! 256-bit view of a vector register (X86).
  kVec256 = 13,
  //! 512-bit view of a vector register (X86).
  kVec512 = 14,
  //! 1024-bit view of a vector register (future).
  kVec1024 = 15,
  //! View of a vector register, which width is implementation specific (AArch64).
  kVecNLen = 16,

  //! Mask register (X86).
  kMask = 17,

  //! Start of architecture dependent register types.
  kExtra = 18,

  // X86 Specific Register Types
  // ---------------------------

  // X86 Specific Register Types
  // ===========================

  //! Instruction pointer (RIP), only addressable in \ref x86::Mem in 64-bit targets.
  kX86_Rip = kPC,
  //! Low GPB register (AL, BL, CL, DL, ...).
  kX86_GpbLo = kGp8Lo,
  //! High GPB register (AH, BH, CH, DH only).
  kX86_GpbHi = kGp8Hi,
  //! GPW register.
  kX86_Gpw = kGp16,
  //! GPD register.
  kX86_Gpd = kGp32,
  //! GPQ register (64-bit).
  kX86_Gpq = kGp64,
  //! XMM register (SSE+).
  kX86_Xmm = kVec128,
  //! YMM register (AVX+).
  kX86_Ymm = kVec256,
  //! ZMM register (AVX512+).
  kX86_Zmm = kVec512,
  //! K register (AVX512+).
  kX86_KReg = kMask,
  //! MMX register.
  kX86_Mm = kExtra + 0,
  //! Segment register (None, ES, CS, SS, DS, FS, GS).
  kX86_SReg = kExtra + 1,
  //! Control register (CR).
  kX86_CReg = kExtra + 2,
  //! Debug register (DR).
  kX86_DReg = kExtra + 3,
  //! FPU (x87) register.
  kX86_St = kExtra + 4,
  //! Bound register (BND).
  kX86_Bnd = kExtra + 5,
  //! TMM register (AMX_TILE)
  kX86_Tmm = kExtra + 6,

  // ARM Specific Register Types
  // ===========================

  //! Program pointer (PC) register (AArch64).
  kARM_PC = kPC,
  //! 32-bit general purpose register (R or W).
  kARM_GpW = kGp32,
  //! 64-bit general purpose register (X).
  kARM_GpX = kGp64,
  //! 8-bit view of VFP/ASIMD register (B).
  kARM_VecB = kVec8,
  //! 16-bit view of VFP/ASIMD register (H).
  kARM_VecH = kVec16,
  //! 32-bit view of VFP/ASIMD register (S).
  kARM_VecS = kVec32,
  //! 64-bit view of VFP/ASIMD register (D).
  kARM_VecD = kVec64,
  //! 128-bit view of VFP/ASIMD register (Q|V).
  kARM_VecV = kVec128,

  //! Maximum value of `RegType`.
  kMaxValue = 31
};

enum class RegGroup : uint8_t {
  //! General purpose register group compatible with all backends.
  kGp = 0,
  //! Vector register group compatible with all backends.
  //!
  //! Describes X86 XMM|YMM|ZMM registers ARM/AArch64 V registers.
  kVec = 1,

  //! Extra virtual group #2 that can be used by Compiler for register allocation.
  kExtraVirt2 = 2,
  //! Extra virtual group #3 that can be used by Compiler for register allocation.
  kExtraVirt3 = 3,

  //! Program counter group.
  kPC = 4,

  //! Extra non-virtual group that can be used by registers not managed by Compiler.
  kExtraNonVirt = 5,

  // X86 Specific Register Groups
  // ----------------------------

  //! K register group (KReg) - maps to \ref RegGroup::kExtraVirt2 (X86, X86_64).
  kX86_K = kExtraVirt2,
  //! MMX register group (MM) - maps to \ref RegGroup::kExtraVirt3 (X86, X86_64).
  kX86_MM = kExtraVirt3,

  //! Instruction pointer (X86, X86_64).
  kX86_Rip = kPC,
  //! Segment register group (X86, X86_64).
  kX86_SReg = kExtraNonVirt + 0,
  //! CR register group (X86, X86_64).
  kX86_CReg = kExtraNonVirt + 1,
  //! DR register group (X86, X86_64).
  kX86_DReg = kExtraNonVirt + 2,
  //! FPU register group (X86, X86_64).
  kX86_St = kExtraNonVirt + 3,
  //! BND register group (X86, X86_64).
  kX86_Bnd = kExtraNonVirt + 4,
  //! TMM register group (X86, X86_64).
  kX86_Tmm = kExtraNonVirt + 5,

  //! First group - only used in loops.
  k0 = 0,
  //! Last value of a virtual register that is managed by \ref BaseCompiler.
  kMaxVirt = Globals::kNumVirtGroups - 1,
  //! Maximum value of `RegGroup`.
  kMaxValue = 15
};


//! Operand signature is a 32-bit number describing \ref Operand and some of its payload.
//!
//! In AsmJit operand signature is used to store additional payload of register, memory, and immediate operands.
//! In practice the biggest pressure on OperandSignature is from \ref BaseMem and architecture specific memory
//! operands that need to store additional payload that cannot be stored elsewhere as values of all other members
//! are fully specified by \ref BaseMem.
struct OperandSignature {
  //! \name Constants
  //! \{

  enum : uint32_t {
    // Operand type (3 least significant bits).
    // |........|........|........|.....XXX|
    kOpTypeShift = 0,
    kOpTypeMask = 0x07u << kOpTypeShift,

    // Register type (5 bits).
    // |........|........|........|XXXXX...|
    kRegTypeShift = 3,
    kRegTypeMask = 0x1Fu << kRegTypeShift,

    // Register group (4 bits).
    // |........|........|....XXXX|........|
    kRegGroupShift = 8,
    kRegGroupMask = 0x0Fu << kRegGroupShift,

    // Memory base type (5 bits).
    // |........|........|........|XXXXX...|
    kMemBaseTypeShift = 3,
    kMemBaseTypeMask = 0x1Fu << kMemBaseTypeShift,

    // Memory index type (5 bits).
    // |........|........|...XXXXX|........|
    kMemIndexTypeShift = 8,
    kMemIndexTypeMask = 0x1Fu << kMemIndexTypeShift,

    // Memory base+index combined (10 bits).
    // |........|........|...XXXXX|XXXXX...|
    kMemBaseIndexShift = 3,
    kMemBaseIndexMask = 0x3FFu << kMemBaseIndexShift,

    // This memory operand represents a home-slot or stack (Compiler) (1 bit).
    // |........|........|..X.....|........|
    kMemRegHomeShift = 13,
    kMemRegHomeFlag = 0x01u << kMemRegHomeShift,

    // Immediate type (1 bit).
    // |........|........|........|....X...|
    kImmTypeShift = 3,
    kImmTypeMask = 0x01u << kImmTypeShift,

    // Predicate used by either registers or immediate values (4 bits).
    // |........|XXXX....|........|........|
    kPredicateShift = 20,
    kPredicateMask = 0x0Fu << kPredicateShift,

    // Operand size (8 most significant bits).
    // |XXXXXXXX|........|........|........|
    kSizeShift = 24,
    kSizeMask = 0xFFu << kSizeShift
  };

  //! \}

  //! \name Members
  //! \{

  uint32_t _bits;

  //! \}

  //! \name Overloaded Operators
  //!
  //! Overloaded operators make `OperandSignature` behave like regular integer.
  //!
  //! \{

  inline constexpr bool operator!() const noexcept { return _bits != 0; }
  inline constexpr explicit operator bool() const noexcept { return _bits != 0; }

  inline OperandSignature& operator|=(uint32_t x) noexcept { _bits |= x; return *this; }
  inline OperandSignature& operator&=(uint32_t x) noexcept { _bits &= x; return *this; }
  inline OperandSignature& operator^=(uint32_t x) noexcept { _bits ^= x; return *this; }

  inline OperandSignature& operator|=(const OperandSignature& other) noexcept { return operator|=(other._bits); }
  inline OperandSignature& operator&=(const OperandSignature& other) noexcept { return operator&=(other._bits); }
  inline OperandSignature& operator^=(const OperandSignature& other) noexcept { return operator^=(other._bits); }

  inline constexpr OperandSignature operator~() const noexcept { return OperandSignature{~_bits}; }

  inline constexpr OperandSignature operator|(uint32_t x) const noexcept { return OperandSignature{_bits | x}; }
  inline constexpr OperandSignature operator&(uint32_t x) const noexcept { return OperandSignature{_bits & x}; }
  inline constexpr OperandSignature operator^(uint32_t x) const noexcept { return OperandSignature{_bits ^ x}; }

  inline constexpr OperandSignature operator|(const OperandSignature& other) const noexcept { return OperandSignature{_bits | other._bits}; }
  inline constexpr OperandSignature operator&(const OperandSignature& other) const noexcept { return OperandSignature{_bits & other._bits}; }
  inline constexpr OperandSignature operator^(const OperandSignature& other) const noexcept { return OperandSignature{_bits ^ other._bits}; }

  inline constexpr bool operator==(uint32_t x) const noexcept { return _bits == x; }
  inline constexpr bool operator!=(uint32_t x) const noexcept { return _bits != x; }

  inline constexpr bool operator==(const OperandSignature& other) const noexcept { return _bits == other._bits; }
  inline constexpr bool operator!=(const OperandSignature& other) const noexcept { return _bits != other._bits; }

  //! \}

  //! \name Accessors
  //! \{

  inline constexpr uint32_t bits() const noexcept { return _bits; }

  template<uint32_t kFieldMask, uint32_t kFieldShift = Support::ConstCTZ<kFieldMask>::value>
  inline constexpr bool hasField() const noexcept {
    return (_bits & kFieldMask) != 0;
  }

  template<uint32_t kFieldMask, uint32_t kFieldShift = Support::ConstCTZ<kFieldMask>::value>
  inline constexpr bool hasField(uint32_t value) const noexcept {
    return (_bits & kFieldMask) != value << kFieldShift;
  }

  template<uint32_t kFieldMask, uint32_t kFieldShift = Support::ConstCTZ<kFieldMask>::value>
  inline constexpr uint32_t getField() const noexcept {
    return (_bits >> kFieldShift) & (kFieldMask >> kFieldShift);
  }

  inline constexpr bool isValid() const noexcept { return _bits != 0; }

  inline constexpr OperandType opType() const noexcept { return (OperandType)getField<kOpTypeMask>(); }

  inline constexpr RegType regType() const noexcept { return (RegType)getField<kRegTypeMask>(); }
  inline constexpr RegGroup regGroup() const noexcept { return (RegGroup)getField<kRegGroupMask>(); }
  inline constexpr uint32_t size() const noexcept { return getField<kSizeMask>(); }

  //! \}

  //! \name Static Constructors
  //! \{

  static inline constexpr OperandSignature fromBits(uint32_t bits) noexcept {
    return OperandSignature{bits};
  }

  template<uint32_t kFieldMask, typename T>
  static inline constexpr OperandSignature fromValue(const T& value) noexcept {
    return OperandSignature{uint32_t(value) << Support::ConstCTZ<kFieldMask>::value};
  }

  static inline constexpr OperandSignature fromOpType(OperandType opType) noexcept {
    return OperandSignature{uint32_t(opType) << kOpTypeShift};
  }

  static inline constexpr OperandSignature fromRegType(RegType regType) noexcept {
    return OperandSignature{uint32_t(regType) << kRegTypeShift};
  }

  static inline constexpr OperandSignature fromRegGroup(RegGroup regGroup) noexcept {
    return OperandSignature{uint32_t(regGroup) << kRegGroupShift};
  }

  static inline constexpr OperandSignature fromMemBaseType(RegType baseType) noexcept {
    return OperandSignature{uint32_t(baseType) << kMemBaseTypeShift};
  }

  static inline constexpr OperandSignature fromMemIndexType(RegType indexType) noexcept {
    return OperandSignature{uint32_t(indexType) << kMemIndexTypeShift};
  }

  static inline constexpr OperandSignature fromPredicate(uint32_t predicate) noexcept {
    return OperandSignature{predicate << kPredicateShift};
  }

  static inline constexpr OperandSignature fromSize(uint32_t size) noexcept {
    return OperandSignature{size << kSizeShift};
  }

  //! \}
};

//! Base class representing an operand in AsmJit (non-default constructed version).
//!
//! Contains no initialization code and can be used safely to define an array of operands that won't be initialized.
//! This is a \ref Operand base structure designed to be statically initialized, static const, or to be used by user
//! code to define an array of operands without having them default initialized at construction time.
//!
//! The key difference between \ref Operand and \ref Operand_ is:
//!
//! ```
//! Operand_ xArray[10];    // Not initialized, contains garbage.
//! Operand_ yArray[10] {}; // All operands initialized to none explicitly (zero initialized).
//! Operand  yArray[10];    // All operands initialized to none implicitly (zero initialized).
//! ```
struct Operand_ {
  //! \name Types
  //! \{

  typedef OperandSignature Signature;

  //! \}

  //! \name Constants
  //! \{

  // Indexes to `_data` array.
  enum DataIndex : uint32_t {
    kDataMemIndexId = 0,
    kDataMemOffsetLo = 1,

    kDataImmValueLo = ASMJIT_ARCH_LE ? 0 : 1,
    kDataImmValueHi = ASMJIT_ARCH_LE ? 1 : 0
  };
  //! \}

  //! \name Members
  //! \{

  //! Provides operand type and additional payload.
  Signature _signature;
  //! Either base id as used by memory operand or any id as used by others.
  uint32_t _baseId;

  //! Data specific to the operand type.
  //!
  //! The reason we don't use union is that we have `constexpr` constructors that construct operands and other
  //!`constexpr` functions that return whether another Operand or something else. These cannot generally work with
  //! unions so we also cannot use `union` if we want to be standard compliant.
  uint32_t _data[2];

  //! \}

  //! \name Overloaded Operators
  //! \{

  //! Tests whether this operand is the same as `other`.
  inline constexpr bool operator==(const Operand_& other) const noexcept { return  equals(other); }
  //! Tests whether this operand is not the same as `other`.
  inline constexpr bool operator!=(const Operand_& other) const noexcept { return !equals(other); }

  //! \}

  //! \name Cast
  //! \{

  //! Casts this operand to `T` type.
  template<typename T>
  inline T& as() noexcept { return static_cast<T&>(*this); }

  //! Casts this operand to `T` type (const).
  template<typename T>
  inline const T& as() const noexcept { return static_cast<const T&>(*this); }

  //! \}

  //! \name Accessors
  //! \{


  //! Returns the type of the operand, see `OpType`.
  inline constexpr OperandType opType() const noexcept { return _signature.opType(); }
  //! Tests whether the operand is none (`OperandType::kNone`).
  inline constexpr bool isNone() const noexcept { return _signature == Signature::fromBits(0); }
  //! Tests whether the operand is a register (`OperandType::kReg`).
  inline constexpr bool isReg() const noexcept { return opType() == OperandType::kReg; }
  //! Tests whether the operand is a memory location (`OperandType::kMem`).
  inline constexpr bool isMem() const noexcept { return opType() == OperandType::kMem; }
  //! Tests whether the operand is an immediate (`OperandType::kImm`).
  inline constexpr bool isImm() const noexcept { return opType() == OperandType::kImm; }

  //! Returns the size of the operand in bytes.
  //!
  //! The value returned depends on the operand type:
  //!   * None  - Should always return zero size.
  //!   * Reg   - Should always return the size of the register. If the register size depends on architecture
  //!             (like `x86::CReg` and `x86::DReg`) the size returned should be the greatest possible (so it
  //!             should return 64-bit size in such case).
  //!   * Mem   - Size is optional and will be in most cases zero.
  //!   * Imm   - Should always return zero size.
  //!   * Label - Should always return zero size.
  inline constexpr uint32_t size() const noexcept { return _signature.getField<Signature::kSizeMask>(); }

  //! Returns the operand id.
  //!
  //! The value returned should be interpreted accordingly to the operand type:
  //!   * None  - Should be `0`.
  //!   * Reg   - Physical or virtual register id.
  //!   * Mem   - Multiple meanings - BASE address (register or label id), or high value of a 64-bit absolute address.
  //!   * Imm   - Should be `0`.
  //!   * Label - Label id if it was created by using `newLabel()` or `Globals::kInvalidId` if the label is invalid or
  //!             not initialized.
  inline constexpr uint32_t id() const noexcept { return _baseId; }

  //! Tests whether the operand is 100% equal to `other` operand.
  //!
  //! \note This basically performs a binary comparison, if aby bit is
  //! different the operands are not equal.
  inline constexpr bool equals(const Operand_& other) const noexcept {
    return (_signature == other._signature) &
           (_baseId    == other._baseId   ) &
           (_data[0]   == other._data[0]  ) &
           (_data[1]   == other._data[1]  ) ;
  }
};

//! Base class representing an operand in AsmJit (default constructed version).
class Operand : public Operand_ {
public:
  //! \name Construction & Destruction
  //! \{

  //! Creates `kOpNone` operand having all members initialized to zero.
  inline constexpr Operand() noexcept
    : Operand_{ Signature::fromOpType(OperandType::kNone), 0u, { 0u, 0u }} {}

  //! Creates a cloned `other` operand.
  inline constexpr Operand(const Operand& other) noexcept = default;

  //! Creates a cloned `other` operand.
  inline constexpr explicit Operand(const Operand_& other)
    : Operand_(other) {}

  //! Creates an operand initialized to raw `[u0, u1, u2, u3]` values.
  inline constexpr Operand(Globals::Init_, const Signature& u0, uint32_t u1, uint32_t u2, uint32_t u3) noexcept
    : Operand_{ u0, u1, { u2, u3 }} {}

  //! Creates an uninitialized operand (dangerous).
  inline explicit Operand(Globals::NoInit_) noexcept {}

  //! \}

  //! \name Overloaded Operators
  //! \{

  inline Operand& operator=(const Operand& other) noexcept = default;
  inline Operand& operator=(const Operand_& other) noexcept { return operator=(static_cast<const Operand&>(other)); }

  //! \}

  //! \}
};

static_assert(sizeof(Operand) == 16, "asmjit::Operand must be exactly 16 bytes long");

//! Label (jump target or data location).
//!
//! Label represents a location in code typically used as a jump target, but may be also a reference to some data or
//! a static variable. Label has to be explicitly created by BaseEmitter.
//!
//! Example of using labels:
//!
//! ```
//! // Create some emitter (for example x86::Assembler).
//! x86::Assembler a;
//!
//! // Create Label instance.
//! Label L1 = a.newLabel();
//!
//! // ... your code ...
//!
//! // Using label.
//! a.jump(L1);
//!
//! // ... your code ...
//!
//! // Bind label to the current position, see `BaseEmitter::bind()`.
//! a.bind(L1);
//! ```
class Label : public Operand {
public:
  //! \name Construction & Destruction
  //! \{

  //! Creates a label operand without ID (you must set the ID to make it valid).
  inline constexpr Label() noexcept
    : Operand(Globals::Init, Signature::fromOpType(OperandType::kLabel), Globals::kInvalidId, 0, 0) {}

  //! Creates a cloned label operand of `other`.
  inline constexpr Label(const Label& other) noexcept
    : Operand(other) {}

  //! Creates a label operand of the given `id`.
  inline constexpr explicit Label(uint32_t id) noexcept
    : Operand(Globals::Init, Signature::fromOpType(OperandType::kLabel), id, 0, 0) {}

  inline explicit Label(Globals::NoInit_) noexcept
    : Operand(Globals::NoInit) {}

  //! Resets the label, will reset all properties and set its ID to `Globals::kInvalidId`.
  inline void reset() noexcept {
    _signature = Signature::fromOpType(OperandType::kLabel);
    _baseId = Globals::kInvalidId;
    _data[0] = 0;
    _data[1] = 0;
  }

  //! \}

  //! \name Overloaded Operators
  //! \{

  inline Label& operator=(const Label& other) noexcept = default;

  //! \}

  //! \name Accessors
  //! \{

  //! Tests whether the label was created by CodeHolder and/or an attached emitter.
  inline constexpr bool isValid() const noexcept { return _baseId != Globals::kInvalidId; }
  //! Sets the label `id`.
  inline void setId(uint32_t id) noexcept { _baseId = id; }

  //! \}
};

//! \cond INTERNAL
//! Default register traits.
struct BaseRegTraits {
  enum : uint32_t {
    //! \ref TypeId representing this register type, could be \ref TypeId::kVoid if such type doesn't exist.
    kTypeId = uint32_t(TypeId::kVoid),
    //! RegType is not valid by default.
    kValid = 0,
    //! Count of registers (0 if none).
    kCount = 0,

    //! Zero type by default (defeaults to None).
    kType = uint32_t(RegType::kNone),
    //! Zero group by default (defaults to GP).
    kGroup = uint32_t(RegGroup::kGp),
    //! No size by default.
    kSize = 0,

    //! Empty signature by default (not even having operand type set to register).
    kSignature = 0
  };
};
//! \endcond

//! Physical or virtual register operand.
class BaseReg : public Operand {
public:
  //! \name Constants
  //! \{

  enum : uint32_t {
    //! None or any register (mostly internal).
    kIdBad = 0xFFu,

    kBaseSignatureMask =
      Signature::kOpTypeMask   |
      Signature::kRegTypeMask  |
      Signature::kRegGroupMask |
      Signature::kSizeMask,

    kTypeNone = uint32_t(RegType::kNone),
    kSignature = Signature::fromOpType(OperandType::kReg).bits()
  };

  //! \}

  //! \name Construction & Destruction
  //! \{

  //! Creates a dummy register operand.
  inline constexpr BaseReg() noexcept
    : Operand(Globals::Init, Signature::fromOpType(OperandType::kReg), kIdBad, 0, 0) {}

  //! Creates a new register operand which is the same as `other` .
  inline constexpr BaseReg(const BaseReg& other) noexcept
    : Operand(other) {}

  //! Creates a new register operand compatible with `other`, but with a different `id`.
  inline constexpr BaseReg(const BaseReg& other, uint32_t id) noexcept
    : Operand(Globals::Init, other._signature, id, 0, 0) {}

  //! Creates a register initialized to the given `signature` and `id`.
  inline constexpr BaseReg(const Signature& signature, uint32_t id) noexcept
    : Operand(Globals::Init, signature, id, 0, 0) {}

  inline explicit BaseReg(Globals::NoInit_) noexcept
    : Operand(Globals::NoInit) {}

  //! \}

  //! \name Accessors
  //! \{

  //! Returns base signature of the register associated with each register type.
  //!
  //! Base signature only contains the operand type, register type, register group, and register size. It doesn't
  //! contain element type, predicate, or other architecture-specific data. Base signature is a signature that is
  //! provided by architecture-specific `RegTraits`, like \ref x86::RegTraits.
  inline constexpr OperandSignature baseSignature() const noexcept { return _signature & kBaseSignatureMask; }

  //! Tests whether the operand's base signature matches the given signature `sign`.
  inline constexpr bool hasBaseSignature(uint32_t signature) const noexcept { return baseSignature() == signature; }
  //! Tests whether the register is valid (either virtual or physical).
  inline constexpr bool isValid() const noexcept { return (_signature != 0) & (_baseId != kIdBad); }
  inline constexpr RegType type() const noexcept { return _signature.regType(); }
};

#define ASMJIT_DEFINE_REG_TRAITS(REG, REG_TYPE, GROUP, SIZE, COUNT, TYPE_ID) \
template<>                                                                   \
struct RegTraits<REG_TYPE> {                                                 \
  typedef REG RegT;                                                          \
                                                                             \
  enum : uint32_t {                                                          \
    kValid = uint32_t(true),                                                 \
    kCount = uint32_t(COUNT),                                                \
    kType = uint32_t(REG_TYPE),                                              \
    kGroup = uint32_t(GROUP),                                                \
    kSize = uint32_t(SIZE),                                                  \
    kTypeId = uint32_t(TYPE_ID),                                             \
                                                                             \
    kSignature = (OperandSignature::fromOpType(OperandType::kReg) |          \
                  OperandSignature::fromRegType(REG_TYPE)         |          \
                  OperandSignature::fromRegGroup(GROUP)           |          \
                  OperandSignature::fromSize(kSize)).bits(),                 \
  };                                                                         \
}

//! Adds constructors and member functions to a class that implements abstract register. Abstract register is register
//! that doesn't have type or signature yet, it's a base class like `x86::Reg` or `arm::Reg`.
#define ASMJIT_DEFINE_ABSTRACT_REG(REG, BASE)                                \
public:                                                                      \
  /*! Default constructor that only setups basics. */                        \
  inline constexpr REG() noexcept                                            \
    : BASE(Signature{kSignature}, kIdBad) {}                                 \
                                                                             \
  /*! Makes a copy of the `other` register operand. */                       \
  inline constexpr REG(const REG& other) noexcept                            \
    : BASE(other) {}                                                         \
                                                                             \
  /*! Makes a copy of the `other` register having id set to `id` */          \
  inline constexpr REG(const BaseReg& other, uint32_t id) noexcept           \
    : BASE(other, id) {}                                                     \
                                                                             \
  /*! Creates a register based on `signature` and `id`. */                   \
  inline constexpr REG(const OperandSignature& sgn, uint32_t id) noexcept    \
    : BASE(sgn, id) {}                                                       \
                                                                             \
  /*! Creates a completely uninitialized REG register operand (garbage). */  \
  inline explicit REG(Globals::NoInit_) noexcept                             \
    : BASE(Globals::NoInit) {}                                               \
                                                                             \
  /*! Creates a new register from register type and id. */                   \
  static inline REG fromTypeAndId(RegType type, uint32_t id) noexcept {      \
    return REG(signatureOf(type), id);                                       \
  }                                                                          \
                                                                             \
  /*! Clones the register operand. */                                        \
  inline constexpr REG clone() const noexcept { return REG(*this); }         \
                                                                             \
  inline REG& operator=(const REG& other) noexcept = default;

//! Adds constructors and member functions to a class that implements final register. Final registers MUST HAVE a valid
//! signature.
#define ASMJIT_DEFINE_FINAL_REG(REG, BASE, TRAITS)                           \
public:                                                                      \
  enum : uint32_t {                                                          \
    kThisType  = TRAITS::kType,                                              \
    kThisGroup = TRAITS::kGroup,                                             \
    kThisSize  = TRAITS::kSize,                                              \
    kSignature = TRAITS::kSignature                                          \
  };                                                                         \
                                                                             \
  ASMJIT_DEFINE_ABSTRACT_REG(REG, BASE)                                      \
                                                                             \
  /*! Creates a register operand having its id set to `id`. */               \
  inline constexpr explicit REG(uint32_t id) noexcept                        \
    : BASE(Signature{kSignature}, id) {}
//! \endcond

//! Base class for all memory operands.
//!
//! The data is split into the following parts:
//!
//!   - BASE - Base register or label - requires 36 bits total. 4 bits are used to encode the type of the BASE operand
//!     (label vs. register type) and the remaining 32 bits define the BASE id, which can be a physical or virtual
//!     register index. If BASE type is zero, which is never used as a register type and label doesn't use it as well
//!     then BASE field contains a high DWORD of a possible 64-bit absolute address, which is possible on X64.
//!
//!   - INDEX - Index register (or theoretically Label, which doesn't make sense). Encoding is similar to BASE - it
//!     also requires 36 bits and splits the encoding to INDEX type (4 bits defining the register type) and 32-bit id.
//!
//!   - OFFSET - A relative offset of the address. Basically if BASE is specified the relative displacement adjusts
//!     BASE and an optional INDEX. if BASE is not specified then the OFFSET should be considered as ABSOLUTE address
//!     (at least on X86). In that case its low 32 bits are stored in DISPLACEMENT field and the remaining high 32
//!     bits are stored in BASE.
//!
//!   - OTHER - There is rest 8 bits that can be used for whatever purpose. For example \ref x86::Mem operand uses
//!     these bits to store segment override prefix and index shift (or scale).
class BaseMem : public Operand {
public:
  //! \name Construction & Destruction
  //! \{

  //! Creates a default `BaseMem` operand, that points to [0].
  inline constexpr BaseMem() noexcept
      : Operand(Globals::Init, Signature::fromOpType(OperandType::kMem), 0, 0, 0) {}

  //! Creates a `BaseMem` operand that is a clone of `other`.
  inline constexpr BaseMem(const BaseMem& other) noexcept
    : Operand(other) {}

  //! Creates a `BaseMem` operand from `baseReg` and `offset`.
  //!
  //! \note This is an architecture independent constructor that can be used to create an architecture
  //! independent memory operand to be used in portable code that can handle multiple architectures.
  inline constexpr explicit BaseMem(const BaseReg& baseReg, int32_t offset = 0) noexcept
    : Operand(Globals::Init,
              Signature::fromOpType(OperandType::kMem) | Signature::fromMemBaseType(baseReg.type()),
              baseReg.id(),
              0,
              uint32_t(offset)) {}

  //! \cond INTERNAL
  //! Creates a `BaseMem` operand from 4 integers as used by `Operand_` struct.
  inline constexpr BaseMem(const OperandSignature& u0, uint32_t baseId, uint32_t indexId, int32_t offset) noexcept
    : Operand(Globals::Init, u0, baseId, indexId, uint32_t(offset)) {}
  //! \endcond

  //! Creates a completely uninitialized `BaseMem` operand.
  inline explicit BaseMem(Globals::NoInit_) noexcept
    : Operand(Globals::NoInit) {}

  //! \name Accessors
  //! \{

  //! Tests whether the memory operand has a BASE register or label specified.
  inline constexpr bool hasBase() const noexcept {
    return (_signature & Signature::kMemBaseTypeMask) != 0;
  }

  //! Tests whether the memory operand has an INDEX register specified.
  inline constexpr bool hasIndex() const noexcept {
    return (_signature & Signature::kMemIndexTypeMask) != 0;
  }

  //! Tests whether the memory operand has BASE or INDEX register.
  inline constexpr bool hasBaseOrIndex() const noexcept {
    return (_signature & Signature::kMemBaseIndexMask) != 0;
  }

  //! This is used internally for BASE+INDEX validation.
  inline constexpr uint32_t baseAndIndexTypes() const noexcept { return _signature.getField<Signature::kMemBaseIndexMask>(); }

  //! Returns both BASE (4:0 bits) and INDEX (9:5 bits) types combined into a single value.
  //!
  //! \remarks Returns id of the BASE register or label (if the BASE was specified as label).
  inline constexpr uint32_t baseId() const noexcept { return _baseId; }

  //! Returns the id of the INDEX register.
  inline constexpr uint32_t indexId() const noexcept { return _data[kDataMemIndexId]; }
  //! Returns a 32-bit low part of a 64-bit offset or absolute address.
  inline constexpr int32_t offsetLo32() const noexcept { return int32_t(_data[kDataMemOffsetLo]); }
};

//! Immediate operands are encoded with instruction data.
class Imm : public Operand {
public:
  //! \cond INTERNAL
  template<typename T>
  struct IsConstexprConstructibleAsImmType
    : public std::integral_constant<bool, std::is_enum<T>::value ||
                                          std::is_pointer<T>::value ||
                                          std::is_integral<T>::value ||
                                          std::is_function<T>::value> {};
  //! \endcond

  //! \name Construction & Destruction
  //! \{

  //! Creates a new immediate value (initial value is 0).
  inline constexpr Imm() noexcept
    : Operand(Globals::Init, Signature::fromOpType(OperandType::kImm), 0, 0, 0) {}

  //! Creates a new immediate value from `other`.
  inline constexpr Imm(const Imm& other) noexcept
    : Operand(other) {}

  //! Creates a new signed immediate value, assigning the value to `val` and an architecture-specific predicate
  //! to `predicate`.
  //!
  //! \note Predicate is currently only used by ARM architectures.
  template<typename T, typename = typename std::enable_if<IsConstexprConstructibleAsImmType<typename std::decay<T>::type>::value>::type>
  inline constexpr Imm(const T& val, const uint32_t predicate = 0) noexcept
    : Operand(Globals::Init,
              Signature::fromOpType(OperandType::kImm) | Signature::fromPredicate(predicate),
              0,
              Support::unpackU32At0(int64_t(val)),
              Support::unpackU32At1(int64_t(val))) {}

  //! Returns the immediate value as `int64_t`, which is the internal format Imm uses.
  inline constexpr int64_t value() const noexcept {
    return int64_t((uint64_t(_data[kDataImmValueHi]) << 32) | _data[kDataImmValueLo]);
  }
};

//! Creates a new immediate operand.
template<typename T>
static inline constexpr Imm imm(const T& val) noexcept { return Imm(val); }

//! \}

ASMJIT_END_NAMESPACE

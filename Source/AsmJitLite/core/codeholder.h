#pragma once

#include "../core/archtraits.h"
#include "../core/codebuffer.h"
#include "../core/environment.h"
#include "../core/errorhandler.h"
#include "../core/operand.h"
#include "../core/string.h"
#include "../core/support.h"
#include "../core/zone.h"
#include "../core/zonehash.h"
#include "../core/zonestring.h"
#include "../core/zonetree.h"
#include "../core/zonevector.h"

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_core
//! \{

class BaseEmitter;
class CodeHolder;
class LabelEntry;
class Logger;

//! Operator type that can be used within an \ref Expression.
enum class ExpressionOpType : uint8_t {
  //! Addition.
  kAdd = 0,
  //! Subtraction.
  kSub = 1,
  //! Multiplication
  kMul = 2,
  //! Logical left shift.
  kSll = 3,
  //! Logical right shift.
  kSrl = 4,
  //! Arithmetic right shift.
  kSra = 5
};

//! Value tyoe that can be used within an \ref Expression.
enum class ExpressionValueType : uint8_t {
  //! No value or invalid.
  kNone = 0,
  //! Value is 64-bit unsigned integer (constant).
  kConstant = 1,
  //! Value is \ref LabelEntry, which references a \ref Label.
  kLabel = 2,
  //! Value is \ref Expression
  kExpression = 3
};

//! Expression node that can reference constants, labels, and another expressions.
struct Expression {
  //! Expression value.
  union Value {
    //! Constant.
    uint64_t constant;
    //! Pointer to another expression.
    Expression* expression;
    //! Pointer to \ref LabelEntry.
    LabelEntry* label;
  };

  //! \name Members
  //! \{

  //! Operation type.
  ExpressionOpType opType;
  //! Value types of \ref value.
  ExpressionValueType valueType[2];
  //! Reserved for future use, should be initialized to zero.
  uint8_t reserved[5];
  //! Expression left and right values.
  Value value[2];

  //! \}

  //! \name Accessors
  //! \{

  //! Resets the whole expression.
  //!
  //! Changes both values to \ref ExpressionValueType::kNone.
  inline void reset() noexcept { memset(this, 0, sizeof(*this)); }

  //! Sets the value type at `index` to \ref ExpressionValueType::kConstant and its content to `constant`.
  inline void setValueAsConstant(size_t index, uint64_t constant) noexcept {
    valueType[index] = ExpressionValueType::kConstant;
    value[index].constant = constant;
  }

  //! Sets the value type at `index` to \ref ExpressionValueType::kLabel and its content to `labelEntry`.
  inline void setValueAsLabel(size_t index, LabelEntry* labelEntry) noexcept {
    valueType[index] = ExpressionValueType::kLabel;
    value[index].label = labelEntry;
  }

  //! Sets the value type at `index` to \ref ExpressionValueType::kExpression and its content to `expression`.
  inline void setValueAsExpression(size_t index, Expression* expression) noexcept {
    valueType[index] = ExpressionValueType::kExpression;
    value[index].expression = expression;
  }

  //! \}
};

//! Section flags, used by \ref Section.
enum class SectionFlags : uint32_t {
  //! No flags.
  kNone = 0,
  //! Executable (.text sections).
  kExecutable = 0x00000001u,
  //! Read-only (.text and .data sections).
  kReadOnly = 0x00000002u,
  //! Zero initialized by the loader (BSS).
  kZeroInitialized = 0x00000004u,
  //! Info / comment flag.
  kComment = 0x00000008u,
  //! Section created implicitly, can be deleted by \ref Target.
  kImplicit = 0x80000000u
};
ASMJIT_DEFINE_ENUM_FLAGS(SectionFlags)

//! Flags that can be used with \ref CodeHolder::copySectionData() and \ref CodeHolder::copyFlattenedData().
enum class CopySectionFlags : uint32_t {
  //! No flags.
  kNone = 0,

  //! If virtual size of a section is greater than the size of its \ref CodeBuffer then all bytes between the buffer
  //! size and virtual size will be zeroed. If this option is not set then those bytes would be left as is, which
  //! means that if the user didn't initialize them they would have a previous content, which may be unwanted.
  kPadSectionBuffer = 0x00000001u,

  //! Clears the target buffer if the flattened data is less than the destination size. This option works
  //! only with \ref CodeHolder::copyFlattenedData() as it processes multiple sections. It is ignored by
  //! \ref CodeHolder::copySectionData().
  kPadTargetBuffer = 0x00000002u
};
ASMJIT_DEFINE_ENUM_FLAGS(CopySectionFlags)

//! Section entry.
class Section {
public:
  //! \name Members
  //! \{

  //! Section id.
  uint32_t _id;
  //! Section flags.
  SectionFlags _flags;
  //! Section alignment requirements (0 if no requirements).
  uint32_t _alignment;
  //! Offset of this section from base-address.
  uint64_t _offset;
  //! Virtual size of the section (zero initialized sections).
  uint64_t _virtualSize;
  //! Section name (max 35 characters, PE allows max 8).
  FixedString<Globals::kMaxSectionNameSize + 1> _name;
  //! Code or data buffer.
  CodeBuffer _buffer;

  //! \}

  //! \name Accessors
  //! \{

  //! Returns the section id.
  inline uint32_t id() const noexcept { return _id; }

  //! Returns the section data.
  inline uint8_t* data() noexcept { return _buffer.data(); }
  //! \overload
  inline const uint8_t* data() const noexcept { return _buffer.data(); }

  //! Returns the minimum section alignment
  inline uint32_t alignment() const noexcept { return _alignment; }

  //! Returns the section offset, relative to base.
  inline uint64_t offset() const noexcept { return _offset; }
  //! Returns the virtual size of the section.
  //!
  //! Virtual size is initially zero and is never changed by AsmJit. It's normal if virtual size is smaller than
  //! size returned by `bufferSize()` as the buffer stores real data emitted by assemblers or appended by users.
  //!
  //! Use `realSize()` to get the real and final size of this section.
  inline uint64_t virtualSize() const noexcept { return _virtualSize; }
  //! Returns the buffer size of the section.
  inline size_t bufferSize() const noexcept { return _buffer.size(); }
  //! Returns the real size of the section calculated from virtual and buffer sizes.
  inline uint64_t realSize() const noexcept { return Support::max<uint64_t>(virtualSize(), bufferSize()); }

  //! Returns the `CodeBuffer` used by this section.
  inline CodeBuffer& buffer() noexcept { return _buffer; }
  //! Returns the `CodeBuffer` used by this section (const).
  inline const CodeBuffer& buffer() const noexcept { return _buffer; }

  //! \}
};

//! Entry in an address table.
class AddressTableEntry : public ZoneTreeNodeT<AddressTableEntry> {
public:
  ASMJIT_NONCOPYABLE(AddressTableEntry)

  //! \name Members
  //! \{

  //! Address.
  uint64_t _address;
  //! Slot.
  uint32_t _slot;

  //! \}

  //! \name Construction & Destruction
  //! \{

  inline explicit AddressTableEntry(uint64_t address) noexcept
    : _address(address),
      _slot(0xFFFFFFFFu) {}

  //! \}

  //! \name Accessors
  //! \{

  inline uint64_t address() const noexcept { return _address; }
  inline uint32_t slot() const noexcept { return _slot; }

  inline bool hasAssignedSlot() const noexcept { return _slot != 0xFFFFFFFFu; }

  inline bool operator<(const AddressTableEntry& other) const noexcept { return _address < other._address; }
  inline bool operator>(const AddressTableEntry& other) const noexcept { return _address > other._address; }

  inline bool operator<(uint64_t queryAddress) const noexcept { return _address < queryAddress; }
  inline bool operator>(uint64_t queryAddress) const noexcept { return _address > queryAddress; }

  //! \}
};

//! Offset format type, used by \ref OffsetFormat.
enum class OffsetType : uint8_t {
  //! A value having `_immBitCount` bits and shifted by `_immBitShift`.
  //!
  //! This offset type is sufficient for many targets that store offset as a continuous set bits within an
  //! instruction word / sequence of bytes.
  kSignedOffset,

  //! An unsigned value having `_immBitCount` bits and shifted by `_immBitShift`.
  kUnsignedOffset,

  // AArch64 Specific Offset Formats
  // -------------------------------

  //! AARCH64 ADR format of `[.|immlo:2|.....|immhi:19|.....]`.
  kAArch64_ADR,

  //! AARCH64 ADRP format of `[.|immlo:2|.....|immhi:19|.....]` (4kB pages).
  kAArch64_ADRP,

  //! Maximum value of `OffsetFormatType`.
  kMaxValue = kAArch64_ADRP
};

//! Provides information about formatting offsets, absolute addresses, or their parts. Offset format is used by both
//! \ref RelocEntry and \ref LabelLink. The illustration below describes the relation of region size and offset size.
//! Region size is the size of the whole unit whereas offset size is the size of the unit that will be patched.
//!
//! ```
//! +-> Code buffer |   The subject of the relocation (region)  |
//! |               | (Word-Offset)  (Word-Size)                |
//! |xxxxxxxxxxxxxxx|................|*PATCHED*|................|xxxxxxxxxxxx->
//!                                  |         |
//!     [Word Offset points here]----+         +--- [WordOffset + WordSize]
//! ```
//!
//! Once the offset word has been located it can be patched like this:
//!
//! ```
//!                               |ImmDiscardLSB (discard LSB bits).
//!                               |..
//! [0000000000000iiiiiiiiiiiiiiiiiDD] - Offset value (32-bit)
//! [000000000000000iiiiiiiiiiiiiiiii] - Offset value after discard LSB.
//! [00000000000iiiiiiiiiiiiiiiii0000] - Offset value shifted by ImmBitShift.
//! [xxxxxxxxxxxiiiiiiiiiiiiiiiiixxxx] - Patched word (32-bit)
//!             |...............|
//!               (ImmBitCount) +- ImmBitShift
//! ```
struct OffsetFormat {
  //! \name Members
  //! \{

  //! Type of the offset.
  OffsetType _type;
  //! Encoding flags.
  uint8_t _flags;
  //! Size of the region (in bytes) containing the offset value, if the offset value is part of an instruction,
  //! otherwise it would be the same as `_valueSize`.
  uint8_t _regionSize;
  //! Size of the offset value, in bytes (1, 2, 4, or 8).
  uint8_t _valueSize;
  //! Offset of the offset value, in bytes, relative to the start of the region or data. Value offset would be
  //! zero if both region size and value size are equal.
  uint8_t _valueOffset;
  //! Size of the offset immediate value in bits.
  uint8_t _immBitCount;
  //! Shift of the offset immediate value in bits in the target word.
  uint8_t _immBitShift;
  //! Number of least significant bits to discard before writing the immediate to the destination. All discarded
  //! bits must be zero otherwise the value is invalid.
  uint8_t _immDiscardLsb;

  //! \}

  //! \name Accessors
  //! \{

  //! Returns the type of the offset.
  inline OffsetType type() const noexcept { return _type; }

  //! Returns flags.
  inline uint32_t flags() const noexcept { return _flags; }

  //! Returns the size of the region/instruction where the offset is encoded.
  inline uint32_t regionSize() const noexcept { return _regionSize; }

  //! Returns the offset of the word relative to the start of the region where the offset is.
  inline uint32_t valueOffset() const noexcept { return _valueOffset; }

  //! Returns the size of the data-type (word) that contains the offset, in bytes.
  inline uint32_t valueSize() const noexcept { return _valueSize; }
  //! Returns the count of bits of the offset value in the data it's stored in.
  inline uint32_t immBitCount() const noexcept { return _immBitCount; }
  //! Returns the bit-shift of the offset value in the data it's stored in.
  inline uint32_t immBitShift() const noexcept { return _immBitShift; }
  //! Returns the number of least significant bits of the offset value, that must be zero and that are not part of
  //! the encoded data.
  inline uint32_t immDiscardLsb() const noexcept { return _immDiscardLsb; }

  //! Resets this offset format to a simple data value of `dataSize` bytes.
  //!
  //! The region will be the same size as data and immediate bits would correspond to `dataSize * 8`. There will be
  //! no immediate bit shift or discarded bits.
  inline void resetToSimpleValue(OffsetType type, size_t valueSize) noexcept {
    ASMJIT_ASSERT(valueSize <= 8u);

    _type = type;
    _flags = uint8_t(0);
    _regionSize = uint8_t(valueSize);
    _valueSize = uint8_t(valueSize);
    _valueOffset = uint8_t(0);
    _immBitCount = uint8_t(valueSize * 8u);
    _immBitShift = uint8_t(0);
    _immDiscardLsb = uint8_t(0);
  }

  inline void resetToImmValue(OffsetType type, size_t valueSize, uint32_t immBitShift, uint32_t immBitCount, uint32_t immDiscardLsb) noexcept {
    ASMJIT_ASSERT(valueSize <= 8u);
    ASMJIT_ASSERT(immBitShift < valueSize * 8u);
    ASMJIT_ASSERT(immBitCount <= 64u);
    ASMJIT_ASSERT(immDiscardLsb <= 64u);

    _type = type;
    _flags = uint8_t(0);
    _regionSize = uint8_t(valueSize);
    _valueSize = uint8_t(valueSize);
    _valueOffset = uint8_t(0);
    _immBitCount = uint8_t(immBitCount);
    _immBitShift = uint8_t(immBitShift);
    _immDiscardLsb = uint8_t(immDiscardLsb);
  }

  inline void setRegion(size_t regionSize, size_t valueOffset) noexcept {
    _regionSize = uint8_t(regionSize);
    _valueOffset = uint8_t(valueOffset);
  }

  inline void setLeadingAndTrailingSize(size_t leadingSize, size_t trailingSize) noexcept {
    _regionSize = uint8_t(leadingSize + trailingSize + _valueSize);
    _valueOffset = uint8_t(leadingSize);
  }

  //! \}
};

//! Relocation type.
enum class RelocType : uint32_t {
  //! None/deleted (no relocation).
  kNone = 0,
  //! Expression evaluation, `_payload` is pointer to `Expression`.
  kExpression = 1,
  //! Relocate absolute to absolute.
  kAbsToAbs = 2,
  //! Relocate relative to absolute.
  kRelToAbs = 3,
  //! Relocate absolute to relative.
  kAbsToRel = 4,
  //! Relocate absolute to relative or use trampoline.
  kX64AddressEntry = 5
};

//! Relocation entry.
struct RelocEntry {
  //! \name Members
  //! \{

  //! Relocation id.
  uint32_t _id;
  //! Type of the relocation.
  RelocType _relocType;
  //! Format of the relocated value.
  OffsetFormat _format;
  //! Source section id.
  uint32_t _sourceSectionId;
  //! Target section id.
  uint32_t _targetSectionId;
  //! Source offset (relative to start of the section).
  uint64_t _sourceOffset;
  //! Payload (target offset, target address, expression, etc).
  uint64_t _payload;

  //! \}

  //! \name Accessors
  //! \{

  inline uint32_t id() const noexcept { return _id; }

  inline RelocType relocType() const noexcept { return _relocType; }
  inline const OffsetFormat& format() const noexcept { return _format; }

  inline uint32_t sourceSectionId() const noexcept { return _sourceSectionId; }
  inline uint32_t targetSectionId() const noexcept { return _targetSectionId; }

  inline uint64_t sourceOffset() const noexcept { return _sourceOffset; }
  inline uint64_t payload() const noexcept { return _payload; }

  Expression* payloadAsExpression() const noexcept {
    return reinterpret_cast<Expression*>(uintptr_t(_payload));
  }

  //! \}
};

//! Type of the \ref Label.
enum class LabelType : uint8_t {
  //! Anonymous label that can optionally have a name, which is only used for debugging purposes.
  kAnonymous = 0,
  //! Local label (always has parentId).
  kLocal = 1,
  //! Global label (never has parentId).
  kGlobal = 2,
  //! External label (references an external symbol).
  kExternal = 3,

  //! Maximum value of `LabelType`.
  kMaxValue = kExternal
};

//! Data structure used to link either unbound labels or cross-section links.
struct LabelLink {
  //! Next link (single-linked list).
  LabelLink* next;
  //! Section id where the label is bound.
  uint32_t sectionId;
  //! Relocation id or Globals::kInvalidId.
  uint32_t relocId;
  //! Label offset relative to the start of the section.
  size_t offset;
  //! Inlined rel8/rel32.
  intptr_t rel;
  //! Offset format information.
  OffsetFormat format;
};

//! Label entry.
//!
//! Contains the following properties:
//!   - Label id - This is the only thing that is set to the `Label` operand.
//!   - Label name - Optional, used mostly to create executables and libraries.
//!   - Label type - Type of the label, default `LabelType::kAnonymous`.
//!   - Label parent id - Derived from many assemblers that allow to define a local label that falls under a global
//!     label. This allows to define many labels of the same name that have different parent (global) label.
//!   - Offset - offset of the label bound by `Assembler`.
//!   - Links - single-linked list that contains locations of code that has to be patched when the label gets bound.
//!     Every use of unbound label adds one link to `_links` list.
//!   - HVal - Hash value of label's name and optionally parentId.
//!   - HashNext - Hash-table implementation detail.
class LabelEntry : public ZoneHashNode {
public:
  //! \name Constants
  //! \{

  enum : uint32_t {
    //! SSO size of \ref _name.
    //!
    //! \cond INTERNAL
    //! Let's round the size of `LabelEntry` to 64 bytes (as `ZoneAllocator` has granularity of 32 bytes anyway). This
    //! gives `_name` the remaining space, which is should be 16 bytes on 64-bit and 28 bytes on 32-bit architectures.
    //! \endcond
    kStaticNameSize = 64 - (sizeof(ZoneHashNode) + 8 + sizeof(Section*) + sizeof(size_t) + sizeof(LabelLink*))
  };

  //! \}

  //! \name Members
  //! \{

  //! Type of the label.
  LabelType _type;
  //! Label parent id or zero.
  uint32_t _parentId;
  //! Label offset relative to the start of the `_section`.
  uint64_t _offset;
  //! Section where the label was bound.
  Section* _section;
  //! Label links.
  LabelLink* _links;
  //! Label name.
  ZoneString<kStaticNameSize> _name;

  //! \}

  //! \name Accessors
  //! \{

  // NOTE: Label id is stored in `_customData`, which is provided by ZoneHashNode to fill a padding that a C++
  // compiler targeting 64-bit CPU will add to align the structure to 64-bits.

  //! Returns label id.
  inline uint32_t id() const noexcept { return _customData; }
  //! Sets label id (internal, used only by `CodeHolder`).
  inline void _setId(uint32_t id) noexcept { _customData = id; }

  //! Returns label type.
  inline LabelType type() const noexcept { return _type; }

  //! Tests whether the label has a parent label.
  inline bool hasParent() const noexcept { return _parentId != Globals::kInvalidId; }
  //! Returns label's parent id.
  inline uint32_t parentId() const noexcept { return _parentId; }

  //! Returns the section where the label was bound.
  //!
  //! If the label was not yet bound the return value is `nullptr`.
  inline Section* section() const noexcept { return _section; }

  //! Tests whether the label has name.
  inline bool hasName() const noexcept { return !_name.empty(); }

  //! Returns the label's name.
  //!
  //! \note Local labels will return their local name without their parent part, for example ".L1".
  inline const char* name() const noexcept { return _name.data(); }
  //! Tests whether the label is bound.
  inline bool isBound() const noexcept { return _section != nullptr; }
  //! Tests whether the label is bound to a the given `sectionId`.
  inline bool isBoundTo(Section* section) const noexcept { return _section == section; }

  //! Returns the label offset (only useful if the label is bound).
  inline uint64_t offset() const noexcept { return _offset; }

  //! \}
};

//! Holds assembled code and data (including sections, labels, and relocation information).
//!
//! CodeHolder connects emitters with their targets. It provides them interface that can be used to query information
//! about the target environment (architecture, etc...) and API to create labels, sections, relocations, and to write
//! data to a \ref CodeBuffer, which is always part of \ref Section. More than one emitter can be attached to a single
//! CodeHolder instance at a time, which is used in practice
//!
//! CodeHolder provides interface for all emitter types. Assemblers use CodeHolder to write into \ref CodeBuffer, and
//! higher level emitters like Builder and Compiler use CodeHolder to manage labels and sections so higher level code
//! can be serialized to Assembler by \ref BaseEmitter::finalize() and \ref BaseBuilder::serializeTo().
//!
//! In order to use CodeHolder, it must be first initialized by \ref init(). After the CodeHolder has been successfully
//! initialized it can be used to hold assembled code, sections, labels, relocations, and to attach / detach code
//! emitters. After the end of code generation it can be used to query physical locations of labels and to relocate
//! the assembled code into the right address.
//!
//! \note \ref CodeHolder has an ability to attach an \ref ErrorHandler, however, the error handler is not triggered
//! by \ref CodeHolder itself, it's instead propagated to all emitters that attach to it.
class CodeHolder {
public:
  ASMJIT_NONCOPYABLE(CodeHolder)

  //! \name Members
  //! \{

  //! Environment information.
  Environment _environment;
  //! Base address or \ref Globals::kNoBaseAddress.
  uint64_t _baseAddress;

  //! Attached `Logger`, used by all consumers.
  Logger* _logger;
  //! Attached `ErrorHandler`.
  ErrorHandler* _errorHandler;

  //! Code zone (used to allocate core structures).
  Zone _zone;
  //! Zone allocator, used to manage internal containers.
  ZoneAllocator _allocator;

  //! Attached emitters.
  ZoneVector<BaseEmitter*> _emitters;
  //! Section entries.
  ZoneVector<Section*> _sections;
  //! Section entries sorted by section order and then section id.
  ZoneVector<Section*> _sectionsByOrder;
  //! Label entries.
  ZoneVector<LabelEntry*> _labelEntries;
  //! Relocation entries.
  ZoneVector<RelocEntry*> _relocations;

  //! Count of label links, which are not resolved.
  size_t _unresolvedLinkCount;
  //! Pointer to an address table section (or null if this section doesn't exist).
  Section* _addressTableSection;
  //! Address table entries.
  ZoneTree<AddressTableEntry> _addressTableEntries;

  //! \}

  //! \name Construction & Destruction
  //! \{

  //! Creates an uninitialized CodeHolder (you must init() it before it can be used).
  //!
  //! An optional `temporary` argument can be used to initialize the first block of \ref Zone that the CodeHolder
  //! uses into a temporary memory provided by the user.
  ASMJIT_API explicit CodeHolder(const Support::Temporary* temporary = nullptr) noexcept;
  //! Destroys the CodeHolder and frees all resources it has allocated.
  ASMJIT_API ~CodeHolder() noexcept;

  //! Tests whether the `CodeHolder` has been initialized.
  //!
  //! Emitters can be only attached to initialized `CodeHolder` instances.
  inline bool isInitialized() const noexcept { return _environment.isInitialized(); }

  //! Initializes CodeHolder to hold code described by the given `environment` and `baseAddress`.
  ASMJIT_API Error init(const Environment& environment, uint64_t baseAddress = Globals::kNoBaseAddress) noexcept;
  //! Detaches all code-generators attached and resets the `CodeHolder`.
  ASMJIT_API void reset(ResetPolicy resetPolicy = ResetPolicy::kSoft) noexcept;

  //! \}

  //! \name Attach & Detach
  //! \{

  //! Attaches an emitter to this `CodeHolder`.
  ASMJIT_API Error attach(BaseEmitter* emitter) noexcept;
  //! Detaches an emitter from this `CodeHolder`.
  ASMJIT_API Error detach(BaseEmitter* emitter) noexcept;

  //! \}

  //! \name Allocators
  //! \{

  //! Returns the allocator that the `CodeHolder` uses.
  //!
  //! \note This should be only used for AsmJit's purposes. Code holder uses arena allocator to allocate everything,
  //! so anything allocated through this allocator will be invalidated by \ref CodeHolder::reset() or by CodeHolder's
  //! destructor.
  inline ZoneAllocator* allocator() const noexcept { return const_cast<ZoneAllocator*>(&_allocator); }

  //! \}

  //! \name Code & Architecture
  //! \{

  //! Returns the target environment information.
  inline const Environment& environment() const noexcept { return _environment; }

  //! Returns the target architecture.
  inline Arch arch() const noexcept { return environment().arch(); }
  //! Returns a static base-address or \ref Globals::kNoBaseAddress, if not set.
  inline uint64_t baseAddress() const noexcept { return _baseAddress; }

  //! \}

  //! \name Emitters
  //! \{

  //! Returns a vector of attached emitters.
  inline const ZoneVector<BaseEmitter*>& emitters() const noexcept { return _emitters; }

  //! Returns the attached logger.
  inline Logger* logger() const noexcept { return _logger; }
  //! Attaches a `logger` to CodeHolder and propagates it to all attached emitters.
  ASMJIT_API void setLogger(Logger* logger) noexcept;
  //! Resets the logger to none.
  inline void resetLogger() noexcept { setLogger(nullptr); }

  //! \name Error Handling
  //! \{

  //! Tests whether the CodeHolder has an attached error handler, see \ref ErrorHandler.
  inline bool hasErrorHandler() const noexcept { return _errorHandler != nullptr; }
  //! Returns the attached error handler.
  inline ErrorHandler* errorHandler() const noexcept { return _errorHandler; }
  //! Attach an error handler to this `CodeHolder`.
  ASMJIT_API void setErrorHandler(ErrorHandler* errorHandler) noexcept;
  //! \}

  //! \name Code Buffer
  //! \{

  //! Makes sure that at least `n` bytes can be added to CodeHolder's buffer `cb`.
  //!
  //! \note The buffer `cb` must be managed by `CodeHolder` - otherwise the behavior of the function is undefined.
  ASMJIT_API Error growBuffer(CodeBuffer* cb, size_t n) noexcept;

  //! Reserves the size of `cb` to at least `n` bytes.
  //!
  //! \note The buffer `cb` must be managed by `CodeHolder` - otherwise the behavior of the function is undefined.
  ASMJIT_API Error reserveBuffer(CodeBuffer* cb, size_t n) noexcept;

  //! Returns a section entry of the given index.
  inline Section* sectionById(uint32_t sectionId) const noexcept { return _sections[sectionId]; }
  //! Tests whether the label having `id` is valid (i.e. created by `newLabelEntry()`).
  inline bool isLabelValid(uint32_t labelId) const noexcept {
    return labelId < _labelEntries.size();
  }

  //! Returns LabelEntry of the given label `id`.
  inline LabelEntry* labelEntry(uint32_t labelId) const noexcept {
    return isLabelValid(labelId) ? _labelEntries[labelId] : static_cast<LabelEntry*>(nullptr);
  }

  //! Returns LabelEntry of the given `label`.
  inline LabelEntry* labelEntry(const Label& label) const noexcept {
    return labelEntry(label.id());
  }

  //! Creates a new anonymous label and return its id in `idOut`.
  //!
  //! Returns `Error`, does not report error to `ErrorHandler`.
  ASMJIT_API Error newLabelEntry(LabelEntry** entryOut) noexcept;

  //! Creates a new label-link used to store information about yet unbound labels.
  //!
  //! Returns `null` if the allocation failed.
  ASMJIT_API LabelLink* newLabelLink(LabelEntry* le, uint32_t sectionId, size_t offset, intptr_t rel, const OffsetFormat& format) noexcept;

  //! Binds a label to a given `sectionId` and `offset` (relative to start of the section).
  //!
  //! This function is generally used by `BaseAssembler::bind()` to do the heavy lifting.
  ASMJIT_API Error bindLabel(const Label& label, uint32_t sectionId, uint64_t offset) noexcept;

  //! Creates a new relocation entry of type `relocType`.
  //!
  //! Additional fields can be set after the relocation entry was created.
  ASMJIT_API Error newRelocEntry(RelocEntry** dst, RelocType relocType) noexcept;
  //! Returns computed the size of code & data of all sections.
  //!
  //! \note All sections will be iterated over and the code size returned would represent the minimum code size of
  //! all combined sections after applying minimum alignment. Code size may decrease after calling `flatten()` and
  //! `relocateToBase()`.
  ASMJIT_API size_t codeSize() const noexcept;

  //! Relocates the code to the given `baseAddress`.
  //!
  //! \param baseAddress Absolute base address where the code will be relocated to. Please note that nothing is
  //! copied to such base address, it's just an absolute value used by the relocator to resolve all stored relocations.
  //!
  //! \note This should never be called more than once.
  ASMJIT_API Error relocateToBase(uint64_t baseAddress) noexcept;

  //! Copies all sections into `dst`.
  //!
  //! This should only be used if the data was flattened and there are no gaps between the sections. The `dstSize`
  //! is always checked and the copy will never write anything outside the provided buffer.
  ASMJIT_API Error copyFlattenedData(void* dst, size_t dstSize, CopySectionFlags copyFlags = CopySectionFlags::kNone) noexcept;

  //! \}
};

//! \}

ASMJIT_END_NAMESPACE

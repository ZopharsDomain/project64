#pragma once
#include "../core/archtraits.h"
#include "../core/codeholder.h"
#include "../core/formatter.h"
#include "../core/inst.h"
#include "logger.h"
#include "errorhandler.h"

ASMJIT_BEGIN_NAMESPACE

//! Emitter type used by \ref BaseEmitter.
enum class EmitterType : uint8_t {
  //! Unknown or uninitialized.
  kNone = 0,
  //! Emitter inherits from \ref BaseAssembler.
  kAssembler = 1,
  //! Emitter inherits from \ref BaseBuilder.
  kBuilder = 2,
  //! Emitter inherits from \ref BaseCompiler.
  kCompiler = 3,

  //! Maximum value of `EmitterType`.
  kMaxValue = kCompiler
};

//! Emitter flags, used by \ref BaseEmitter.
enum class EmitterFlags : uint8_t {
  //! No flags.
  kNone = 0u,
  //! Emitter is attached to CodeHolder.
  kAttached = 0x01u,
  //! The emitter must emit comments.
  kLogComments = 0x08u,
  //! The emitter has its own \ref Logger (not propagated from \ref CodeHolder).
  kOwnLogger = 0x10u,
  //! The emitter has its own \ref ErrorHandler (not propagated from \ref CodeHolder).
  kOwnErrorHandler = 0x20u,
  //! The emitter was finalized.
  kFinalized = 0x40u,
  //! The emitter was destroyed.
  //!
  //! This flag is used for a very short time when an emitter is being destroyed by
  //! CodeHolder.
  kDestroyed = 0x80u
};
ASMJIT_DEFINE_ENUM_FLAGS(EmitterFlags)

//! Diagnostic options are used to tell emitters and their passes to perform diagnostics when emitting or processing
//! user code. These options control validation and extra diagnostics that can be performed by higher level emitters.
//!
//! Instruction Validation
//! ----------------------
//!
//! \ref BaseAssembler implementation perform by default only basic checks that are necessary to identify all
//! variations of an instruction so the correct encoding can be selected. This is fine for production-ready code
//! as the assembler doesn't have to perform checks that would slow it down. However, sometimes these checks are
//! beneficial especially when the project that uses AsmJit is in a development phase, in which mistakes happen
//! often. To make the experience of using AsmJit seamless it offers validation features that can be controlled
//! by \ref DiagnosticOptions.
//!
//! Compiler Diagnostics
//! --------------------
//!
//! Diagnostic options work with \ref BaseCompiler passes (precisely with its register allocation pass). These options
//! can be used to enable logging of all operations that the Compiler does.
enum class DiagnosticOptions : uint32_t {
  //! No validation options.
  kNone = 0,

  //! Perform strict validation in \ref BaseAssembler::emit() implementations.
  //!
  //! This flag ensures that each instruction is checked before it's encoded into a binary representation. This flag
  //! is only relevant for \ref BaseAssembler implementations, but can be set in any other emitter type, in that case
  //! if that emitter needs to create an assembler on its own, for the purpose of \ref BaseEmitter::finalize() it
  //! would propagate this flag to such assembler so all instructions passed to it are explicitly validated.
  //!
  //! Default: false.
  kValidateAssembler = 0x00000001u,

  //! Perform strict validation in \ref BaseBuilder::emit() and \ref BaseCompiler::emit() implementations.
  //!
  //! This flag ensures that each instruction is checked before an \ref InstNode representing the instruction is
  //! created by \ref BaseBuilder or \ref BaseCompiler. This option could be more useful than \ref kValidateAssembler
  //! in cases in which there is an invalid instruction passed to an assembler, which was invalid much earlier, most
  //! likely when such instruction was passed to Builder/Compiler.
  //!
  //! This is a separate option that was introduced, because it's possible to manipulate the instruction stream
  //! emitted by \ref BaseBuilder and \ref BaseCompiler - this means that it's allowed to emit invalid instructions
  //! (for example with missing operands) that will be fixed later before finalizing it.
  //!
  //! Default: false.
  kValidateIntermediate = 0x00000002u,

  //! Annotate all nodes processed by register allocator (Compiler/RA).
  //!
  //! \note Annotations don't need debug options, however, some debug options like `kRADebugLiveness` may influence
  //! their output (for example the mentioned option would add liveness information to per-instruction annotation).
  kRAAnnotate = 0x00000080u,

  //! Debug CFG generation and other related algorithms / operations (Compiler/RA).
  kRADebugCFG = 0x00000100u,

  //! Debug liveness analysis (Compiler/RA).
  kRADebugLiveness = 0x00000200u,

  //! Debug register allocation assignment (Compiler/RA).
  kRADebugAssignment = 0x00000400u,

  //! Debug the removal of code part of unreachable blocks.
  kRADebugUnreachable = 0x00000800u,

  //! Enable all debug options (Compiler/RA).
  kRADebugAll = 0x0000FF00u,
};
ASMJIT_DEFINE_ENUM_FLAGS(DiagnosticOptions)

//! Provides a base foundation to emitting code - specialized by \ref BaseAssembler and \ref BaseBuilder.
class ASMJIT_VIRTAPI BaseEmitter {
public:
  ASMJIT_BASE_CLASS(BaseEmitter)

  //! \name Members
  //! \{

  //! See \ref EmitterType.
  EmitterType _emitterType = EmitterType::kNone;
  //! See \ref EmitterFlags.
  EmitterFlags _emitterFlags = EmitterFlags::kNone;
  //! Validation options.
  DiagnosticOptions _diagnosticOptions = DiagnosticOptions::kNone;

  //! All supported architectures in a bit-mask, where LSB is the bit with a zero index.
  uint64_t _archMask = 0;

  //! Forced instruction options, combined with \ref _instOptions by \ref emit().
  InstOptions _forcedInstOptions = InstOptions::kReserved;
  //! Internal private data used freely by any emitter.
  uint32_t _privateData = 0;

  //! CodeHolder the emitter is attached to.
  CodeHolder* _code = nullptr;
  //! Attached \ref Logger.
  Logger* _logger = nullptr;
  //! Attached \ref ErrorHandler.
  ErrorHandler* _errorHandler = nullptr;

  //! Describes the target environment, matches \ref CodeHolder::environment().
  Environment _environment {};
  //! Native GP register signature and signature related information.
  OperandSignature _gpSignature {};
  //! Inline comment of the next instruction (affects the next instruction).
  const char* _inlineComment = nullptr;
  //! \}

  //! \name Construction & Destruction
  //! \{

  ASMJIT_API explicit BaseEmitter(EmitterType emitterType) noexcept;
  ASMJIT_API virtual ~BaseEmitter() noexcept;

  //! \}
  //! \name Emitter Type & Flags
  //! \{

  //! Returns the type of this emitter, see `EmitterType`.
  inline EmitterType emitterType() const noexcept { return _emitterType; }
  //! Returns emitter flags , see `Flags`.
  inline EmitterFlags emitterFlags() const noexcept { return _emitterFlags; }

  //! Tests whether the emitter inherits from `BaseAssembler`.
  inline bool isAssembler() const noexcept { return _emitterType == EmitterType::kAssembler; }
  //! Tests whether the emitter has the given `flag` enabled.
  inline bool hasEmitterFlag(EmitterFlags flag) const noexcept { return Support::test(_emitterFlags, flag); }
  //! Tests whether the emitter is destroyed (only used during destruction).
  inline bool isDestroyed() const noexcept { return hasEmitterFlag(EmitterFlags::kDestroyed); }

  inline void _addEmitterFlags(EmitterFlags flags) noexcept { _emitterFlags |= flags; }
  inline void _clearEmitterFlags(EmitterFlags flags) noexcept { _emitterFlags &= _emitterFlags & ~flags; }

  //! \}

  //! \name Target Information
  //! \{

  //! Returns the CodeHolder this emitter is attached to.
  inline CodeHolder* code() const noexcept { return _code; }
  //! Finalizes this emitter.
  //!
  //! Materializes the content of the emitter by serializing it to the attached \ref CodeHolder through an architecture
  //! specific \ref BaseAssembler. This function won't do anything if the emitter inherits from \ref BaseAssembler as
  //! assemblers emit directly to a \ref CodeBuffer held by \ref CodeHolder. However, if this is an emitter that
  //! inherits from \ref BaseBuilder or \ref BaseCompiler then these emitters need the materialization phase as they
  //! store their content in a representation not visible to \ref CodeHolder.
  ASMJIT_API virtual Error finalize();

  //! Tests whether the emitter has its own logger.
  //!
  //! Own logger means that it overrides the possible logger that may be used by \ref CodeHolder this emitter is
  //! attached to.
  inline bool hasOwnLogger() const noexcept { return hasEmitterFlag(EmitterFlags::kOwnLogger); }

  //! Returns the logger this emitter uses.
  //!
  //! The returned logger is either the emitter's own logger or it's logger used by \ref CodeHolder this emitter
  //! is attached to.
  inline Logger* logger() const noexcept { return _logger; }

  //! Sets or resets the logger of the emitter.
  //!
  //! If the `logger` argument is non-null then the logger will be considered emitter's own logger, see \ref
  //! hasOwnLogger() for more details. If the given `logger` is null then the emitter will automatically use logger
  //! that is attached to the \ref CodeHolder this emitter is attached to.
  ASMJIT_API void setLogger(Logger* logger) noexcept;
  //! Tests whether the emitter has its own error handler.
  //!
  //! Own error handler means that it overrides the possible error handler that may be used by \ref CodeHolder this
  //! emitter is attached to.
  inline bool hasOwnErrorHandler() const noexcept { return hasEmitterFlag(EmitterFlags::kOwnErrorHandler); }

  //! Sets or resets the error handler of the emitter.
  ASMJIT_API void setErrorHandler(ErrorHandler* errorHandler) noexcept;

  //! Handles the given error in the following way:
  //!   1. If the emitter has \ref ErrorHandler attached, it calls its \ref ErrorHandler::handleError() member function
  //!      first, and then returns the error. The `handleError()` function may throw.
  //!   2. if the emitter doesn't have \ref ErrorHandler, the error is simply returned.
  ASMJIT_API Error reportError(Error err, const char* message = nullptr);
  //! Tests whether the given `option` is present in the emitter's diagnostic options.
  inline bool hasDiagnosticOption(DiagnosticOptions option) const noexcept { return Support::test(_diagnosticOptions, option); }
  //! Resets the comment/annotation to nullptr.
  inline void resetInlineComment() noexcept { _inlineComment = nullptr; }

  //! \name Events
  //! \{

  //! Called after the emitter was attached to `CodeHolder`.
  virtual Error onAttach(CodeHolder* ASMJIT_NONNULL(code)) noexcept = 0;
  //! Called after the emitter was detached from `CodeHolder`.
  virtual Error onDetach(CodeHolder* ASMJIT_NONNULL(code)) noexcept = 0;

  //! Called when \ref CodeHolder has updated an important setting, which involves the following:
  //!
  //!   - \ref Logger has been changed (\ref CodeHolder::setLogger() has been called).
  //!
  //!   - \ref ErrorHandler has been changed (\ref CodeHolder::setErrorHandler() has been called).
  //!
  //! This function ensures that the settings are properly propagated from \ref CodeHolder to the emitter.
  //!
  //! \note This function is virtual and can be overridden, however, if you do so, always call \ref
  //! BaseEmitter::onSettingsUpdated() within your own implementation to ensure that the emitter is
  //! in a consistent state.
  ASMJIT_API virtual void onSettingsUpdated() noexcept;

  //! \}
};

//! \}

ASMJIT_END_NAMESPACE

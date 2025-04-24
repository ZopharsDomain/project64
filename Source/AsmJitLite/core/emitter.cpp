#include "emitter.h"

ASMJIT_BEGIN_NAMESPACE

// BaseEmitter - Construction & Destruction
// ========================================

BaseEmitter::BaseEmitter(EmitterType emitterType) noexcept
  : _emitterType(emitterType) {}

BaseEmitter::~BaseEmitter() noexcept {
  if (_code) {
    _addEmitterFlags(EmitterFlags::kDestroyed);
    _code->detach(this);
  }
}

// BaseEmitter - Finalize
// ======================

Error BaseEmitter::finalize() {
  // Does nothing by default, overridden by `BaseBuilder` and `BaseCompiler`.
  return kErrorOk;
}

// BaseEmitter - Internals
// =======================

static constexpr EmitterFlags kEmitterPreservedFlags = EmitterFlags::kOwnLogger | EmitterFlags::kOwnErrorHandler;

static ASMJIT_NOINLINE void BaseEmitter_updateForcedOptions(BaseEmitter* self) noexcept {
  bool emitComments = false;
  bool hasDiagnosticOptions = false;

  if (self->emitterType() == EmitterType::kAssembler) {
    // Assembler: Don't emit comments if logger is not attached.
    emitComments = self->_code != nullptr && self->_logger != nullptr;
    hasDiagnosticOptions = self->hasDiagnosticOption(DiagnosticOptions::kValidateAssembler);
  }
  else {
    // Builder/Compiler: Always emit comments, we cannot assume they won't be used.
    emitComments = self->_code != nullptr;
    hasDiagnosticOptions = self->hasDiagnosticOption(DiagnosticOptions::kValidateIntermediate);
  }

  if (emitComments)
    self->_addEmitterFlags(EmitterFlags::kLogComments);
  else
    self->_clearEmitterFlags(EmitterFlags::kLogComments);

  // The reserved option tells emitter (Assembler/Builder/Compiler) that there may be either a border
  // case (CodeHolder not attached, for example) or that logging or validation is required.
  if (self->_code == nullptr || self->_logger || hasDiagnosticOptions)
    self->_forcedInstOptions |= InstOptions::kReserved;
  else
    self->_forcedInstOptions &= ~InstOptions::kReserved;
}


// BaseEmitter - Logging
// =====================

void BaseEmitter::setLogger(Logger* logger) noexcept {
#ifndef ASMJIT_NO_LOGGING
  if (logger) {
    _logger = logger;
    _addEmitterFlags(EmitterFlags::kOwnLogger);
  }
  else {
    _logger = nullptr;
    _clearEmitterFlags(EmitterFlags::kOwnLogger);
    if (_code)
      _logger = _code->logger();
  }
  BaseEmitter_updateForcedOptions(this);
#else
  DebugUtils::unused(logger);
#endif
}

// BaseEmitter - Error Handling
// ============================

void BaseEmitter::setErrorHandler(ErrorHandler* errorHandler) noexcept {
  if (errorHandler) {
    _errorHandler = errorHandler;
    _addEmitterFlags(EmitterFlags::kOwnErrorHandler);
  }
  else {
    _errorHandler = nullptr;
    _clearEmitterFlags(EmitterFlags::kOwnErrorHandler);
    if (_code)
      _errorHandler = _code->errorHandler();
  }
}

Error BaseEmitter::reportError(Error err, const char* message) {
  ErrorHandler* eh = _errorHandler;
  if (eh) {
    if (!message)
      message = DebugUtils::errorAsString(err);
    eh->handleError(err, message, this);
  }
  return err;
}

// BaseEmitter - Events
// ====================

Error BaseEmitter::onAttach(CodeHolder* code) noexcept {
  _code = code;
  _environment = code->environment();
  _addEmitterFlags(EmitterFlags::kAttached);

  const ArchTraits& archTraits = ArchTraits::byArch(code->arch());
  RegType nativeRegType = Environment::is32Bit(code->arch()) ? RegType::kGp32 : RegType::kGp64;
  _gpSignature = archTraits.regTypeToSignature(nativeRegType);

  onSettingsUpdated();
  return kErrorOk;
}

Error BaseEmitter::onDetach(CodeHolder* code) noexcept {
  DebugUtils::unused(code);

  if (!hasOwnLogger())
    _logger = nullptr;

  if (!hasOwnErrorHandler())
    _errorHandler = nullptr;

  _clearEmitterFlags(~kEmitterPreservedFlags);
  _forcedInstOptions = InstOptions::kReserved;
  _privateData = 0;

  _environment.reset();
  //_gpSignature.reset();

  //_instOptions = InstOptions::kNone;
  //_extraReg.reset();
  //_inlineComment = nullptr;

  return kErrorOk;
}

void BaseEmitter::onSettingsUpdated() noexcept {
  // Only called when attached to CodeHolder by CodeHolder.
  ASMJIT_ASSERT(_code != nullptr);

  if (!hasOwnLogger())
    _logger = _code->logger();

  if (!hasOwnErrorHandler())
    _errorHandler = _code->errorHandler();

  BaseEmitter_updateForcedOptions(this);
}

ASMJIT_END_NAMESPACE

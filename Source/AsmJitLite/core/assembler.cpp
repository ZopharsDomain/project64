#include "../core/api-build_p.h"
#include "../core/assembler.h"
#include "../core/emitterutils_p.h"
#include "../core/logger.h"
#include "../core/support.h"

ASMJIT_BEGIN_NAMESPACE

// BaseAssembler - Construction & Destruction
// ==========================================

BaseAssembler::BaseAssembler() noexcept
  : BaseEmitter(EmitterType::kAssembler) {}

BaseAssembler::~BaseAssembler() noexcept {}

// BaseAssembler - Section Management
// ==================================

static void BaseAssembler_initSection(BaseAssembler* self, Section* section) noexcept {
  uint8_t* p = section->_buffer._data;

  self->_section = section;
  self->_bufferData = p;
  self->_bufferPtr  = p + section->_buffer._size;
  self->_bufferEnd  = p + section->_buffer._capacity;
}

// BaseAssembler - Label Management
// ================================

Label BaseAssembler::newLabel() {
  uint32_t labelId = Globals::kInvalidId;
  if (ASMJIT_LIKELY(_code)) {
    LabelEntry* le;
    Error err = _code->newLabelEntry(&le);
    if (ASMJIT_UNLIKELY(err))
      reportError(err);
    else
      labelId = le->id();
  }
  return Label(labelId);
}

Error BaseAssembler::bind(const Label& label) {
  if (ASMJIT_UNLIKELY(!_code))
    return reportError(DebugUtils::errored(kErrorNotInitialized));

  Error err = _code->bindLabel(label, _section->id(), offset());

#ifndef ASMJIT_NO_LOGGING
  if (_logger)
    EmitterUtils::logLabelBound(this, label);
#endif

  resetInlineComment();
  if (err)
    return reportError(err);

  return kErrorOk;
}

// BaseAssembler - Events
// ======================

Error BaseAssembler::onAttach(CodeHolder* code) noexcept {
  ASMJIT_PROPAGATE(Base::onAttach(code));

  // Attach to the end of the .text section.
  BaseAssembler_initSection(this, code->_sections[0]);

  return kErrorOk;
}

Error BaseAssembler::onDetach(CodeHolder* code) noexcept {
  _section    = nullptr;
  _bufferData = nullptr;
  _bufferEnd  = nullptr;
  _bufferPtr  = nullptr;
  return Base::onDetach(code);
}

ASMJIT_END_NAMESPACE

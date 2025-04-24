#pragma once

#include "../core/codeholder.h"
#include "../core/emitter.h"
#include "../core/operand.h"

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_assembler
//! \{

//! Base assembler.
//!
//! This is a base class that provides interface used by architecture specific
//! assembler implementations. Assembler doesn't hold any data, instead it's
//! attached to \ref CodeHolder, which provides all the data that Assembler
//! needs and which can be altered by it.
//!
//! Check out architecture specific assemblers for more details and examples:
//!
//!   - \ref x86::Assembler - X86/X64 assembler implementation.
class ASMJIT_VIRTAPI BaseAssembler : public BaseEmitter {
public:
  ASMJIT_NONCOPYABLE(BaseAssembler)
  typedef BaseEmitter Base;

  //! Current section where the assembling happens.
  Section* _section = nullptr;
  //! Start of the CodeBuffer of the current section.
  uint8_t* _bufferData = nullptr;
  //! End (first invalid byte) of the current section.
  uint8_t* _bufferEnd = nullptr;
  //! Pointer in the CodeBuffer of the current section.
  uint8_t* _bufferPtr = nullptr;

  //! \name Construction & Destruction
  //! \{

  //! Creates a new `BaseAssembler` instance.
  ASMJIT_API BaseAssembler() noexcept;
  //! Destroys the `BaseAssembler` instance.
  ASMJIT_API virtual ~BaseAssembler() noexcept;

  //! \}

  //! \name Code-Buffer Management
  //! \{

  //! Returns the capacity of the current CodeBuffer.
  inline size_t bufferCapacity() const noexcept { return (size_t)(_bufferEnd - _bufferData); }
  //! Returns the number of remaining bytes in the current CodeBuffer.
  inline size_t remainingSpace() const noexcept { return (size_t)(_bufferEnd - _bufferPtr); }

  //! Returns the current position in the CodeBuffer.
  inline size_t offset() const noexcept { return (size_t)(_bufferPtr - _bufferData); }

  //! Returns the start of the CodeBuffer in the current section.
  inline uint8_t* bufferData() const noexcept { return _bufferData; }
  //! Returns the end (first invalid byte) in the current section.
  inline uint8_t* bufferEnd() const noexcept { return _bufferEnd; }
  //! Returns the current pointer in the CodeBuffer in the current section.
  inline uint8_t* bufferPtr() const noexcept { return _bufferPtr; }

  ASMJIT_API Label newLabel() /*override*/;
  ASMJIT_API Error bind(const Label& label) /*override*/;
  //! \name Events
  //! \{

  ASMJIT_API Error onAttach(CodeHolder* code) noexcept override;
  ASMJIT_API Error onDetach(CodeHolder* code) noexcept override;

  //! \}
};

ASMJIT_END_NAMESPACE

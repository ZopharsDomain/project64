#pragma once

#include "../core/assembler.h"
#include "../core/support.h"

ASMJIT_BEGIN_NAMESPACE

class CodeWriter {
public:
  uint8_t* _cursor;

  ASMJIT_FORCE_INLINE explicit CodeWriter(BaseAssembler* a) noexcept
    : _cursor(a->_bufferPtr) {}

  ASMJIT_FORCE_INLINE Error ensureSpace(BaseAssembler* a, size_t n) noexcept {
    size_t remainingSpace = (size_t)(a->_bufferEnd - _cursor);
    if (ASMJIT_UNLIKELY(remainingSpace < n)) {
      CodeBuffer& buffer = a->_section->_buffer;
      Error err = a->_code->growBuffer(&buffer, n);
      if (ASMJIT_UNLIKELY(err))
        return a->reportError(err);
      _cursor = a->_bufferPtr;
    }
    return kErrorOk;
  }

  ASMJIT_FORCE_INLINE size_t offsetFrom(uint8_t* from) const noexcept {
    ASMJIT_ASSERT(_cursor >= from);
    return (size_t)(_cursor - from);
  }

  template<typename T>
  ASMJIT_FORCE_INLINE void emit8(T val) noexcept {
    typedef typename std::make_unsigned<T>::type U;
    _cursor[0] = uint8_t(U(val) & U(0xFF));
    _cursor++;
    }

  template<typename T>
  ASMJIT_FORCE_INLINE void emit16(T val) noexcept {
      typedef typename std::make_unsigned<T>::type U;
      *((uint16_t *)&_cursor[0]) = uint16_t(U(val) & U(0xFFFF));
      _cursor += 2;
  }

  template<typename T>
  ASMJIT_FORCE_INLINE void emit32(T val) noexcept {
      typedef typename std::make_unsigned<T>::type U;
      *((uint32_t *)&_cursor[0]) = uint32_t(U(val) & U(0xFFFFFFFF));
      _cursor += 4;
  }

  ASMJIT_FORCE_INLINE void done(BaseAssembler* a) noexcept {
    CodeBuffer& buffer = a->_section->_buffer;
    size_t newSize = (size_t)(_cursor - a->_bufferData);
    ASMJIT_ASSERT(newSize <= buffer.capacity());

    a->_bufferPtr = _cursor;
    buffer._size = Support::max(buffer._size, newSize);
  }
};

//! Code writer utilities.
namespace CodeWriterUtils {

bool encodeOffset32(uint32_t* dst, int64_t offset64, const OffsetFormat& format) noexcept;
bool encodeOffset64(uint64_t* dst, int64_t offset64, const OffsetFormat& format) noexcept;

bool writeOffset(void* dst, int64_t offset64, const OffsetFormat& format) noexcept;

} // {CodeWriterUtils}

//! \}
//! \endcond

ASMJIT_END_NAMESPACE

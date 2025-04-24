#include "../core/api-build_p.h"
#include "../core/string.h"
#include "../core/support.h"

ASMJIT_BEGIN_NAMESPACE

// String - Globals
// ================

static const char String_baseN[] = "0123456789ABCDEF";

constexpr size_t kMinAllocSize = 64;
constexpr size_t kMaxAllocSize = SIZE_MAX - Globals::kGrowThreshold;

// String - Clear & Reset
// ======================

Error String::reset() noexcept {
  if (_type == kTypeLarge)
    ::free(_large.data);

  _resetInternal();
  return kErrorOk;
}

// String - Prepare
// ================

char* String::prepare(ModifyOp op, size_t size) noexcept {
  char* curData;
  size_t curSize;
  size_t curCapacity;

  if (isLargeOrExternal()) {
    curData = this->_large.data;
    curSize = this->_large.size;
    curCapacity = this->_large.capacity;
  }
  else {
    curData = this->_small.data;
    curSize = this->_small.type;
    curCapacity = kSSOCapacity;
  }

  if (op == ModifyOp::kAssign) {
    if (size > curCapacity) {
      // Prevent arithmetic overflow.
      if (ASMJIT_UNLIKELY(size >= kMaxAllocSize))
        return nullptr;

      size_t newCapacity = Support::alignUp<size_t>(size + 1, kMinAllocSize);
      char* newData = static_cast<char*>(::malloc(newCapacity));

      if (ASMJIT_UNLIKELY(!newData))
        return nullptr;

      if (_type == kTypeLarge)
        ::free(curData);

      _large.type = kTypeLarge;
      _large.size = size;
      _large.capacity = newCapacity - 1;
      _large.data = newData;

      newData[size] = '\0';
      return newData;
    }
    else {
      _setSize(size);
      curData[size] = '\0';
      return curData;
    }
  }
  else {
    // Prevent arithmetic overflow.
    if (ASMJIT_UNLIKELY(size >= kMaxAllocSize - curSize))
      return nullptr;

    size_t newSize = size + curSize;
    size_t newSizePlusOne = newSize + 1;

    if (newSizePlusOne > curCapacity) {
      size_t newCapacity = Support::max<size_t>(curCapacity + 1, kMinAllocSize);

      if (newCapacity < newSizePlusOne && newCapacity < Globals::kGrowThreshold)
        newCapacity = Support::alignUpPowerOf2(newCapacity);

      if (newCapacity < newSizePlusOne)
        newCapacity = Support::alignUp(newSizePlusOne, Globals::kGrowThreshold);

      if (ASMJIT_UNLIKELY(newCapacity < newSizePlusOne))
        return nullptr;

      char* newData = static_cast<char*>(::malloc(newCapacity));
      if (ASMJIT_UNLIKELY(!newData))
        return nullptr;

      memcpy(newData, curData, curSize);

      if (_type == kTypeLarge)
        ::free(curData);

      _large.type = kTypeLarge;
      _large.size = newSize;
      _large.capacity = newCapacity - 1;
      _large.data = newData;

      newData[newSize] = '\0';
      return newData + curSize;
    }
    else {
      _setSize(newSize);
      curData[newSize] = '\0';
      return curData + curSize;
    }
  }
}

// String - Operations
// ===================

Error String::_opString(ModifyOp op, const char* str, size_t size) noexcept {
  if (size == SIZE_MAX)
    size = str ? strlen(str) : size_t(0);

  if (!size)
    return kErrorOk;

  char* p = prepare(op, size);
  if (!p)
    return DebugUtils::errored(kErrorOutOfMemory);

  memcpy(p, str, size);
  return kErrorOk;
}

Error String::_opChar(ModifyOp op, char c) noexcept {
  char* p = prepare(op, 1);
  if (!p)
    return DebugUtils::errored(kErrorOutOfMemory);

  *p = c;
  return kErrorOk;
}

Error String::_opChars(ModifyOp op, char c, size_t n) noexcept {
  if (!n)
    return kErrorOk;

  char* p = prepare(op, n);
  if (!p)
    return DebugUtils::errored(kErrorOutOfMemory);

  memset(p, c, n);
  return kErrorOk;
}

Error String::padEnd(size_t n, char c) noexcept {
  size_t size = this->size();
  return n > size ? appendChars(c, n - size) : kErrorOk;
}

Error String::_opHex(ModifyOp op, const void* data, size_t size, char separator) noexcept {
  char* dst;
  const uint8_t* src = static_cast<const uint8_t*>(data);

  if (!size)
    return kErrorOk;

  if (separator) {
    if (ASMJIT_UNLIKELY(size >= SIZE_MAX / 3))
      return DebugUtils::errored(kErrorOutOfMemory);

    dst = prepare(op, size * 3 - 1);
    if (ASMJIT_UNLIKELY(!dst))
      return DebugUtils::errored(kErrorOutOfMemory);

    size_t i = 0;
    for (;;) {
      dst[0] = String_baseN[(src[0] >> 4) & 0xF];
      dst[1] = String_baseN[(src[0]     ) & 0xF];
      if (++i == size)
        break;
      // This makes sure that the separator is only put between two hexadecimal bytes.
      dst[2] = separator;
      dst += 3;
      src++;
    }
  }
  else {
    if (ASMJIT_UNLIKELY(size >= SIZE_MAX / 2))
      return DebugUtils::errored(kErrorOutOfMemory);

    dst = prepare(op, size * 2);
    if (ASMJIT_UNLIKELY(!dst))
      return DebugUtils::errored(kErrorOutOfMemory);

    for (size_t i = 0; i < size; i++, dst += 2, src++) {
      dst[0] = String_baseN[(src[0] >> 4) & 0xF];
      dst[1] = String_baseN[(src[0]     ) & 0xF];
    }
  }

  return kErrorOk;
}

Error String::_opFormat(ModifyOp op, const char* fmt, ...) noexcept {
  Error err;
  va_list ap;

  va_start(ap, fmt);
  err = _opVFormat(op, fmt, ap);
  va_end(ap);

  return err;
}

Error String::_opVFormat(ModifyOp op, const char* fmt, va_list ap) noexcept {
  size_t startAt = (op == ModifyOp::kAssign) ? size_t(0) : size();
  size_t remainingCapacity = capacity() - startAt;

  char buf[1024];
  int fmtResult;
  size_t outputSize;

  va_list apCopy;
  va_copy(apCopy, ap);

  if (remainingCapacity >= 128) {
    fmtResult = vsnprintf(data() + startAt, remainingCapacity, fmt, ap);
    outputSize = size_t(fmtResult);

    if (ASMJIT_LIKELY(outputSize <= remainingCapacity)) {
      _setSize(startAt + outputSize);
      return kErrorOk;
    }
  }
  else {
    fmtResult = vsnprintf(buf, ASMJIT_ARRAY_SIZE(buf), fmt, ap);
    outputSize = size_t(fmtResult);

    if (ASMJIT_LIKELY(outputSize < ASMJIT_ARRAY_SIZE(buf)))
      return _opString(op, buf, outputSize);
  }

  if (ASMJIT_UNLIKELY(fmtResult < 0))
    return DebugUtils::errored(kErrorInvalidState);

  char* p = prepare(op, outputSize);
  if (ASMJIT_UNLIKELY(!p))
    return DebugUtils::errored(kErrorOutOfMemory);

  fmtResult = vsnprintf(p, outputSize + 1, fmt, apCopy);
  ASMJIT_ASSERT(size_t(fmtResult) == outputSize);

  return kErrorOk;
}

ASMJIT_END_NAMESPACE

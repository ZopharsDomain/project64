#include "../core/api-build_p.h"
#include "../core/support.h"
#include "../core/zone.h"
#include "../core/zonevector.h"
#include <limits>

ASMJIT_BEGIN_NAMESPACE

Error ZoneVectorBase::_grow(ZoneAllocator* allocator, uint32_t sizeOfT, uint32_t n) noexcept {
  uint32_t threshold = Globals::kGrowThreshold / sizeOfT;
  uint32_t capacity = _capacity;
  uint32_t after = _size;

  if (ASMJIT_UNLIKELY(std::numeric_limits<uint32_t>::max() - n < after))
    return DebugUtils::errored(kErrorOutOfMemory);

  after += n;
  if (capacity >= after)
    return kErrorOk;

  // ZoneVector is used as an array to hold short-lived data structures used
  // during code generation. The growing strategy is simple - use small capacity
  // at the beginning (very good for ZoneAllocator) and then grow quicker to
  // prevent successive reallocations.
  if (capacity < 4)
    capacity = 4;
  else if (capacity < 8)
    capacity = 8;
  else if (capacity < 16)
    capacity = 16;
  else if (capacity < 64)
    capacity = 64;
  else if (capacity < 256)
    capacity = 256;

  while (capacity < after) {
    if (capacity < threshold)
      capacity *= 2;
    else
      capacity += threshold;
  }

  return _reserve(allocator, sizeOfT, capacity);
}

Error ZoneVectorBase::_reserve(ZoneAllocator* allocator, uint32_t sizeOfT, uint32_t n) noexcept {
  uint32_t oldCapacity = _capacity;
  if (oldCapacity >= n) return kErrorOk;

  uint32_t nBytes = n * sizeOfT;
  if (ASMJIT_UNLIKELY(nBytes < n))
    return DebugUtils::errored(kErrorOutOfMemory);

  size_t allocatedBytes;
  uint8_t* newData = static_cast<uint8_t*>(allocator->alloc(nBytes, allocatedBytes));

  if (ASMJIT_UNLIKELY(!newData))
    return DebugUtils::errored(kErrorOutOfMemory);

  void* oldData = _data;
  if (_size)
    memcpy(newData, oldData, size_t(_size) * sizeOfT);

  if (oldData)
    allocator->release(oldData, size_t(oldCapacity) * sizeOfT);

  _capacity = uint32_t(allocatedBytes / sizeOfT);
  ASMJIT_ASSERT(_capacity >= n);

  _data = newData;
  return kErrorOk;
}

ASMJIT_END_NAMESPACE

#pragma once
#include "../core/support.h"
#include "../core/zone.h"

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_zone
//! \{

//! Base class used by \ref ZoneVector template.
class ZoneVectorBase {
public:
  ASMJIT_NONCOPYABLE(ZoneVectorBase)

  // STL compatibility;
  typedef uint32_t size_type;

  //! Vector data (untyped).
  void* _data = nullptr;
  //! Size of the vector.
  size_type _size = 0;
  //! Capacity of the vector.
  size_type _capacity = 0;

protected:
  //! \name Construction & Destruction
  //! \{

  //! Creates a new instance of `ZoneVectorBase`.
  inline ZoneVectorBase() noexcept {}
  ASMJIT_API Error _grow(ZoneAllocator* allocator, uint32_t sizeOfT, uint32_t n) noexcept;
  ASMJIT_API Error _reserve(ZoneAllocator* allocator, uint32_t sizeOfT, uint32_t n) noexcept;

  //! \}
  //! \endcond

public:
  //! \name Accessors
  //! \{

  //! Tests whether the vector is empty.
  inline bool empty() const noexcept { return _size == 0; }
  //! Returns the vector size.
  inline size_type size() const noexcept { return _size; }
  //! Returns the vector capacity.
  inline size_type capacity() const noexcept { return _capacity; }

  //! \}

  //! \name Utilities
  //! \{

  //! Resets the vector data and set its `size` to zero.
  inline void reset() noexcept {
    _data = nullptr;
    _size = 0;
    _capacity = 0;
  }

  //! \}
};

//! Template used to store and manage array of Zone allocated data.
//!
//! This template has these advantages over other std::vector<>:
//! - Always non-copyable (designed to be non-copyable, we want it).
//! - Optimized for working only with POD types.
//! - Uses ZoneAllocator, thus small vectors are almost for free.
//! - Explicit allocation, ZoneAllocator is not part of the data.
template <typename T>
class ZoneVector : public ZoneVectorBase {
public:
  ASMJIT_NONCOPYABLE(ZoneVector)

  // STL compatibility;
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  typedef T* iterator;
  typedef const T* const_iterator;

  //! \name Construction & Destruction
  //! \{

  inline ZoneVector() noexcept : ZoneVectorBase() {}

  //! \}

  //! \name Accessors
  //! \{

  //! Returns vector data.
  inline T* data() noexcept { return static_cast<T*>(_data); }
  //! Returns vector data (const)
  inline const T* data() const noexcept { return static_cast<const T*>(_data); }
  //! \name STL Compatibility (Iterators)
  //! \{

  inline iterator begin() noexcept { return iterator(data()); };
  inline const_iterator begin() const noexcept { return const_iterator(data()); };

  inline iterator end() noexcept { return iterator(data() + _size); };
  inline const_iterator end() const noexcept { return const_iterator(data() + _size); };

  //! Append s`item` to the vector (unsafe case).
  //!
  //! Can only be used together with `willGrow()`. If `willGrow(N)` returns `kErrorOk` then N elements
  //! can be added to the vector without checking if there is a place for them. Used mostly internally.
  ASMJIT_FORCE_INLINE void appendUnsafe(const T& item) noexcept {
    ASMJIT_ASSERT(_size < _capacity);

    memcpy(static_cast<T*>(_data) + _size, &item, sizeof(T));
    _size++;
  }

  //! Returns index of the given `val` or `Globals::kNotFound` if it doesn't exist.
  ASMJIT_FORCE_INLINE uint32_t indexOf(const T& val) const noexcept {
    const T* data = static_cast<const T*>(_data);
    uint32_t size = _size;

    for (uint32_t i = 0; i < size; i++)
      if (data[i] == val)
        return i;
    return Globals::kNotFound;
  }

  //! Removes item at index `i`.
  inline void removeAt(size_t i) noexcept {
    ASMJIT_ASSERT(i < _size);

    T* data = static_cast<T*>(_data) + i;
    size_t size = --_size - i;

    if (size)
      ::memmove(data, data + 1, size_t(size) * sizeof(T));
  }
  //! Returns item at index `i`.
  inline T& operator[](size_t i) noexcept {
    ASMJIT_ASSERT(i < _size);
    return data()[i];
  }

  //! Returns item at index `i`.
  inline const T& operator[](size_t i) const noexcept {
    ASMJIT_ASSERT(i < _size);
    return data()[i];
  }

  //! Returns a reference to the last element of the vector.
  //!
  //! \note The vector must have at least one element. Attempting to use `last()` on empty vector will trigger
  //! an assertion failure in debug builds.
  inline T& last() noexcept { return operator[](_size - 1); }
  //! \overload
  inline const T& last() const noexcept { return operator[](_size - 1); }

  inline Error grow(ZoneAllocator* allocator, uint32_t n) noexcept {
    return ZoneVectorBase::_grow(allocator, sizeof(T), n);
  }

  inline Error willGrow(ZoneAllocator* allocator, uint32_t n = 1) noexcept {
    return _capacity - _size < n ? grow(allocator, n) : Error(kErrorOk);
  }
};

ASMJIT_END_NAMESPACE

#pragma once

#include "../core/support.h"
#include <stdio.h>
#include <string.h>

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_zone
//! \{

//! Zone memory.
//!
//! Zone is an incremental memory allocator that allocates memory by simply incrementing a pointer. It allocates
//! blocks of memory by using C's `malloc()`, but divides these blocks into smaller segments requested by calling
//! `Zone::alloc()` and friends.
//!
//! Zone has no function to release the allocated memory. It has to be released all at once by calling `reset()`.
//! If you need a more friendly allocator that also supports `release()`, consider using `Zone` with `ZoneAllocator`.
class Zone {
public:
  ASMJIT_NONCOPYABLE(Zone)

  //! \cond INTERNAL

  //! A single block of memory managed by `Zone`.
  struct Block {
    inline uint8_t* data() const noexcept {
      return const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(this) + sizeof(*this));
    }

    //! Link to the previous block.
    Block* prev;
    //! Link to the next block.
    Block* next;
    //! Size of the block.
    size_t size;
  };

  enum Limits : size_t {
    kBlockSize = sizeof(Block),
    kBlockOverhead = Globals::kAllocOverhead + kBlockSize,

    kMinBlockSize = 64, // The number is ridiculously small, but still possible.
    kMaxBlockSize = size_t(1) << (sizeof(size_t) * 8 - 4 - 1),
    kMinAlignment = 1,
    kMaxAlignment = 64
  };

  //! Pointer in the current block.
  uint8_t* _ptr;
  //! End of the current block.
  uint8_t* _end;
  //! Current block.
  Block* _block;

  union {
    struct {
      //! Default block size.
      size_t _blockSize : Support::bitSizeOf<size_t>() - 4;
      //! First block is temporary (ZoneTmp).
      size_t _isTemporary : 1;
      //! Block alignment (1 << alignment).
      size_t _blockAlignmentShift : 3;
    };
    size_t _packedData;
  };

  static ASMJIT_API const Block _zeroBlock;
  //! Creates a new Zone with a first block pointing to a `temporary` memory.
  inline Zone(size_t blockSize, size_t blockAlignment, const Support::Temporary& temporary) noexcept {
    _init(blockSize, blockAlignment, &temporary);
  }

  //! \overload
  inline Zone(size_t blockSize, size_t blockAlignment, const Support::Temporary* temporary) noexcept {
    _init(blockSize, blockAlignment, temporary);
  }
  //! Destroys the `Zone` instance.
  //!
  //! This will destroy the `Zone` instance and release all blocks of memory allocated by it. It performs implicit
  //! `reset(ResetPolicy::kHard)`.
  inline ~Zone() noexcept { reset(ResetPolicy::kHard); }

  ASMJIT_API void _init(size_t blockSize, size_t blockAlignment, const Support::Temporary* temporary) noexcept;

  //! Resets the `Zone` invalidating all blocks allocated.
  //!
  //! See `Globals::ResetPolicy` for more details.
  ASMJIT_API void reset(ResetPolicy resetPolicy = ResetPolicy::kSoft) noexcept;
  //! Returns the default block size.
  inline size_t blockSize() const noexcept { return _blockSize; }
  //! Returns the default block alignment.
  inline size_t blockAlignment() const noexcept { return size_t(1) << _blockAlignmentShift; }

  //! Returns the current zone cursor (dangerous).
  //!
  //! This is a function that can be used to get exclusive access to the current block's memory buffer.
  template<typename T = uint8_t>
  inline T* ptr() noexcept { return reinterpret_cast<T*>(_ptr); }

  //! Returns the end of the current zone block, only useful if you use `ptr()`.
  template<typename T = uint8_t>
  inline T* end() noexcept { return reinterpret_cast<T*>(_end); }

  //! Sets the current zone pointer to `ptr` (must be within the current block).
  template<typename T>
  inline void setPtr(T* ptr) noexcept {
    uint8_t* p = reinterpret_cast<uint8_t*>(ptr);
    ASMJIT_ASSERT(p >= _ptr && p <= _end);
    _ptr = p;
  }
  //! Aligns the current pointer to `alignment`.
  inline void align(size_t alignment) noexcept {
    _ptr = Support::min(Support::alignUp(_ptr, alignment), _end);
  }

  inline void _assignBlock(Block* block) noexcept {
    size_t alignment = blockAlignment();
    _ptr = Support::alignUp(block->data(), alignment);
    _end = Support::alignDown(block->data() + block->size, alignment);
    _block = block;
  }

  inline void _assignZeroBlock() noexcept {
    Block* block = const_cast<Block*>(&_zeroBlock);
    _ptr = block->data();
    _end = block->data();
    _block = block;
  }


  //! Allocates `size` bytes of zeroed memory. See `alloc()` for more details.
  ASMJIT_API void* allocZeroed(size_t size, size_t alignment = 1) noexcept;

  //! Like `alloc()`, but the return pointer is casted to `T*`.
  template<typename T>
  inline T* allocT(size_t size = sizeof(T), size_t alignment = alignof(T)) noexcept {
    return static_cast<T*>(alloc(size, alignment));
  }
  //! Like `allocZeroed()`, but the return pointer is casted to `T*`.
  template<typename T>
  inline T* allocZeroedT(size_t size = sizeof(T), size_t alignment = alignof(T)) noexcept {
    return static_cast<T*>(allocZeroed(size, alignment));
  }
  //! \cond INTERNAL
  //!
  //! Internal alloc function used by other inlines.
  ASMJIT_API void* _alloc(size_t size, size_t alignment) noexcept;
  //! \endcond

  //! Helper to duplicate data.
  ASMJIT_API void* dup(const void* data, size_t size, bool nullTerminate = false) noexcept;
};

//! Zone-based memory allocator that uses an existing `Zone` and provides a `release()` functionality on top of it.
//! It uses `Zone` only for chunks that can be pooled, and uses libc `malloc()` for chunks that are large.
//!
//! The advantage of ZoneAllocator is that it can allocate small chunks of memory really fast, and these chunks,
//! when released, will be reused by consecutive calls to `alloc()`. Also, since ZoneAllocator uses `Zone`, you can
//! turn any `Zone` into a `ZoneAllocator`, and use it in your `Pass` when necessary.
//!
//! ZoneAllocator is used by AsmJit containers to make containers having only few elements fast (and lightweight)
//! and to allow them to grow and use dynamic blocks when require more storage.
class ZoneAllocator {
public:
  ASMJIT_NONCOPYABLE(ZoneAllocator)

  //! \cond INTERNAL

  // In short, we pool chunks of these sizes:
  //   [32, 64, 96, 128, 192, 256, 320, 384, 448, 512]

  enum : uint32_t {
    //! How many bytes per a low granularity pool (has to be at least 16).
    kLoGranularity = 32,
    //! Number of slots of a low granularity pool.
    kLoCount = 4,
    //! Maximum size of a block that can be allocated in a low granularity pool.
    kLoMaxSize = kLoGranularity * kLoCount,

    //! How many bytes per a high granularity pool.
    kHiGranularity = 64,
    //! Number of slots of a high granularity pool.
    kHiCount = 6,
    //! Maximum size of a block that can be allocated in a high granularity pool.
    kHiMaxSize = kLoMaxSize + kHiGranularity * kHiCount,

    //! Alignment of every pointer returned by `alloc()`.
    kBlockAlignment = kLoGranularity
  };

  //! Single-linked list used to store unused chunks.
  struct Slot {
    //! Link to a next slot in a single-linked list.
    Slot* next;
  };

  //! A block of memory that has been allocated dynamically and is not part of block-list used by the allocator.
  //! This is used to keep track of all these blocks so they can be freed by `reset()` if not freed explicitly.
  struct DynamicBlock {
    DynamicBlock* prev;
    DynamicBlock* next;
  };

  //! \endcond

  //! \name Members
  //! \{

  //! Zone used to allocate memory that fits into slots.
  Zone* _zone;
  //! Indexed slots containing released memory.
  Slot* _slots[kLoCount + kHiCount];
  //! Dynamic blocks for larger allocations (no slots).
  DynamicBlock* _dynamicBlocks;
  //! \note To use it, you must first `init()` it.
  inline ZoneAllocator() noexcept {
    memset(this, 0, sizeof(*this));
  }

  //! Creates a new `ZoneAllocator` initialized to use `zone`.
  inline explicit ZoneAllocator(Zone* zone) noexcept {
    memset(this, 0, sizeof(*this));
    _zone = zone;
  }

  //! Destroys the `ZoneAllocator`.
  inline ~ZoneAllocator() noexcept { reset(); }

  //! Tests whether the `ZoneAllocator` is initialized (i.e. has `Zone`).
  inline bool isInitialized() const noexcept { return _zone != nullptr; }

  //! Resets this `ZoneAllocator` and also forget about the current `Zone` which is attached (if any). Reset
  //! optionally attaches a new `zone` passed, or keeps the `ZoneAllocator` in an uninitialized state, if
  //! `zone` is null.
  ASMJIT_API void reset(Zone* zone = nullptr) noexcept;
  //! \cond
  //! \name Internals
  //! \{

  //! Returns the slot index to be used for `size`. Returns `true` if a valid slot has been written to `slot` and
  //! `allocatedSize` has been filled with slot exact size (`allocatedSize` can be equal or slightly greater than
  //! `size`).
  static inline bool _getSlotIndex(size_t size, uint32_t& slot) noexcept {
    ASMJIT_ASSERT(size > 0);
    if (size > kHiMaxSize)
      return false;

    if (size <= kLoMaxSize)
      slot = uint32_t((size - 1) / kLoGranularity);
    else
      slot = uint32_t((size - kLoMaxSize - 1) / kHiGranularity) + kLoCount;

    return true;
  }

  //! \overload
  static inline bool _getSlotIndex(size_t size, uint32_t& slot, size_t& allocatedSize) noexcept {
    ASMJIT_ASSERT(size > 0);
    if (size > kHiMaxSize)
      return false;

    if (size <= kLoMaxSize) {
      slot = uint32_t((size - 1) / kLoGranularity);
      allocatedSize = Support::alignUp(size, kLoGranularity);
    }
    else {
      slot = uint32_t((size - kLoMaxSize - 1) / kHiGranularity) + kLoCount;
      allocatedSize = Support::alignUp(size, kHiGranularity);
    }

    return true;
  }

  //! \}
  //! \endcond

  //! \name Allocation
  //! \{

  //! \cond INTERNAL
  ASMJIT_API void* _alloc(size_t size, size_t& allocatedSize) noexcept;
  ASMJIT_API void* _allocZeroed(size_t size, size_t& allocatedSize) noexcept;
  ASMJIT_API void _releaseDynamic(void* p, size_t size) noexcept;
  //! \endcond

  //! Allocates `size` bytes of memory, ideally from an available pool.
  //!
  //! \note `size` can't be zero, it will assert in debug mode in such case.
  inline void* alloc(size_t size) noexcept {
    ASMJIT_ASSERT(isInitialized());
    size_t allocatedSize;
    return _alloc(size, allocatedSize);
  }

  //! Like `alloc(size)`, but provides a second argument `allocatedSize` that provides a way to know how big
  //! the block returned actually is. This is useful for containers to prevent growing too early.
  inline void* alloc(size_t size, size_t& allocatedSize) noexcept {
    ASMJIT_ASSERT(isInitialized());
    return _alloc(size, allocatedSize);
  }

  //! Like `alloc()`, but the return pointer is casted to `T*`.
  template<typename T>
  inline T* allocT(size_t size = sizeof(T)) noexcept {
    return static_cast<T*>(alloc(size));
  }

  //! Like `alloc(size)`, but returns zeroed memory.
  inline void* allocZeroed(size_t size) noexcept {
    ASMJIT_ASSERT(isInitialized());
    size_t allocatedSize;
    return _allocZeroed(size, allocatedSize);
  }

  //! Like `alloc(size, allocatedSize)`, but returns zeroed memory.
  inline void* allocZeroed(size_t size, size_t& allocatedSize) noexcept {
    ASMJIT_ASSERT(isInitialized());
    return _allocZeroed(size, allocatedSize);
  }

  //! Like `allocZeroed()`, but the return pointer is casted to `T*`.
  template<typename T>
  inline T* allocZeroedT(size_t size = sizeof(T)) noexcept {
    return static_cast<T*>(allocZeroed(size));
  }

  //! Releases the memory previously allocated by `alloc()`. The `size` argument has to be the same as used to call
  //! `alloc()` or `allocatedSize` returned  by `alloc()`.
  inline void release(void* p, size_t size) noexcept {
    ASMJIT_ASSERT(isInitialized());
    ASMJIT_ASSERT(p != nullptr);
    ASMJIT_ASSERT(size != 0);

    uint32_t slot;
    if (_getSlotIndex(size, slot)) {
      static_cast<Slot*>(p)->next = static_cast<Slot*>(_slots[slot]);
      _slots[slot] = static_cast<Slot*>(p);
    }
    else {
      _releaseDynamic(p, size);
    }
  }

  //! \}
};

ASMJIT_END_NAMESPACE

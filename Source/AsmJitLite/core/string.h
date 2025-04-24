#pragma once
#include "../core/support.h"

ASMJIT_BEGIN_NAMESPACE
//! Fixed string - only useful for strings that would never exceed `N - 1` characters; always null-terminated.
template<size_t N>
union FixedString {
  //! \name Constants
  //! \{

  // This cannot be constexpr as GCC 4.8 refuses constexpr members of unions.
  enum : uint32_t {
    kNumUInt32Words = uint32_t((N + sizeof(uint32_t) - 1) / sizeof(uint32_t))
  };

  //! \}

  //! \name Members
  //! \{

  char str[kNumUInt32Words * sizeof(uint32_t)];
  uint32_t u32[kNumUInt32Words];

  //! \}

  //! \name Utilities
  //! \{

  inline bool eq(const char* other) const noexcept {
    return strcmp(str, other) == 0;
  }

  //! \}
};

//! A simple non-reference counted string that uses small string optimization (SSO).
//!
//! This string has 3 allocation possibilities:
//!
//!   1. Small    - embedded buffer is used for up to `kSSOCapacity` characters. This should handle most small
//!                 strings and thus avoid dynamic memory allocation for most use-cases.
//!
//!   2. Large    - string that doesn't fit into an embedded buffer (or string that was truncated from a larger
//!                 buffer) and is owned by AsmJit. When you destroy the string AsmJit would automatically
//!                 release the large buffer.
//!
//!   3. External - like Large (2), however, the large buffer is not owned by AsmJit and won't be released when
//!                 the string is destroyed or reallocated. This is mostly useful for working with larger temporary
//!                 strings allocated on stack or with immutable strings.
class String {
public:
  ASMJIT_NONCOPYABLE(String)

  //! String operation.
  enum class ModifyOp : uint32_t {
    //! Assignment - a new content replaces the current one.
    kAssign = 0,
    //! Append - a new content is appended to the string.
    kAppend = 1
  };

  //! \cond INTERNAL
  enum : uint32_t {
    kLayoutSize = 32,
    kSSOCapacity = kLayoutSize - 2
  };

  //! String type.
  enum Type : uint8_t {
    //! Large string (owned by String).
    kTypeLarge = 0x1Fu,
    //! External string (zone allocated or not owned by String).
    kTypeExternal = 0x20u
  };

  union Raw {
    uint8_t u8[kLayoutSize];
    uint64_t u64[kLayoutSize / sizeof(uint64_t)];
    uintptr_t uptr[kLayoutSize / sizeof(uintptr_t)];
  };

  struct Small {
    uint8_t type;
    char data[kSSOCapacity + 1u];
  };

  struct Large {
    uint8_t type;
    uint8_t reserved[sizeof(uintptr_t) - 1];
    size_t size;
    size_t capacity;
    char* data;
  };

  union {
    uint8_t _type;
    Raw _raw;
    Small _small;
    Large _large;
  };
  //! \endcond

  //! \name Construction & Destruction
  //! \{

  //! Creates a default-initialized string if zero length.
  inline String() noexcept
    : _small {} {}

  //! Creates a string that takes ownership of the content of the `other` string.
  inline String(String&& other) noexcept {
    _raw = other._raw;
    other._resetInternal();
  }

  inline ~String() noexcept {
    reset();
  }

  //! Reset the string into a construction state.
  ASMJIT_API Error reset() noexcept;

  //! \}

  //! \name Accessors
  //! \{

  inline bool isExternal() const noexcept { return _type == kTypeExternal; }
  inline bool isLargeOrExternal() const noexcept { return _type >= kTypeLarge; }

  //! Returns the size of the string.
  inline size_t size() const noexcept { return isLargeOrExternal() ? size_t(_large.size) : size_t(_type); }
  //! Returns the capacity of the string.
  inline size_t capacity() const noexcept { return isLargeOrExternal() ? _large.capacity : size_t(kSSOCapacity); }

  //! Returns the data of the string.
  inline char* data() noexcept { return isLargeOrExternal() ? _large.data : _small.data; }
  //! \overload
  inline const char* data() const noexcept { return isLargeOrExternal() ? _large.data : _small.data; }
  ASMJIT_API char* prepare(ModifyOp op, size_t size) noexcept;

  ASMJIT_API Error _opString(ModifyOp op, const char* str, size_t size = SIZE_MAX) noexcept;
  ASMJIT_API Error _opChar(ModifyOp op, char c) noexcept;
  ASMJIT_API Error _opChars(ModifyOp op, char c, size_t n) noexcept;
  ASMJIT_API Error _opHex(ModifyOp op, const void* data, size_t size, char separator = '\0') noexcept;
  ASMJIT_API Error _opFormat(ModifyOp op, const char* fmt, ...) noexcept;
  ASMJIT_API Error _opVFormat(ModifyOp op, const char* fmt, va_list ap) noexcept;

  //! Appends `str` having the given size `size` to the string.
  //!
  //! Null terminated strings can set `size` to `SIZE_MAX`.
  inline Error append(const char* str, size_t size = SIZE_MAX) noexcept {
    return _opString(ModifyOp::kAppend, str, size);
  }

  //! Appends a single `c` character.
  inline Error append(char c) noexcept {
    return _opChar(ModifyOp::kAppend, c);
  }

  //! Appends `c` character repeated `n` times.
  inline Error appendChars(char c, size_t n) noexcept {
    return _opChars(ModifyOp::kAppend, c, n);
  }

  //! Appends the given `data` converted to a HEX string.
  inline Error appendHex(const void* data, size_t size, char separator = '\0') noexcept {
    return _opHex(ModifyOp::kAppend, data, size, separator);
  }

  //! Appends a formatted string `fmt` with `args`.
  template<typename... Args>
  inline Error appendFormat(const char* fmt, Args&&... args) noexcept {
    return _opFormat(ModifyOp::kAppend, fmt, std::forward<Args>(args)...);
  }

  ASMJIT_API Error padEnd(size_t n, char c = ' ') noexcept;

  //! \name Internal Functions
  //! \{

  //! Resets string to embedded and makes it empty (zero length, zero first char)
  //!
  //! \note This is always called internally after an external buffer was released as it zeroes all bytes
  //! used by String's embedded storage.
  inline void _resetInternal() noexcept {
    for (size_t i = 0; i < ASMJIT_ARRAY_SIZE(_raw.uptr); i++)
      _raw.uptr[i] = 0;
  }

  inline void _setSize(size_t newSize) noexcept {
    if (isLargeOrExternal())
      _large.size = newSize;
    else
      _small.type = uint8_t(newSize);
  }

  //! \}
};

//! Temporary string builder, has statically allocated `N` bytes.
template<size_t N>
class StringTmp : public String {
public:
  ASMJIT_NONCOPYABLE(StringTmp)

  //! Embedded data.
  char _embeddedData[Support::alignUp(N + 1, sizeof(size_t))];

  //! \name Construction & Destruction
  //! \{

  inline StringTmp() noexcept {
    _resetToTemporary();
  }

  inline void _resetToTemporary() noexcept {
    _large.type = kTypeExternal;
    _large.capacity = ASMJIT_ARRAY_SIZE(_embeddedData) - 1;
    _large.data = _embeddedData;
    _embeddedData[0] = '\0';
  }

  //! \}
};

//! \}

ASMJIT_END_NAMESPACE

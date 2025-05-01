// This file is part of AsmJit project <https://asmjit.com>
//
// See asmjit.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#ifndef ASMJIT_CORE_SUPPORT_H_INCLUDED
#define ASMJIT_CORE_SUPPORT_H_INCLUDED

#include "../core/globals.h"

#if defined(_MSC_VER)
  #include <intrin.h>
#endif

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_utilities
//! \{

//! Contains support classes and functions that may be used by AsmJit source and header files. Anything defined
//! here is considered internal and should not be used outside of AsmJit and related projects like AsmTK.
namespace Support {

// Support - Basic Traits
// ======================

#if ASMJIT_ARCH_X86
typedef uint8_t FastUInt8;
#else
typedef uint32_t FastUInt8;
#endif

//! \cond INTERNAL
namespace Internal {
  template<typename T, size_t Alignment>
  struct AliasedUInt {};

  template<> struct AliasedUInt<uint16_t, 2> { typedef uint16_t ASMJIT_MAY_ALIAS T; };
  template<> struct AliasedUInt<uint32_t, 4> { typedef uint32_t ASMJIT_MAY_ALIAS T; };
  template<> struct AliasedUInt<uint64_t, 8> { typedef uint64_t ASMJIT_MAY_ALIAS T; };

  template<> struct AliasedUInt<uint16_t, 1> { typedef uint16_t ASMJIT_MAY_ALIAS ASMJIT_ALIGN_TYPE(T, 1); };
  template<> struct AliasedUInt<uint32_t, 1> { typedef uint32_t ASMJIT_MAY_ALIAS ASMJIT_ALIGN_TYPE(T, 1); };
  template<> struct AliasedUInt<uint32_t, 2> { typedef uint32_t ASMJIT_MAY_ALIAS ASMJIT_ALIGN_TYPE(T, 2); };
  template<> struct AliasedUInt<uint64_t, 1> { typedef uint64_t ASMJIT_MAY_ALIAS ASMJIT_ALIGN_TYPE(T, 1); };
  template<> struct AliasedUInt<uint64_t, 2> { typedef uint64_t ASMJIT_MAY_ALIAS ASMJIT_ALIGN_TYPE(T, 2); };
  template<> struct AliasedUInt<uint64_t, 4> { typedef uint64_t ASMJIT_MAY_ALIAS ASMJIT_ALIGN_TYPE(T, 4); };

  // StdInt    - Make an int-type by size (signed or unsigned) that is the
  //             same as types defined by <stdint.h>.
  // Int32Or64 - Make an int-type that has at least 32 bits: [u]int[32|64]_t.

  template<size_t Size, unsigned Unsigned>
  struct StdInt {}; // Fail if not specialized.

  template<> struct StdInt<1, 0> { typedef int8_t   Type; };
  template<> struct StdInt<1, 1> { typedef uint8_t  Type; };
  template<> struct StdInt<2, 0> { typedef int16_t  Type; };
  template<> struct StdInt<2, 1> { typedef uint16_t Type; };
  template<> struct StdInt<4, 0> { typedef int32_t  Type; };
  template<> struct StdInt<4, 1> { typedef uint32_t Type; };
  template<> struct StdInt<8, 0> { typedef int64_t  Type; };
  template<> struct StdInt<8, 1> { typedef uint64_t Type; };

  template<typename T, int Unsigned = std::is_unsigned<T>::value>
  struct Int32Or64 : public StdInt<sizeof(T) <= 4 ? size_t(4) : sizeof(T), Unsigned> {};
}
//! \endcond

template<typename T>
static constexpr bool isUnsigned() noexcept { return std::is_unsigned<T>::value; }

//! Casts an integer `x` to either `uint32_t` or `uint64_t` depending on `T`.
template<typename T>
static constexpr typename Internal::Int32Or64<T, 1>::Type asUInt(const T& x) noexcept {
  return (typename Internal::Int32Or64<T, 1>::Type)x;
}

//! Casts an integer `x` to the same type as defined by `<stdint.h>`.
template<typename T>
static constexpr typename Internal::StdInt<sizeof(T), isUnsigned<T>()>::Type asStdInt(const T& x) noexcept {
  return (typename Internal::StdInt<sizeof(T), isUnsigned<T>()>::Type)x;
}

template<typename T>
static constexpr uint32_t bitSizeOf() noexcept { return uint32_t(sizeof(T) * 8u); }

//! Returns `0 - x` in a safe way (no undefined behavior), works for unsigned numbers as well.
template<typename T>
static constexpr T neg(const T& x) noexcept {
  typedef typename std::make_unsigned<T>::type U;
  return T(U(0) - U(x));
}

template<typename T>
static constexpr T allOnes() noexcept { return neg<T>(T(1)); }

//! Returns `x << y` (shift left logical) by explicitly casting `x` to an unsigned type and back.
template<typename X, typename Y>
static constexpr X shl(const X& x, const Y& y) noexcept {
  typedef typename std::make_unsigned<X>::type U;
  return X(U(x) << y);
}

//! Returns `x >> y` (shift right logical) by explicitly casting `x` to an unsigned type and back.
template<typename X, typename Y>
static constexpr X shr(const X& x, const Y& y) noexcept {
  typedef typename std::make_unsigned<X>::type U;
  return X(U(x) >> y);
}

//! Returns `x >> y` (shift right arithmetic) by explicitly casting `x` to a signed type and back.
template<typename X, typename Y>
static constexpr X sar(const X& x, const Y& y) noexcept {
  typedef typename std::make_signed<X>::type S;
  return X(S(x) >> y);
}

//! Returns `x | (x >> y)` - helper used by some bit manipulation helpers.
template<typename X, typename Y>
static constexpr X or_shr(const X& x, const Y& y) noexcept { return X(x | shr(x, y)); }

//! Generates a trailing bit-mask that has `n` least significant (trailing) bits set.
template<typename T, typename CountT>
static constexpr T lsbMask(const CountT& n) noexcept {
  typedef typename std::make_unsigned<T>::type U;
  return (sizeof(U) < sizeof(uintptr_t))
    // Prevent undefined behavior by using a larger type than T.
    ? T(U((uintptr_t(1) << n) - uintptr_t(1)))
    // Prevent undefined behavior by checking `n` before shift.
    : n ? T(shr(allOnes<T>(), bitSizeOf<T>() - size_t(n))) : T(0);
}
//! Tests whether `a & b` is non-zero.
template<typename A, typename B>
static inline constexpr bool test(A a, B b) noexcept { return (asUInt(a) & asUInt(b)) != 0; }

//! \cond
namespace Internal {
  // Fills all trailing bits right from the first most significant bit set.
  static constexpr uint8_t fillTrailingBitsImpl(uint8_t x) noexcept { return or_shr(or_shr(or_shr(x, 1), 2), 4); }
  // Fills all trailing bits right from the first most significant bit set.
  static constexpr uint16_t fillTrailingBitsImpl(uint16_t x) noexcept { return or_shr(or_shr(or_shr(or_shr(x, 1), 2), 4), 8); }
  // Fills all trailing bits right from the first most significant bit set.
  static constexpr uint32_t fillTrailingBitsImpl(uint32_t x) noexcept { return or_shr(or_shr(or_shr(or_shr(or_shr(x, 1), 2), 4), 8), 16); }
  // Fills all trailing bits right from the first most significant bit set.
  static constexpr uint64_t fillTrailingBitsImpl(uint64_t x) noexcept { return or_shr(or_shr(or_shr(or_shr(or_shr(or_shr(x, 1), 2), 4), 8), 16), 32); }
}
//! \endcond

// Fills all trailing bits right from the first most significant bit set.
template<typename T>
static constexpr T fillTrailingBits(const T& x) noexcept {
  typedef typename std::make_unsigned<T>::type U;
  return T(Internal::fillTrailingBitsImpl(U(x)));
}

// Support - Count Leading/Trailing Zeros
// ======================================

//! \cond
namespace Internal {
namespace {

template<typename T>
struct BitScanData { T x; uint32_t n; };

template<typename T, uint32_t N>
struct BitScanCalc {
  static constexpr BitScanData<T> advanceLeft(const BitScanData<T>& data, uint32_t n) noexcept {
    return BitScanData<T> { data.x << n, data.n + n };
  }

  static constexpr BitScanData<T> advanceRight(const BitScanData<T>& data, uint32_t n) noexcept {
    return BitScanData<T> { data.x >> n, data.n + n };
  }

  static constexpr BitScanData<T> clz(const BitScanData<T>& data) noexcept {
    return BitScanCalc<T, N / 2>::clz(advanceLeft(data, data.x & (allOnes<T>() << (bitSizeOf<T>() - N)) ? uint32_t(0) : N));
  }

  static constexpr BitScanData<T> ctz(const BitScanData<T>& data) noexcept {
    return BitScanCalc<T, N / 2>::ctz(advanceRight(data, data.x & (allOnes<T>() >> (bitSizeOf<T>() - N)) ? uint32_t(0) : N));
  }
};

template<typename T>
constexpr uint32_t ctzFallback(const T& x) noexcept {
  return BitScanCalc<T, bitSizeOf<T>() / 2u>::ctz(BitScanData<T>{x, 1}).n;
}

template<typename T> inline uint32_t clzImpl(const T& x) noexcept { return clzFallback(asUInt(x)); }
template<typename T> inline uint32_t ctzImpl(const T& x) noexcept { return ctzFallback(asUInt(x)); }

#if !defined(ASMJIT_NO_INTRINSICS)
# if defined(__GNUC__)
template<> inline uint32_t clzImpl(const uint32_t& x) noexcept { return uint32_t(__builtin_clz(x)); }
template<> inline uint32_t clzImpl(const uint64_t& x) noexcept { return uint32_t(__builtin_clzll(x)); }
template<> inline uint32_t ctzImpl(const uint32_t& x) noexcept { return uint32_t(__builtin_ctz(x)); }
template<> inline uint32_t ctzImpl(const uint64_t& x) noexcept { return uint32_t(__builtin_ctzll(x)); }
# elif defined(_MSC_VER)
template<> inline uint32_t clzImpl(const uint32_t& x) noexcept { unsigned long i; _BitScanReverse(&i, x); return uint32_t(i ^ 31); }
template<> inline uint32_t ctzImpl(const uint32_t& x) noexcept { unsigned long i; _BitScanForward(&i, x); return uint32_t(i); }
#  if ASMJIT_ARCH_X86 == 64 || ASMJIT_ARCH_ARM == 64
template<> inline uint32_t clzImpl(const uint64_t& x) noexcept { unsigned long i; _BitScanReverse64(&i, x); return uint32_t(i ^ 63); }
template<> inline uint32_t ctzImpl(const uint64_t& x) noexcept { unsigned long i; _BitScanForward64(&i, x); return uint32_t(i); }
#  endif
# endif
#endif

} // {anonymous}
} // {Internal}
//! \endcond

//! Count trailing zeros in `x` (returns a position of a first bit set in `x`).
//!
//! \note The input MUST NOT be zero, otherwise the result is undefined.
template<typename T>
static inline uint32_t ctz(T x) noexcept { return Internal::ctzImpl(asUInt(x)); }

template<uint64_t kInput>
struct ConstCTZ {
  static constexpr uint32_t value =
    (kInput & (uint64_t(1) <<  0)) ?  0 :
    (kInput & (uint64_t(1) <<  1)) ?  1 :
    (kInput & (uint64_t(1) <<  2)) ?  2 :
    (kInput & (uint64_t(1) <<  3)) ?  3 :
    (kInput & (uint64_t(1) <<  4)) ?  4 :
    (kInput & (uint64_t(1) <<  5)) ?  5 :
    (kInput & (uint64_t(1) <<  6)) ?  6 :
    (kInput & (uint64_t(1) <<  7)) ?  7 :
    (kInput & (uint64_t(1) <<  8)) ?  8 :
    (kInput & (uint64_t(1) <<  9)) ?  9 :
    (kInput & (uint64_t(1) << 10)) ? 10 :
    (kInput & (uint64_t(1) << 11)) ? 11 :
    (kInput & (uint64_t(1) << 12)) ? 12 :
    (kInput & (uint64_t(1) << 13)) ? 13 :
    (kInput & (uint64_t(1) << 14)) ? 14 :
    (kInput & (uint64_t(1) << 15)) ? 15 :
    (kInput & (uint64_t(1) << 16)) ? 16 :
    (kInput & (uint64_t(1) << 17)) ? 17 :
    (kInput & (uint64_t(1) << 18)) ? 18 :
    (kInput & (uint64_t(1) << 19)) ? 19 :
    (kInput & (uint64_t(1) << 20)) ? 20 :
    (kInput & (uint64_t(1) << 21)) ? 21 :
    (kInput & (uint64_t(1) << 22)) ? 22 :
    (kInput & (uint64_t(1) << 23)) ? 23 :
    (kInput & (uint64_t(1) << 24)) ? 24 :
    (kInput & (uint64_t(1) << 25)) ? 25 :
    (kInput & (uint64_t(1) << 26)) ? 26 :
    (kInput & (uint64_t(1) << 27)) ? 27 :
    (kInput & (uint64_t(1) << 28)) ? 28 :
    (kInput & (uint64_t(1) << 29)) ? 29 :
    (kInput & (uint64_t(1) << 30)) ? 30 :
    (kInput & (uint64_t(1) << 31)) ? 31 :
    (kInput & (uint64_t(1) << 32)) ? 32 :
    (kInput & (uint64_t(1) << 33)) ? 33 :
    (kInput & (uint64_t(1) << 34)) ? 34 :
    (kInput & (uint64_t(1) << 35)) ? 35 :
    (kInput & (uint64_t(1) << 36)) ? 36 :
    (kInput & (uint64_t(1) << 37)) ? 37 :
    (kInput & (uint64_t(1) << 38)) ? 38 :
    (kInput & (uint64_t(1) << 39)) ? 39 :
    (kInput & (uint64_t(1) << 40)) ? 40 :
    (kInput & (uint64_t(1) << 41)) ? 41 :
    (kInput & (uint64_t(1) << 42)) ? 42 :
    (kInput & (uint64_t(1) << 43)) ? 43 :
    (kInput & (uint64_t(1) << 44)) ? 44 :
    (kInput & (uint64_t(1) << 45)) ? 45 :
    (kInput & (uint64_t(1) << 46)) ? 46 :
    (kInput & (uint64_t(1) << 47)) ? 47 :
    (kInput & (uint64_t(1) << 48)) ? 48 :
    (kInput & (uint64_t(1) << 49)) ? 49 :
    (kInput & (uint64_t(1) << 50)) ? 50 :
    (kInput & (uint64_t(1) << 51)) ? 51 :
    (kInput & (uint64_t(1) << 52)) ? 52 :
    (kInput & (uint64_t(1) << 53)) ? 53 :
    (kInput & (uint64_t(1) << 54)) ? 54 :
    (kInput & (uint64_t(1) << 55)) ? 55 :
    (kInput & (uint64_t(1) << 56)) ? 56 :
    (kInput & (uint64_t(1) << 57)) ? 57 :
    (kInput & (uint64_t(1) << 58)) ? 58 :
    (kInput & (uint64_t(1) << 59)) ? 59 :
    (kInput & (uint64_t(1) << 60)) ? 60 :
    (kInput & (uint64_t(1) << 61)) ? 61 :
    (kInput & (uint64_t(1) << 62)) ? 62 :
    (kInput & (uint64_t(1) << 63)) ? 63 : 64;
};

// Support - Min/Max
// =================

// NOTE: These are constexpr `min()` and `max()` implementations that are not
// exactly the same as `std::min()` and `std::max()`. The return value is not
// a reference to `a` or `b` but it's a new value instead.

template<typename T>
static constexpr T min(const T& a, const T& b) noexcept { return b < a ? b : a; }

template<typename T, typename... Args>
static constexpr T min(const T& a, const T& b, Args&&... args) noexcept { return min(min(a, b), std::forward<Args>(args)...); }

template<typename T>
static constexpr T max(const T& a, const T& b) noexcept { return a < b ? b : a; }

template<typename T, typename... Args>
static constexpr T max(const T& a, const T& b, Args&&... args) noexcept { return max(max(a, b), std::forward<Args>(args)...); }

// Support - Overflow Arithmetic
// =============================

//! \cond
namespace Internal {
  template<typename T>
  inline T addOverflowFallback(T x, T y, FastUInt8* of) noexcept {
    typedef typename std::make_unsigned<T>::type U;

    U result = U(x) + U(y);
    *of = FastUInt8(*of | FastUInt8(isUnsigned<T>() ? result < U(x) : T((U(x) ^ ~U(y)) & (U(x) ^ result)) < 0));
    return T(result);
  }
  // These can be specialized.
  template<typename T> inline T addOverflowImpl(const T& x, const T& y, FastUInt8* of) noexcept { return addOverflowFallback(x, y, of); }
} // {Internal}
//! \endcond

template<typename T>
static inline T addOverflow(const T& x, const T& y, FastUInt8* of) noexcept { return T(Internal::addOverflowImpl(asStdInt(x), asStdInt(y), of)); }

template<typename X, typename Y>
static constexpr X alignUp(X x, Y alignment) noexcept {
  typedef typename Internal::StdInt<sizeof(X), 1>::Type U;
  return (X)( ((U)x + ((U)(alignment) - 1u)) & ~((U)(alignment) - 1u) );
}

template<typename T>
static constexpr T alignUpPowerOf2(T x) noexcept {
  typedef typename Internal::StdInt<sizeof(T), 1>::Type U;
  return (T)(fillTrailingBits(U(x) - 1u) + 1u);
}

//! Returns either zero or a positive difference between `base` and `base` when
//! aligned to `alignment`.
template<typename X, typename Y>
static constexpr X alignDown(X x, Y alignment) noexcept {
  typedef typename Internal::StdInt<sizeof(X), 1>::Type U;
  return (X)( (U)x & ~((U)(alignment) - 1u) );
}

// Support - IsBetween
// ===================

//! Checks whether `x` is greater than or equal to `a` and lesser than or equal to `b`.
template<typename T>
static constexpr bool isBetween(const T& x, const T& a, const T& b) noexcept {
  return x >= a && x <= b;
}

//! Checks whether the given integer `x` can be casted to an 8-bit signed integer.
template<typename T>
static constexpr bool isInt8(T x) noexcept {
  typedef typename std::make_signed<T>::type S;
  typedef typename std::make_unsigned<T>::type U;

  return std::is_signed<T>::value ? sizeof(T) <= 1 || isBetween<S>(S(x), -128, 127) : U(x) <= U(127u);
}

//! Checks whether the given integer `x` can be casted to a 32-bit signed integer.
template<typename T>
static constexpr bool isInt32(T x) noexcept {
  typedef typename std::make_signed<T>::type S;
  typedef typename std::make_unsigned<T>::type U;

  return std::is_signed<T>::value ? sizeof(T) <= 4 || isBetween<S>(S(x), -2147483647 - 1, 2147483647)
                                  : sizeof(T) <= 2 || U(x) <= U(2147483647u);
}

//! Checks whether the given integer `x` can be casted to a 32-bit unsigned integer.
template<typename T>
static constexpr bool isUInt32(T x) noexcept {
  typedef typename std::make_unsigned<T>::type U;

  return std::is_signed<T>::value ? (sizeof(T) <= 4 || T(x) <= T(4294967295u)) && x >= T(0)
                                  : (sizeof(T) <= 4 || U(x) <= U(4294967295u));
}

//! Checks whether the given integer `x` can be casted to a 32-bit unsigned integer.
template<typename T>
static constexpr bool isIntOrUInt32(T x) noexcept {
  return sizeof(T) <= 4 ? true : (uint32_t(uint64_t(x) >> 32) + 1u) <= 1u;
}

static bool inline isEncodableOffset32(int32_t offset, uint32_t nBits) noexcept {
  uint32_t nRev = 32 - nBits;
  return Support::sar(Support::shl(offset, nRev), nRev) == offset;
}

static bool inline isEncodableOffset64(int64_t offset, uint32_t nBits) noexcept {
  uint32_t nRev = 64 - nBits;
  return Support::sar(Support::shl(offset, nRev), nRev) == offset;
}

// Support - ByteSwap
// ==================

static inline uint16_t byteswap16(uint16_t x) noexcept {
  return uint16_t(((x >> 8) & 0xFFu) | ((x & 0xFFu) << 8));
}

static inline uint32_t byteswap32(uint32_t x) noexcept {
  return (x << 24) | (x >> 24) | ((x << 8) & 0x00FF0000u) | ((x >> 8) & 0x0000FF00);
}

static inline uint64_t byteswap64(uint64_t x) noexcept {
#if (defined(__GNUC__) || defined(__clang__)) && !defined(ASMJIT_NO_INTRINSICS)
  return uint64_t(__builtin_bswap64(uint64_t(x)));
#elif defined(_MSC_VER) && !defined(ASMJIT_NO_INTRINSICS)
  return uint64_t(_byteswap_uint64(uint64_t(x)));
#else
  return (uint64_t(byteswap32(uint32_t(uint64_t(x) >> 32        )))      ) |
         (uint64_t(byteswap32(uint32_t(uint64_t(x) & 0xFFFFFFFFu))) << 32) ;
#endif
}

// Support - BytePack & Unpack
// ===========================

//! Pack four 8-bit integer into a 32-bit integer as it is an array of `{b0,b1,b2,b3}`.
static constexpr uint32_t bytepack32_4x8(uint32_t a, uint32_t b, uint32_t c, uint32_t d) noexcept {
  return ASMJIT_ARCH_LE ? (a | (b << 8) | (c << 16) | (d << 24))
                        : (d | (c << 8) | (b << 16) | (a << 24));
}

template<typename T>
static constexpr uint32_t unpackU32At0(T x) noexcept { return ASMJIT_ARCH_LE ? uint32_t(uint64_t(x) & 0xFFFFFFFFu) : uint32_t(uint64_t(x) >> 32); }
template<typename T>
static constexpr uint32_t unpackU32At1(T x) noexcept { return ASMJIT_ARCH_BE ? uint32_t(uint64_t(x) & 0xFFFFFFFFu) : uint32_t(uint64_t(x) >> 32); }

static ASMJIT_FORCE_INLINE size_t strLen(const char* s, size_t maxSize) noexcept {
  size_t i = 0;
  while (i < maxSize && s[i] != '\0')
    i++;
  return i;
}
// Support - Memory Read Access - 8 Bits
// =====================================

static inline uint8_t readU8(const void* p) noexcept { return static_cast<const uint8_t*>(p)[0]; }

// Support - Memory Read Access - 16 Bits
// ======================================

template<ByteOrder BO, size_t Alignment>
static inline uint16_t readU16x(const void* p) noexcept {
  typedef typename Internal::AliasedUInt<uint16_t, Alignment>::T U16AlignedToN;
  uint16_t x = static_cast<const U16AlignedToN*>(p)[0];
  return BO == ByteOrder::kNative ? x : byteswap16(x);
}

template<size_t Alignment = 1>
static inline uint16_t readU16uLE(const void* p) noexcept { return readU16x<ByteOrder::kLE, Alignment>(p); }

// Support - Memory Read Access - 32 Bits
// ======================================

template<ByteOrder BO, size_t Alignment>
static inline uint32_t readU32x(const void* p) noexcept {
  typedef typename Internal::AliasedUInt<uint32_t, Alignment>::T U32AlignedToN;
  uint32_t x = static_cast<const U32AlignedToN*>(p)[0];
  return BO == ByteOrder::kNative ? x : byteswap32(x);
}

template<size_t Alignment = 1>
static inline uint32_t readU32uLE(const void* p) noexcept { return readU32x<ByteOrder::kLE, Alignment>(p); }
// Support - Memory Read Access - 64 Bits
// ======================================

template<ByteOrder BO, size_t Alignment>
static inline uint64_t readU64x(const void* p) noexcept {
  typedef typename Internal::AliasedUInt<uint64_t, Alignment>::T U64AlignedToN;
  uint64_t x = static_cast<const U64AlignedToN*>(p)[0];
  return BO == ByteOrder::kNative ? x : byteswap64(x);
}

template<size_t Alignment = 1>
static inline uint64_t readU64uLE(const void* p) noexcept { return readU64x<ByteOrder::kLE, Alignment>(p); }

// Support - Memory Write Access - 8 Bits
// ======================================

static inline void writeU8(void* p, uint8_t x) noexcept { static_cast<uint8_t*>(p)[0] = x; }

// Support - Memory Write Access - 16 Bits
// =======================================

template<ByteOrder BO = ByteOrder::kNative, size_t Alignment = 1>
static inline void writeU16x(void* p, uint16_t x) noexcept {
  typedef typename Internal::AliasedUInt<uint16_t, Alignment>::T U16AlignedToN;
  static_cast<U16AlignedToN*>(p)[0] = BO == ByteOrder::kNative ? x : byteswap16(x);
}

template<size_t Alignment = 1>
static inline void writeU16uLE(void* p, uint16_t x) noexcept { writeU16x<ByteOrder::kLE, Alignment>(p, x); }

// Support - Memory Write Access - 32 Bits
// =======================================

template<ByteOrder BO = ByteOrder::kNative, size_t Alignment = 1>
static inline void writeU32x(void* p, uint32_t x) noexcept {
  typedef typename Internal::AliasedUInt<uint32_t, Alignment>::T U32AlignedToN;
  static_cast<U32AlignedToN*>(p)[0] = (BO == ByteOrder::kNative) ? x : Support::byteswap32(x);
}

template<size_t Alignment = 1>
static inline void writeU32uLE(void* p, uint32_t x) noexcept { writeU32x<ByteOrder::kLE, Alignment>(p, x); }

// Support - Memory Write Access - 64 Bits
// =======================================

template<ByteOrder BO = ByteOrder::kNative, size_t Alignment = 1>
static inline void writeU64x(void* p, uint64_t x) noexcept {
  typedef typename Internal::AliasedUInt<uint64_t, Alignment>::T U64AlignedToN;
  static_cast<U64AlignedToN*>(p)[0] = BO == ByteOrder::kNative ? x : byteswap64(x);
}

template<size_t Alignment = 1>
static inline void writeU64uLE(void* p, uint64_t x) noexcept { writeU64x<ByteOrder::kLE, Alignment>(p, x); }
// Support - Sorting
// =================

//! Sort order.
enum class SortOrder : uint32_t {
  //!< Ascending order.
  kAscending  = 0,
  //!< Descending order.
  kDescending = 1
};

//! A helper class that provides comparison of any user-defined type that
//! implements `<` and `>` operators (primitive types are supported as well).
template<SortOrder kOrder = SortOrder::kAscending>
struct Compare {
  template<typename A, typename B>
  inline int operator()(const A& a, const B& b) const noexcept {
    return kOrder == SortOrder::kAscending ? int(a > b) - int(a < b) : int(a < b) - int(a > b);
  }
};


// Support - Array
// ===============

//! Array type, similar to std::array<T, N>, with the possibility to use enums in operator[].
//!
//! \note The array has C semantics - the elements in the array are not initialized.
template<typename T, size_t N>
struct Array {
  //! \name Members
  //! \{

  //! The underlying array data, use \ref data() to access it.
  T _data[N];

  //! \}

  //! \cond
  // std compatibility.
  typedef T value_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  typedef value_type& reference;
  typedef const value_type& const_reference;

  typedef value_type* pointer;
  typedef const value_type* const_pointer;

  typedef pointer iterator;
  typedef const_pointer const_iterator;
  //! \endcond

  //! \name Overloaded Operators
  //! \{

  template<typename Index>
  inline T& operator[](const Index& index) noexcept {
    typedef typename Internal::StdInt<sizeof(Index), 1>::Type U;
    ASMJIT_ASSERT(U(index) < N);
    return _data[U(index)];
  }

  template<typename Index>
  inline const T& operator[](const Index& index) const noexcept {
    typedef typename Internal::StdInt<sizeof(Index), 1>::Type U;
    ASMJIT_ASSERT(U(index) < N);
    return _data[U(index)];
  }

  inline bool operator==(const Array& other) const noexcept {
    for (size_t i = 0; i < N; i++)
      if (_data[i] != other._data[i])
        return false;
    return true;
  }

  inline bool operator!=(const Array& other) const noexcept {
    return !operator==(other);
  }

  //! \}

  //! \name Accessors
  //! \{

  inline bool empty() const noexcept { return false; }
  inline size_t size() const noexcept { return N; }

  inline T* data() noexcept { return _data; }
  inline const T* data() const noexcept { return _data; }

  inline T& front() noexcept { return _data[0]; }
  inline const T& front() const noexcept { return _data[0]; }

  inline T& back() noexcept { return _data[N - 1]; }
  inline const T& back() const noexcept { return _data[N - 1]; }

  inline T* begin() noexcept { return _data; }
  inline T* end() noexcept { return _data + N; }

  inline const T* begin() const noexcept { return _data; }
  inline const T* end() const noexcept { return _data + N; }

  inline const T* cbegin() const noexcept { return _data; }
  inline const T* cend() const noexcept { return _data + N; }

  //! \}

  //! \name Utilities
  //! \{

  inline void swap(Array& other) noexcept {
    for (size_t i = 0; i < N; i++)
      std::swap(_data[i], other._data[i]);
  }

  inline void fill(const T& value) noexcept {
    for (size_t i = 0; i < N; i++)
      _data[i] = value;
  }

  inline void copyFrom(const Array& other) noexcept {
    for (size_t i = 0; i < N; i++)
      _data[i] = other._data[i];
  }

  template<typename Operator>
  inline void combine(const Array& other) noexcept {
    for (size_t i = 0; i < N; i++)
      _data[i] = Operator::op(_data[i], other._data[i]);
  }

  template<typename Operator>
  inline T aggregate(T initialValue = T()) const noexcept {
    T value = initialValue;
    for (size_t i = 0; i < N; i++)
      value = Operator::op(value, _data[i]);
    return value;
  }

  template<typename Fn>
  inline void forEach(Fn&& fn) noexcept {
    for (size_t i = 0; i < N; i++)
      fn(_data[i]);
  }
  //! \}
};

// Support::Temporary
// ==================

//! Used to pass a temporary buffer to:
//!
//!   - Containers that use user-passed buffer as an initial storage (still can grow).
//!   - Zone allocator that would use the temporary buffer as a first block.
struct Temporary {
  //! \name Members
  //! \{

  void* _data;
  size_t _size;

  //! \}

  //! \name Construction & Destruction
  //! \{

  inline constexpr Temporary(const Temporary& other) noexcept = default;
  inline constexpr Temporary(void* data, size_t size) noexcept
    : _data(data),
      _size(size) {}

  //! \}

  //! \name Overloaded Operators
  //! \{

  inline Temporary& operator=(const Temporary& other) noexcept = default;

  //! \}

  //! \name Accessors
  //! \{

  //! Returns the data storage.
  template<typename T = void>
  inline constexpr T* data() const noexcept { return static_cast<T*>(_data); }
  //! Returns the data storage size in bytes.
  inline constexpr size_t size() const noexcept { return _size; }

  //! \}
};

} // {Support}

//! \}

ASMJIT_END_NAMESPACE

#endif // ASMJIT_CORE_SUPPORT_H_INCLUDED

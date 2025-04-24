#pragma once
#include "../core/globals.h"
#include "../core/support.h"

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_core
//! \{

//! Type identifier provides a minimalist type system used across AsmJit library.
//!
//! This is an additional information that can be used to describe a value-type of physical or virtual register. It's
//! used mostly by BaseCompiler to describe register representation (the group of data stored in the register and the
//! width used) and it's also used by APIs that allow to describe and work with function signatures.
enum class TypeId : uint8_t {
  //! Void type.
  kVoid = 0,

  _kBaseStart = 32,
  _kBaseEnd = 44,

  _kIntStart = 32,
  _kIntEnd = 41,

  //! Abstract signed integer type that has a native size.
  kIntPtr = 32,
  //! Abstract unsigned integer type that has a native size.
  kUIntPtr = 33,

  //! 8-bit signed integer type.
  kInt8 = 34,
  //! 8-bit unsigned integer type.
  kUInt8 = 35,
  //! 16-bit signed integer type.
  kInt16 = 36,
  //! 16-bit unsigned integer type.
  kUInt16 = 37,
  //! 32-bit signed integer type.
  kInt32 = 38,
  //! 32-bit unsigned integer type.
  kUInt32 = 39,
  //! 64-bit signed integer type.
  kInt64 = 40,
  //! 64-bit unsigned integer type.
  kUInt64 = 41,

  _kFloatStart  = 42,
  _kFloatEnd = 44,

  //! 32-bit floating point type.
  kFloat32 = 42,
  //! 64-bit floating point type.
  kFloat64 = 43,
  //! 80-bit floating point type.
  kFloat80 = 44,

  _kMaskStart = 45,
  _kMaskEnd = 48,

  //! 8-bit opmask register (K).
  kMask8 = 45,
  //! 16-bit opmask register (K).
  kMask16 = 46,
  //! 32-bit opmask register (K).
  kMask32 = 47,
  //! 64-bit opmask register (K).
  kMask64 = 48,

  _kMmxStart = 49,
  _kMmxEnd = 50,

  //! 64-bit MMX register only used for 32 bits.
  kMmx32 = 49,
  //! 64-bit MMX register.
  kMmx64 = 50,

  _kVec32Start  = 51,
  _kVec32End = 60,

  kInt8x4 = 51,
  kUInt8x4 = 52,
  kInt16x2 = 53,
  kUInt16x2 = 54,
  kInt32x1 = 55,
  kUInt32x1 = 56,
  kFloat32x1 = 59,

  _kVec64Start  = 61,
  _kVec64End = 70,

  kInt8x8 = 61,
  kUInt8x8 = 62,
  kInt16x4 = 63,
  kUInt16x4 = 64,
  kInt32x2 = 65,
  kUInt32x2 = 66,
  kInt64x1 = 67,
  kUInt64x1 = 68,
  kFloat32x2 = 69,
  kFloat64x1 = 70,

  _kVec128Start = 71,
  _kVec128End = 80,

  kInt8x16 = 71,
  kUInt8x16 = 72,
  kInt16x8 = 73,
  kUInt16x8 = 74,
  kInt32x4 = 75,
  kUInt32x4 = 76,
  kInt64x2 = 77,
  kUInt64x2 = 78,
  kFloat32x4 = 79,
  kFloat64x2 = 80,

  _kVec256Start = 81,
  _kVec256End = 90,

  kInt8x32 = 81,
  kUInt8x32 = 82,
  kInt16x16 = 83,
  kUInt16x16 = 84,
  kInt32x8 = 85,
  kUInt32x8 = 86,
  kInt64x4 = 87,
  kUInt64x4 = 88,
  kFloat32x8 = 89,
  kFloat64x4 = 90,

  _kVec512Start = 91,
  _kVec512End = 100,

  kInt8x64 = 91,
  kUInt8x64 = 92,
  kInt16x32 = 93,
  kUInt16x32 = 94,
  kInt32x16 = 95,
  kUInt32x16 = 96,
  kInt64x8 = 97,
  kUInt64x8 = 98,
  kFloat32x16 = 99,
  kFloat64x8 = 100,

  kLastAssigned = kFloat64x8,

  kMaxValue = 255
};

ASMJIT_END_NAMESPACE

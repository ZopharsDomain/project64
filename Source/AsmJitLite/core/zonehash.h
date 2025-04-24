#pragma once
#include "../core/zone.h"

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_zone
//! \{

//! Node used by \ref ZoneHash template.
//!
//! You must provide function `bool eq(const Key& key)` in order to make `ZoneHash::get()` working.
class ZoneHashNode {
public:
  ASMJIT_NONCOPYABLE(ZoneHashNode)

  inline ZoneHashNode(uint32_t hashCode = 0) noexcept
    : _hashNext(nullptr),
      _hashCode(hashCode),
      _customData(0) {}

  //! Next node in the chain, null if it terminates the chain.
  ZoneHashNode* _hashNext;
  //! Precalculated hash-code of key.
  uint32_t _hashCode;
  //! Padding, can be reused by any Node that inherits `ZoneHashNode`.
  uint32_t _customData;
};

ASMJIT_END_NAMESPACE

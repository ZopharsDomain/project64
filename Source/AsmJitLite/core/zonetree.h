#pragma once

#include "../core/support.h"

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_zone
//! \{

//! RB-Tree node.
//!
//! The color is stored in a least significant bit of the `left` node.
//!
//! WARNING: Always use accessors to access left and right children.
class ZoneTreeNode {
public:
  ASMJIT_NONCOPYABLE(ZoneTreeNode)

  //! \name Constants
  //! \{

  enum : uintptr_t {
    kRedMask = 0x1,
    kPtrMask = ~kRedMask
  };

  //! \}

  //! \name Members
  //! \{

  uintptr_t _rbNodeData[2];

  //! \}

  //! \name Construction & Destruction
  //! \{

  inline ZoneTreeNode() noexcept
    : _rbNodeData { 0, 0 } {}

  //! \}

  //! \name Accessors
  //! \{

  //! \}

  //! \cond INTERNAL
  //! \name Internal
  //! \{

  inline ZoneTreeNode* _getChild(size_t i) const noexcept { return (ZoneTreeNode*)(_rbNodeData[i] & kPtrMask); }

  //! \}
  //! \endcond
};

//! RB-Tree node casted to `NodeT`.
template<typename NodeT>
class ZoneTreeNodeT : public ZoneTreeNode {
public:
  ASMJIT_NONCOPYABLE(ZoneTreeNodeT)

  //! \name Construction & Destruction
  //! \{

  inline ZoneTreeNodeT() noexcept
    : ZoneTreeNode() {}

  //! \}

  //! \name Accessors
  //! \{

  inline NodeT* child(size_t i) const noexcept { return static_cast<NodeT*>(_getChild(i)); }
  inline NodeT* left() const noexcept { return static_cast<NodeT*>(_getLeft()); }
  inline NodeT* right() const noexcept { return static_cast<NodeT*>(_getRight()); }

  //! \}
};

//! RB-Tree.
template<typename NodeT>
class ZoneTree {
public:
  ASMJIT_NONCOPYABLE(ZoneTree)

  typedef NodeT Node;
  NodeT* _root;

  //! \name Construction & Destruction
  //! \{

  inline ZoneTree() noexcept
    : _root(nullptr) {}

  inline ZoneTree(ZoneTree&& other) noexcept
    : _root(other._root) {}

  inline void reset() noexcept { _root = nullptr; }


  //! \}

  template<typename KeyT, typename CompareT = Support::Compare<Support::SortOrder::kAscending>>
  inline NodeT* get(const KeyT& key, const CompareT& cmp = CompareT()) const noexcept {
    ZoneTreeNode* node = _root;
    while (node) {
      auto result = cmp(*static_cast<const NodeT*>(node), key);
      if (result == 0) break;

      // Go left or right depending on the `result`.
      node = node->_getChild(result < 0);
    }
    return static_cast<NodeT*>(node);
  }
  //! \endcond
};

//! \}

ASMJIT_END_NAMESPACE

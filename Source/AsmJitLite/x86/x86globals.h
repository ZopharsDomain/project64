#pragma once

ASMJIT_BEGIN_SUB_NAMESPACE(x86)

//! Condition code.
enum class CondCode : uint8_t {
  kO             = 0x00u,       //!<         OF==1
  kNO            = 0x01u,       //!<         OF==0
  kC             = 0x02u,       //!< CF==1
  kB             = 0x02u,       //!< CF==1          (unsigned < )
  kNAE           = 0x02u,       //!< CF==1          (unsigned < )
  kNC            = 0x03u,       //!< CF==0
  kAE            = 0x03u,       //!< CF==0          (unsigned >=)
  kNB            = 0x03u,       //!< CF==0          (unsigned >=)
  kE             = 0x04u,       //!<         ZF==1  (any_sign ==)
  kZ             = 0x04u,       //!<         ZF==1  (any_sign ==)
  kNE            = 0x05u,       //!<         ZF==0  (any_sign !=)
  kNZ            = 0x05u,       //!<         ZF==0  (any_sign !=)
  kBE            = 0x06u,       //!< CF==1 | ZF==1  (unsigned <=)
  kNA            = 0x06u,       //!< CF==1 | ZF==1  (unsigned <=)
  kA             = 0x07u,       //!< CF==0 & ZF==0  (unsigned > )
  kNBE           = 0x07u,       //!< CF==0 & ZF==0  (unsigned > )
  kS             = 0x08u,       //!<         SF==1  (is negative)
  kNS            = 0x09u,       //!<         SF==0  (is positive or zero)
  kP             = 0x0Au,       //!< PF==1
  kPE            = 0x0Au,       //!< PF==1
  kPO            = 0x0Bu,       //!< PF==0
  kNP            = 0x0Bu,       //!< PF==0
  kL             = 0x0Cu,       //!<         SF!=OF (signed < )
  kNGE           = 0x0Cu,       //!<         SF!=OF (signed < )
  kGE            = 0x0Du,       //!<         SF==OF (signed >=)
  kNL            = 0x0Du,       //!<         SF==OF (signed >=)
  kLE            = 0x0Eu,       //!< ZF==1 | SF!=OF (signed <=)
  kNG            = 0x0Eu,       //!< ZF==1 | SF!=OF (signed <=)
  kG             = 0x0Fu,       //!< ZF==0 & SF==OF (signed > )
  kNLE           = 0x0Fu,       //!< ZF==0 & SF==OF (signed > )

  kZero          = kZ,          //!< Zero flag.
  kNotZero       = kNZ,         //!< Non-zero flag.

  kSign          = kS,          //!< Sign flag.
  kNotSign       = kNS,         //!< No sign flag.

  kNegative      = kS,          //!< Sign flag.
  kPositive      = kNS,         //!< No sign flag.

  kOverflow      = kO,          //!< Overflow (signed).
  kNotOverflow   = kNO,         //!< Not overflow (signed).

  kEqual         = kE,          //!< `a == b` (equal).
  kNotEqual      = kNE,         //!< `a != b` (not equal).

  kSignedLT      = kL,          //!< `a <  b` (signed).
  kSignedLE      = kLE,         //!< `a <= b` (signed).
  kSignedGT      = kG,          //!< `a >  b` (signed).
  kSignedGE      = kGE,         //!< `a >= b` (signed).

  kUnsignedLT    = kB,          //!< `a <  b` (unsigned).
  kUnsignedLE    = kBE,         //!< `a <= b` (unsigned).
  kUnsignedGT    = kA,          //!< `a >  b` (unsigned).
  kUnsignedGE    = kAE,         //!< `a >= b` (unsigned).

  kParityEven    = kP,          //!< Even parity flag.
  kParityOdd     = kPO,         //!< Odd parity flag.

  kMaxValue      = 0x0Fu
};

ASMJIT_END_SUB_NAMESPACE

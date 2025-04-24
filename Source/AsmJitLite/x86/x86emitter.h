// This file is part of AsmJit project <https://asmjit.com>
//
// See asmjit.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#ifndef ASMJIT_X86_X86EMITTER_H_INCLUDED
#define ASMJIT_X86_X86EMITTER_H_INCLUDED

#include "../core/emitter.h"
#include "../core/globals.h"
#include "../x86/x86globals.h"
#include "../x86/x86operand.h"

ASMJIT_BEGIN_SUB_NAMESPACE(x86)

#define ASMJIT_INST_0x(NAME, ID) \
  Error NAME();

#define ASMJIT_INST_1x(NAME, ID, T0) \
  Error NAME(const T0& o0);

#define ASMJIT_INST_1c(NAME, ID, CONV, T0) \
  Error NAME(CondCode cc, const T0& o0); \
  Error NAME##a(const T0& o0); \
  Error NAME##ae(const T0& o0); \
  Error NAME##b(const T0& o0); \
  Error NAME##be(const T0& o0); \
  Error NAME##c(const T0& o0); \
  Error NAME##e(const T0& o0); \
  Error NAME##g(const T0& o0); \
  Error NAME##ge(const T0& o0); \
  Error NAME##l(const T0& o0); \
  Error NAME##le(const T0& o0); \
  Error NAME##na(const T0& o0); \
  Error NAME##nae(const T0& o0); \
  Error NAME##nb(const T0& o0); \
  Error NAME##nbe(const T0& o0); \
  Error NAME##nc(const T0& o0); \
  Error NAME##ne(const T0& o0); \
  Error NAME##ng(const T0& o0); \
  Error NAME##nge(const T0& o0); \
  Error NAME##nl(const T0& o0); \
  Error NAME##nle(const T0& o0); \
  Error NAME##no(const T0& o0); \
  Error NAME##np(const T0& o0); \
  Error NAME##ns(const T0& o0); \
  Error NAME##nz(const T0& o0); \
  Error NAME##o(const T0& o0); \
  Error NAME##p(const T0& o0); \
  Error NAME##pe(const T0& o0); \
  Error NAME##po(const T0& o0); \
  Error NAME##s(const T0& o0); \
  Error NAME##z(const T0& o0);

#define ASMJIT_INST_2x(NAME, ID, T0, T1) \
  Error NAME(const T0& o0, const T1& o1);

#define ASMJIT_INST_3x(NAME, ID, T0, T1, T2) \
  Error NAME(const T0& o0, const T1& o1, const T2& o2);

template<typename This>
struct EmitterExplicitT {
    This * _emitter;

    EmitterExplicitT(This * emitter) 
    {
        _emitter = emitter;
    }
    
  typedef Gp Gp_AH;
  typedef Gp Gp_CL;
  ASMJIT_INST_2x(adc, Adc, Gp, Gp)                                     // ANY
  ASMJIT_INST_2x(adc, Adc, Gp, Mem)                                    // ANY
  ASMJIT_INST_2x(adc, Adc, Gp, Imm)                                    // ANY
  ASMJIT_INST_2x(add, Add, Gp, Gp)                                     // ANY
  ASMJIT_INST_2x(add, Add, Gp, Mem)                                    // ANY
  ASMJIT_INST_2x(add, Add, Gp, Imm)                                    // ANY
  ASMJIT_INST_2x(add, Add, Mem, Imm)                                   // ANY
  ASMJIT_INST_2x(and_, And, Gp, Gp)                                    // ANY
  ASMJIT_INST_2x(and_, And, Gp, Mem)                                   // ANY
  ASMJIT_INST_2x(and_, And, Gp, Imm)                                   // ANY
  ASMJIT_INST_2x(and_, And, Mem, Imm)                                  // ANY
  ASMJIT_INST_1x(call, Call, Gp)                                       // ANY
  ASMJIT_INST_1x(call, Call, Mem)                                      // ANY
  ASMJIT_INST_1x(call, Call, Imm)                                      // ANY
  ASMJIT_INST_2x(cmp, Cmp, Gp, Gp)                                     // ANY
  ASMJIT_INST_2x(cmp, Cmp, Gp, Mem)                                    // ANY
  ASMJIT_INST_2x(cmp, Cmp, Gp, Imm)                                    // ANY
  ASMJIT_INST_2x(cmp, Cmp, Mem, Imm)                                   // ANY
  ASMJIT_INST_1x(dec, Dec, Gp)                                         // ANY
  ASMJIT_INST_1x(inc, Inc, Gp)                                         // ANY
  ASMJIT_INST_1c(j, J, Inst::jccFromCond, Label)                       // ANY
  ASMJIT_INST_1x(jmp, Jmp, Label)                                      // ANY
  ASMJIT_INST_2x(lea, Lea, Gp, Mem)                                    // ANY
  ASMJIT_INST_2x(mov, Mov, Gp, Gp)                                     // ANY
  ASMJIT_INST_2x(mov, Mov, Gp, Mem)                                    // ANY
  ASMJIT_INST_2x(mov, Mov, Gp, Imm)                                    // ANY
  ASMJIT_INST_2x(mov, Mov, Mem, Gp)                                    // ANY
  ASMJIT_INST_2x(mov, Mov, Mem, Imm)                                   // ANY
  ASMJIT_INST_2x(movsx, Movsx, Gp, Mem)                                // ANY
  ASMJIT_INST_2x(movzx, Movzx, Gp, Mem)                                // ANY
  ASMJIT_INST_1x(not_, Not, Gp)                                        // ANY
  ASMJIT_INST_2x(or_, Or, Gp, Gp)                                      // ANY
  ASMJIT_INST_2x(or_, Or, Gp, Mem)                                     // ANY
  ASMJIT_INST_2x(or_, Or, Gp, Imm)                                     // ANY
  ASMJIT_INST_2x(or_, Or, Mem, Gp)                                     // ANY
  ASMJIT_INST_2x(or_, Or, Mem, Imm)                                    // ANY
  ASMJIT_INST_1x(pop, Pop, Gp)                                         // ANY
  ASMJIT_INST_0x(popad, Popad)                                         // X86
  ASMJIT_INST_1x(push, Push, Gp)                                       // ANY
  ASMJIT_INST_1x(push, Push, Imm)                                      // ANY
  ASMJIT_INST_0x(pushad, Pushad)                                       // X86
  ASMJIT_INST_2x(sbb, Sbb, Gp, Gp)                                     // ANY
  ASMJIT_INST_2x(sbb, Sbb, Gp, Mem)                                    // ANY
  ASMJIT_INST_2x(sbb, Sbb, Gp, Imm)                                    // ANY
  ASMJIT_INST_2x(sar, Sar, Gp, Gp_CL)                                  // ANY
  ASMJIT_INST_2x(sar, Sar, Gp, Imm)                                    // ANY
  ASMJIT_INST_1c(set, Set, Inst::setccFromCond, Gp)                    // ANY
  ASMJIT_INST_1c(set, Set, Inst::setccFromCond, Mem)                   // ANY
  ASMJIT_INST_2x(shl, Shl, Gp, Gp_CL)                                  // ANY
  ASMJIT_INST_2x(shl, Shl, Gp, Imm)                                    // ANY
  ASMJIT_INST_2x(shr, Shr, Gp, Gp_CL)                                  // ANY
  ASMJIT_INST_2x(shr, Shr, Gp, Imm)                                    // ANY
  ASMJIT_INST_3x(shld, Shld, Gp, Gp, Gp_CL)                            // ANY
  ASMJIT_INST_3x(shld, Shld, Gp, Gp, Imm)                              // ANY
  ASMJIT_INST_3x(shrd, Shrd, Gp, Gp, Gp_CL)                            // ANY
  ASMJIT_INST_3x(shrd, Shrd, Gp, Gp, Imm)                              // ANY
  ASMJIT_INST_2x(sub, Sub, Gp, Gp)                                     // ANY
  ASMJIT_INST_2x(sub, Sub, Gp, Mem)                                    // ANY
  ASMJIT_INST_2x(sub, Sub, Gp, Imm)                                    // ANY
  ASMJIT_INST_2x(sub, Sub, Mem, Imm)                                   // ANY
  ASMJIT_INST_2x(test, Test, Gp, Gp)                                   // ANY
  ASMJIT_INST_2x(test, Test, Gp, Imm)                                  // ANY
  ASMJIT_INST_2x(test, Test, Mem, Imm)                                 // ANY
  ASMJIT_INST_2x(xor_, Xor, Gp, Gp)                                    // ANY
  ASMJIT_INST_2x(xor_, Xor, Gp, Mem)                                   // ANY
  ASMJIT_INST_2x(xor_, Xor, Gp, Imm)                                   // ANY
  ASMJIT_INST_1x(sahf, Sahf, Gp_AH)                                    // LAHFSAHF [EXPLICIT] EFL <- AH
  ASMJIT_INST_1x(ldmxcsr, Ldmxcsr, Mem)                                // SSE
  ASMJIT_INST_1x(stmxcsr, Stmxcsr, Mem)                                // SSE
  ASMJIT_INST_0x(int3, Int3)                                           // ANY

  ASMJIT_INST_0x(fabs, Fabs)                                           // FPU
  ASMJIT_INST_1x(fadd, Fadd, Mem)                                      // FPU
  ASMJIT_INST_0x(fchs, Fchs)                                           // FPU
  ASMJIT_INST_0x(fclex, Fclex)                                         // FPU
  ASMJIT_INST_1x(fcomp, Fcomp, St)                                     // FPU
  ASMJIT_INST_1x(fcomp, Fcomp, Mem)                                    // FPU
  ASMJIT_INST_0x(fcompp, Fcompp)                                       // FPU
  ASMJIT_INST_1x(fdiv, Fdiv, Mem)                                      // FPU
  ASMJIT_INST_1x(ffree, Ffree, St)                                     // FPU
  ASMJIT_INST_1x(fild, Fild, Mem)                                      // FPU
  ASMJIT_INST_0x(fincstp, Fincstp)                                     // FPU
  ASMJIT_INST_0x(finit, Finit)                                         // FPU
  ASMJIT_INST_1x(fist, Fist, Mem)                                      // FPU
  ASMJIT_INST_1x(fistp, Fistp, Mem)                                    // FPU
  ASMJIT_INST_1x(fld, Fld, Mem)                                        // FPU
  ASMJIT_INST_1x(fld, Fld, St)                                         // FPU
  ASMJIT_INST_1x(fldcw, Fldcw, Mem)                                    // FPU
  ASMJIT_INST_1x(fmul, Fmul, Mem)                                      // FPU
  ASMJIT_INST_1x(fnstcw, Fnstcw, Mem)                                  // FPU
  ASMJIT_INST_0x(fsqrt, Fsqrt)                                         // FPU
  ASMJIT_INST_1x(fst, Fst, Mem)                                        // FPU
  ASMJIT_INST_1x(fstp, Fstp, Mem)                                      // FPU
  ASMJIT_INST_1x(fsub, Fsub, Mem)                                      // FPU
  ASMJIT_INST_1x(fxch, Fxch, St)                                       // FPU
  ASMJIT_INST_1x(fstsw, Fstsw, Gp)                                     // FPU
  ASMJIT_INST_1x(fnstsw, Fnstsw, Gp)                                   // FPU

  Error emitJump(uint16_t Opcode, uint8_t OpcodeSize, const Label & o0);
  static const char * x86_Name(const x86::Gp & Reg);
};

template<typename This>
struct EmitterImplicitT : public EmitterExplicitT<This> {
    EmitterImplicitT(This * emitter) : EmitterExplicitT<This>(emitter) {}

    EmitterImplicitT() = delete;
    EmitterImplicitT(const EmitterImplicitT&) = delete;
    EmitterImplicitT& operator=(const EmitterImplicitT&) = delete;

  ASMJIT_INST_1x(div, Div, Gp)                                         // ANY       [IMPLICIT] {AH[Rem]: AL[Quot] <- AX / r8} {xDX[Rem]:xAX[Quot] <- DX:AX / r16|r32|r64}
  ASMJIT_INST_1x(idiv, Idiv, Gp)                                       // ANY       [IMPLICIT] {AH[Rem]: AL[Quot] <- AX / r8} {xDX[Rem]:xAX[Quot] <- DX:AX / r16|r32|r64}
  ASMJIT_INST_1x(imul, Imul, Gp)                                       // ANY       [IMPLICIT] {AX <- AL * r8} {xAX:xDX <- xAX * r16|r32|r64}
  ASMJIT_INST_1x(jecxz, Jecxz, Label)                                  // ANY       [IMPLICIT] Short jump if CX/ECX/RCX is zero.
  ASMJIT_INST_1x(mul, Mul, Gp)                                         // ANY       [IMPLICIT] {AX <- AL * r8} {xDX:xAX <- xAX * r16|r32|r64}
  ASMJIT_INST_0x(ret, Ret)
  ASMJIT_INST_0x(sahf, Sahf)                                           // LAHFSAHF  [IMPLICIT] EFL <- AH
};

#undef ASMJIT_INST_0x
#undef ASMJIT_INST_1x
#undef ASMJIT_INST_1c
#undef ASMJIT_INST_2x
#undef ASMJIT_INST_2c
#undef ASMJIT_INST_3x
#undef ASMJIT_INST_4x
#undef ASMJIT_INST_5x
#undef ASMJIT_INST_6x

ASMJIT_END_SUB_NAMESPACE

#endif // ASMJIT_X86_X86EMITTER_H_INCLUDED

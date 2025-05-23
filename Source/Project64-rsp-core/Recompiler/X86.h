#pragma once
#if defined(__i386__) || defined(_M_IX86)
#include <stdint.h>

enum x86RegValues
{
    x86_EAX = 0,
    x86_EBX = 1,
    x86_ECX = 2,
    x86_EDX = 3,
    x86_ESI = 4,
    x86_EDI = 5,
    x86_EBP = 6,
    x86_ESP = 7
};

enum mmxRegValues
{
    x86_MM0 = 0,
    x86_MM1 = 1,
    x86_MM2 = 2,
    x86_MM3 = 3,
    x86_MM4 = 4,
    x86_MM5 = 5,
    x86_MM6 = 6,
    x86_MM7 = 7
};

enum sseRegValues
{
    x86_XMM0 = 0,
    x86_XMM1 = 1,
    x86_XMM2 = 2,
    x86_XMM3 = 3,
    x86_XMM4 = 4,
    x86_XMM5 = 5,
    x86_XMM6 = 6,
    x86_XMM7 = 7
};

void AdcX86RegToX86Reg(int Destination, int Source);
void AdcX86regToVariable(int x86reg, void * Variable, const char * VariableName);
void AdcX86regHalfToVariable(int x86reg, void * Variable, const char * VariableName);
void AdcConstToX86reg(uint8_t Constant, int x86reg);
void AdcConstToVariable(void * Variable, const char * VariableName, uint8_t Constant);
void AdcConstHalfToVariable(void * Variable, const char * VariableName, uint8_t Constant);
void AddConstToVariable(uint32_t Const, void * Variable, const char * VariableName);
void AddConstToX86Reg(int x86Reg, size_t Const);
void AddVariableToX86reg(int x86reg, void * Variable, const char * VariableName);
void AddX86regToVariable(int x86reg, void * Variable, const char * VariableName);
void AddX86regHalfToVariable(int x86reg, void * Variable, const char * VariableName);
void AddX86RegToX86Reg(int Destination, int Source);
void AndConstToVariable(uint32_t Const, void * Variable, const char * VariableName);
void AndConstToX86Reg(int x86Reg, uint32_t Const);
void AndVariableToX86Reg(void * Variable, const char * VariableName, int x86Reg);
void AndVariableToX86regHalf(void * Variable, const char * VariableName, int x86Reg);
void AndX86RegToVariable(void * Variable, const char * VariableName, int x86Reg);
void AndX86RegToX86Reg(int Destination, int Source);
void AndX86RegHalfToX86RegHalf(int Destination, int Source);
void X86BreakPoint(const char * FileName, int LineNumber);
void BsrX86RegToX86Reg(int Destination, int Source);
void Call_Direct(void * FunctAddress, const char * FunctName);
void Call_Indirect(void * FunctAddress, const char * FunctName);
void CondMoveEqual(int Destination, int Source);
void CondMoveNotEqual(int Destination, int Source);
void CondMoveGreater(int Destination, int Source);
void CondMoveGreaterEqual(int Destination, int Source);
void CondMoveLess(int Destination, int Source);
void CondMoveLessEqual(int Destination, int Source);
void CompConstToVariable(uint32_t Const, void * Variable, const char * VariableName);
void CompConstHalfToVariable(uint16_t Const, void * Variable, const char * VariableName);
void CompConstToX86reg(int x86Reg, uint32_t Const);
void CompX86regToVariable(int x86Reg, void * Variable, const char * VariableName);
void CompVariableToX86reg(int x86Reg, void * Variable, const char * VariableName);
void CompX86RegToX86Reg(int Destination, int Source);
void Cwd(void);
void Cwde(void);
void DecX86reg(int x86Reg);
void DivX86reg(int x86reg);
void idivX86reg(int x86reg);
void imulX86reg(int x86reg);
void ImulX86RegToX86Reg(int Destination, int Source);
void IncX86reg(int x86Reg);
void JaeLabel32(const char * Label, uint32_t Value);
void JaLabel8(const char * Label, uint8_t Value);
void JaLabel32(const char * Label, uint32_t Value);
void JbLabel8(const char * Label, uint8_t Value);
void JbLabel32(const char * Label, uint32_t Value);
void JeLabel8(const char * Label, uint8_t Value);
void JeLabel32(const char * Label, uint32_t Value);
void JgeLabel8(const char * Label, uint8_t Value);
void JgeLabel32(const char * Label, uint32_t Value);
void JgLabel8(const char * Label, uint8_t Value);
void JgLabel32(const char * Label, uint32_t Value);
void JleLabel8(const char * Label, uint8_t Value);
void JleLabel32(const char * Label, uint32_t Value);
void JlLabel8(const char * Label, uint8_t Value);
void JlLabel32(const char * Label, uint32_t Value);
void JumpX86Reg(int x86reg);
void JmpLabel8(const char * Label, uint8_t Value);
void JmpLabel32(const char * Label, uint32_t Value);
void JneLabel8(const char * Label, uint8_t Value);
void JneLabel32(const char * Label, uint32_t Value);
void JnsLabel8(const char * Label, uint8_t Value);
void JnsLabel32(const char * Label, uint32_t Value);
void JsLabel32(const char * Label, uint32_t Value);
void LeaSourceAndOffset(int x86DestReg, int x86SourceReg, size_t offset);
void MoveConstByteToN64Mem(uint8_t Const, int AddrReg);
void MoveConstHalfToN64Mem(uint16_t Const, int AddrReg);
void MoveConstByteToVariable(uint8_t Const, void * Variable, const char * VariableName);
void MoveConstHalfToVariable(uint16_t Const, void * Variable, const char * VariableName);
void MoveConstToN64Mem(uint32_t Const, int AddrReg);
void MoveConstToN64MemDisp(uint32_t Const, int AddrReg, uint8_t Disp);
void MoveConstToVariable(uint32_t Const, void * Variable, const char * VariableName);
void MoveConstToX86reg(uint32_t Const, int x86reg);
void MoveOffsetToX86reg(size_t Const, const char * VariableName, int x86reg);
void MoveX86regByteToX86regPointer(int Source, int AddrReg);
void MoveX86regHalfToX86regPointer(int Source, int AddrReg);
void MoveX86regHalfToX86regPointerDisp(int Source, int AddrReg, uint8_t Disp);
void MoveX86regToX86regPointer(int Source, int AddrReg);
void MoveX86RegToX86regPointerDisp(int Source, int AddrReg, uint8_t Disp);
void MoveX86regPointerToX86regByte(int Destination, int AddrReg);
void MoveX86regPointerToX86regHalf(int Destination, int AddrReg);
void MoveX86regPointerToX86reg(int Destination, int AddrReg);
void MoveN64MemDispToX86reg(int x86reg, int AddrReg, uint8_t Disp);
void MoveN64MemToX86reg(int x86reg, int AddrReg);
void MoveN64MemToX86regByte(int x86reg, int AddrReg);
void MoveN64MemToX86regHalf(int x86reg, int AddrReg);
void MoveX86regByteToN64Mem(int x86reg, int AddrReg);
void MoveX86regByteToVariable(int x86reg, void * Variable, const char * VariableName);
void MoveX86regHalfToN64Mem(int x86reg, int AddrReg);
void MoveX86regHalfToVariable(int x86reg, void * Variable, const char * VariableName);
void MoveX86regToN64Mem(int x86reg, int AddrReg);
void MoveX86regToN64MemDisp(int x86reg, int AddrReg, uint8_t Disp);
void MoveX86regToVariable(int x86reg, void * Variable, const char * VariableName);
void MoveX86RegToX86Reg(int Source, int Destination);
void MoveVariableToX86reg(void * Variable, const char * VariableName, int x86reg);
void MoveVariableToX86regByte(void * Variable, const char * VariableName, int x86reg);
void MoveVariableToX86regHalf(void * Variable, const char * VariableName, int x86reg);
void MoveSxX86RegHalfToX86Reg(int Source, int Destination);
void MoveSxX86RegPtrDispToX86RegHalf(int AddrReg, uint8_t Disp, int Destination);
void MoveSxN64MemToX86regByte(int x86reg, int AddrReg);
void MoveSxN64MemToX86regHalf(int x86reg, int AddrReg);
void MoveSxVariableToX86regByte(void * Variable, const char * VariableName, int x86reg);
void MoveSxVariableToX86regHalf(void * Variable, const char * VariableName, int x86reg);
void MoveZxX86RegHalfToX86Reg(int Source, int Destination);
void MoveZxX86RegPtrDispToX86RegHalf(int AddrReg, uint8_t Disp, int Destination);
void MoveZxN64MemToX86regByte(int x86reg, int AddrReg);
void MoveZxN64MemToX86regHalf(int x86reg, int AddrReg);
void MoveZxVariableToX86regByte(void * Variable, const char * VariableName, int x86reg);
void MoveZxVariableToX86regHalf(void * Variable, const char * VariableName, int x86reg);
void MulX86reg(int x86reg);
void NegateX86reg(int x86reg);
void NotX86reg(int x86reg);
void OrConstToVariable(uint32_t Const, void * Variable, const char * VariableName);
void OrConstToX86Reg(uint32_t Const, int x86Reg);
void OrVariableToX86Reg(void * Variable, const char * VariableName, int x86Reg);
void OrVariableToX86regHalf(void * Variable, const char * VariableName, int x86Reg);
void OrX86RegToVariable(void * Variable, const char * VariableName, int x86Reg);
void OrX86RegToX86Reg(int Destination, int Source);
void Popad(void);
void Pushad(void);
void Push(int x86reg);
void Pop(int x86reg);
void PushImm32(const char * String, uint32_t Value);
void Ret(void);
void Seta(int x86reg);
void Setae(int x86reg);
void Setl(int x86reg);
void Setb(int x86reg);
void Setg(int x86reg);
void Setz(int x86reg);
void Setnz(int x86reg);
void SetlVariable(void * Variable, const char * VariableName);
void SetleVariable(void * Variable, const char * VariableName);
void SetgVariable(void * Variable, const char * VariableName);
void SetgeVariable(void * Variable, const char * VariableName);
void SetbVariable(void * Variable, const char * VariableName);
void SetaVariable(void * Variable, const char * VariableName);
void SetzVariable(void * Variable, const char * VariableName);
void SetnzVariable(void * Variable, const char * VariableName);
void ShiftLeftSign(int x86reg);
void ShiftLeftSignImmed(int x86reg, uint8_t Immediate);
void ShiftLeftSignVariableImmed(void * Variable, const char * VariableName, uint8_t Immediate);
void ShiftRightSignImmed(int x86reg, uint8_t Immediate);
void ShiftRightSignVariableImmed(void * Variable, const char * VariableName, uint8_t Immediate);
void ShiftRightUnsign(int x86reg);
void ShiftRightUnsignImmed(int x86reg, uint8_t Immediate);
void ShiftRightUnsignVariableImmed(void * Variable, const char * VariableName, uint8_t Immediate);
void ShiftLeftDoubleImmed(int Destination, int Source, uint8_t Immediate);
void ShiftRightDoubleImmed(int Destination, int Source, uint8_t Immediate);
void SubConstFromVariable(uint32_t Const, void * Variable, const char * VariableName);
void SubConstFromX86Reg(int x86Reg, uint32_t Const);
void SubVariableFromX86reg(int x86reg, void * Variable, const char * VariableName);
void SubX86RegToX86Reg(int Destination, int Source);
void SubX86regFromVariable(int x86reg, void * Variable, const char * VariableName);
void SbbX86RegToX86Reg(int Destination, int Source);
void TestConstToVariable(uint32_t Const, void * Variable, const char * VariableName);
void TestConstToX86Reg(uint32_t Const, int x86reg);
void TestX86RegToX86Reg(int Destination, int Source);
void XorConstToX86Reg(int x86Reg, uint32_t Const);
void XorX86RegToX86Reg(int Source, int Destination);
void XorVariableToX86reg(void * Variable, const char * VariableName, int x86reg);
void XorX86RegToVariable(void * Variable, const char * VariableName, int x86reg);
void XorConstToVariable(void * Variable, const char * VariableName, uint32_t Const);

#define _MMX_SHUFFLE(a, b, c, d) \
    ((uint8_t)(((a) << 6) | ((b) << 4) | ((c) << 2) | (d)))

void MmxMoveRegToReg(int Dest, int Source);
void MmxMoveQwordRegToVariable(int Dest, void * Variable, const char * VariableName);
void MmxMoveQwordVariableToReg(int Dest, void * Variable, const char * VariableName);
void MmxPandRegToReg(int Dest, int Source);
void MmxPandnRegToReg(int Dest, int Source);
void MmxPandVariableToReg(void * Variable, const char * VariableName, int Dest);
void MmxPorRegToReg(int Dest, int Source);
void MmxPorVariableToReg(void * Variable, const char * VariableName, int Dest);
void MmxXorRegToReg(int Dest, int Source);
void MmxShuffleMemoryToReg(int Dest, void * Variable, const char * VariableName, uint8_t Immed);
void MmxPcmpeqwRegToReg(int Dest, int Source);
void MmxPmullwRegToReg(int Dest, int Source);
void MmxPmullwVariableToReg(int Dest, void * Variable, const char * VariableName);
void MmxPmulhuwRegToReg(int Dest, int Source);
void MmxPmulhwRegToReg(int Dest, int Source);
void MmxPmulhwRegToVariable(int Dest, void * Variable, const char * VariableName);
void MmxPsrlwImmed(int Dest, uint8_t Immed);
void MmxPsrawImmed(int Dest, uint8_t Immed);
void MmxPsllwImmed(int Dest, uint8_t Immed);
void MmxPaddswRegToReg(int Dest, int Source);
void MmxPaddswVariableToReg(int Dest, void * Variable, const char * VariableName);
void MmxPaddwRegToReg(int Dest, int Source);
void MmxPsubswVariableToReg(int Dest, void * Variable, const char * VariableName);
void MmxPsubswRegToReg(int Dest, int Source);
void MmxPackSignedDwords(int Dest, int Source);
void MmxUnpackLowWord(int Dest, int Source);
void MmxUnpackHighWord(int Dest, int Source);
void MmxCompareGreaterWordRegToReg(int Dest, int Source);
void MmxEmptyMultimediaState(void);

void SseMoveAlignedVariableToReg(void * Variable, const char * VariableName, int sseReg);
void SseMoveAlignedRegToVariable(int sseReg, void * Variable, const char * VariableName);
void SseMoveAlignedN64MemToReg(int sseReg, int AddrReg);
void SseMoveAlignedRegToN64Mem(int sseReg, int AddrReg);
void SseMoveUnalignedVariableToReg(void * Variable, const char * VariableName, int sseReg);
void SseMoveUnalignedRegToVariable(int sseReg, void * Variable, const char * VariableName);
void SseMoveUnalignedN64MemToReg(int sseReg, int AddrReg);
void SseMoveUnalignedRegToN64Mem(int sseReg, int AddrReg);
void SseMoveRegToReg(int Dest, int Source);
void SseXorRegToReg(int Dest, int Source);

#pragma warning(push)
#pragma warning(disable : 4201) // Non-standard extension used: nameless struct/union

typedef union
{
    struct
    {
        unsigned Reg0 : 2;
        unsigned Reg1 : 2;
        unsigned Reg2 : 2;
        unsigned Reg3 : 2;
    };
    unsigned UB : 8;
} SHUFFLE;

#pragma warning(pop)

void SseShuffleReg(int Dest, int Source, uint8_t Immed);

void x86_SetBranch32b(void * Jumpuint8_t, void * Destination);
void x86_SetBranch8b(void * Jumpuint8_t, void * Destination);

void * GetAddressOf_(int32_t value, ...);
#define AddressOf(Addr) GetAddressOf_(5, (Addr))

#endif

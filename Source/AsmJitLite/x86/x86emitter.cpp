#include "x86assembler.h"
#include "x86emitter.h"
#include "x86operand.h"
#include "X86BufferWriter.h"
#include <cstdarg>
#include <stdio.h>
#include <Common/Platform.h>
#include <Common/StdString.h>

int _vscprintf(const char * format, va_list pargs);

ASMJIT_BEGIN_SUB_NAMESPACE(x86)

static ASMJIT_FORCE_INLINE uint32_t x86EncodeMod(uint32_t m, uint32_t o, uint32_t rm) noexcept {
    ASMJIT_ASSERT(m <= 3);
    ASMJIT_ASSERT(o <= 7);
    ASMJIT_ASSERT(rm <= 7);
    return (m << 6) + (o << 3) + rm;
}

//! Memory operand's info bits.
//!
//! A lookup table that contains various information based on the BASE and INDEX information of a memory operand. This
//! is much better and safer than playing with IFs in the code and can check for errors must faster and better.
enum X86MemInfo_Enum {
    kX86MemInfo_0 = 0x00,

    kX86MemInfo_BaseGp = 0x01, //!< Has BASE reg, REX.B can be 1, compatible with REX.B byte.
    kX86MemInfo_Index = 0x02, //!< Has INDEX reg, REX.X can be 1, compatible with REX.X byte.

    kX86MemInfo_BaseLabel = 0x10, //!< Base is Label.
    kX86MemInfo_BaseRip = 0x20, //!< Base is RIP.

    kX86MemInfo_67H_X86 = 0x40, //!< Address-size override in 32-bit mode.
    kX86MemInfo_67H_X64 = 0x80, //!< Address-size override in 64-bit mode.
    kX86MemInfo_67H_Mask = 0xC0  //!< Contains all address-size override bits.
};

void Internal_CPU_Message(Assembler * _emitter, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    int nlen = _vscprintf(format, args) + 1;
    if (nlen > 0) 
    {
        char* buffer = static_cast<char*>(alloca(nlen * sizeof(char)));
        if (buffer != nullptr)
        {
            int result = vsnprintf(buffer, nlen, format, args);
            if (result >= 0 && result < nlen) 
            {
                _emitter->_logger->_log(buffer, result);
            }
        }
    }
    va_end(args);
};


#define CPU_Message(format, ...) \
do { \
    if (_emitter->_logger != nullptr) { \
        Internal_CPU_Message(_emitter, (format), ##__VA_ARGS__); \
    } \
} while (0)

template<>
Error EmitterExplicitT<Assembler>::adc(Gp const & /*o0*/, Gp const & /*o1*/)
{
    __debugbreak();
    return 0;
}

template<>
Error EmitterExplicitT<Assembler>::adc(Gp const & /*o0*/, Mem const & /*o1*/)
{
    __debugbreak();
    return 0;
}

template<>
Error EmitterExplicitT<Assembler>::adc(Gp const & /*o0*/, Imm const & /*o1*/)
{
    __debugbreak();
    return 0;
}

template<>
Error EmitterExplicitT<Assembler>::add(Gp const & o0, Gp const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && o1.size() == 4)
    {
        CPU_Message("add %s, %s", x86_Name(o0), x86_Name(o1));
        writer.emit16(0xC003 + (o0.id() << 11) + (o1.id() << 8));
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

template<>
Error EmitterExplicitT<Assembler>::add(Gp const & o0, Mem const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && !o1.hasBase() && !o1.hasIndex() && !o1.hasShift())
    {
        CPU_Message("add %s, dword ptr [0x%X]", x86_Name(o0), (uint32_t)o1.offsetLo32());
        writer.emit16(0x0503 | (o0.id() << 11));
        writer.emit32((uint32_t)o1.offsetLo32());
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

template<>
Error EmitterExplicitT<Assembler>::add(Gp const & o0, Imm const & o1)
{
    X86BufferWriter writer(_emitter);
    int32_t immValue = (uint32_t)o1.value();
    if (o0.size() == 4)
    {
        CPU_Message("add %s, 0x%X", x86_Name(o0), immValue);
        if (Support::isInt8(immValue))
        {
            writer.emit16(0xC083 + (o0.id() << 8));
            writer.emit8(immValue);
            return kErrorOk;
        }
        writer.emit16(0xC081 + (o0.id() << 8));
        writer.emit32(immValue);
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::add(Mem const & o0, Imm const & o1)
{
    X86BufferWriter writer(_emitter);
    if (!o0.hasBase() && !o0.hasIndex() && !o0.hasShift())
    {
        CPU_Message("add dword ptr [0x%X], 0x%X", (uint32_t)o0.offsetLo32(), (uint32_t)o1.value());
        writer.emit16(0x0581);
        writer.emit32((uint32_t)o0.offsetLo32());
        writer.emit32((uint32_t)o1.value());
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::and_(Gp const & /*o0*/, Gp const & /*o0*/)
{
    int a = 5;
    a = 66;
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::and_(Gp const & o0, Mem const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && !o1.hasBase() && !o1.hasIndex() && !o1.hasShift())
    {
        CPU_Message("and %s, dword ptr [0x%X]", x86_Name(o0), (uint32_t)o1.offsetLo32());
        writer.emit16(0x0523 + (o0.id() << 11));
        writer.emit32(o1.offsetLo32());
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::and_(Gp const & o0, Imm const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4)
    {
        int32_t immValue = (uint32_t)o1.value();
        CPU_Message("and %s, %Xh", x86_Name(o0), immValue);
        if (Support::isInt8(immValue))
        {
            writer.emit8(0x83);
            writer.emit8(x86EncodeMod(3, 4, o0.id()));
            writer.emit8((uint8_t)immValue);
        }
        else
        {
            writer.emit8(0x81);
            writer.emit8(x86EncodeMod(3, 4, o0.id()));
            writer.emit32(immValue);
        }
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::and_(Mem const & o0, Imm const & o1)
{
    X86BufferWriter writer(_emitter);
    if (!o0.hasBase() && !o0.hasIndex() && !o0.hasShift())
    {
        int32_t immValue = (uint32_t)o1.value();
        CPU_Message("and dword ptr [0x%X], 0x%X", (uint32_t)o0.offsetLo32(), immValue);
        if (Support::isInt8(immValue))
        {
            writer.emit16(0x2580);
            writer.emit32((uint32_t)o0.offsetLo32());
            writer.emit8((uint8_t)immValue);
            return kErrorOk;
        }
        writer.emit16(0x2581);
        writer.emit32((uint32_t)o0.offsetLo32());
        writer.emit32(immValue);
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::call(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::call(Imm const & o0)
{
    X86BufferWriter writer(_emitter);
    CodeHolder * _code = _emitter->code();
    uint64_t baseAddress = _code->baseAddress();
    uint64_t jumpAddress = o0.value();
    CPU_Message("call offset 0x%X", (uint32_t)jumpAddress);
    if (baseAddress != Globals::kNoBaseAddress) {
        __debugbreak();
        return kErrorInvalidInstruction;
    }
    
    RelocEntry * re = nullptr;
    Error err = _code->newRelocEntry(&re, RelocType::kAbsToRel);
    if (ASMJIT_UNLIKELY(err))
    {
        __debugbreak();
        return kErrorInvalidInstruction;
    }
    re->_sourceOffset = _emitter->offset();
    re->_sourceSectionId = _emitter->_section->id();
    re->_payload = jumpAddress;
    re->_format.resetToSimpleValue(OffsetType::kSignedOffset, 4);
    writer.emit8(0xE8);
    re->_format.setLeadingAndTrailingSize(writer.offsetFrom(_emitter->_bufferPtr), 0);
    writer.emit32(0);
    return kErrorOk;
}

Error EmitterExplicitT<Assembler>::cmp(Gp const & o0, Gp const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && o1.size() == 4)
    {
        CPU_Message("cmp %s, %s", x86_Name(o0), x86_Name(o1));
        writer.emit16(0xC03B + (o0.id() << 11) + (o1.id() << 8));
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidInstruction;
}

Error EmitterExplicitT<Assembler>::cmp(Gp const & o0, Mem const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && !o1.hasBase() && !o1.hasIndex() && !o1.hasShift())
    {
        CPU_Message("cmp %s, dword ptr [0x%X]", x86_Name(o0), (uint32_t)o1.offsetLo32());
        writer.emit16(0x053B + (o0.id() << 11));
        writer.emit32(o1.offsetLo32());
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidInstruction;
}

Error EmitterExplicitT<Assembler>::cmp(Gp const & o0, Imm const & o1)
{
    X86BufferWriter writer(_emitter);
    int32_t immValue = (uint32_t)o1.value();
    if (o0.size() == 4)
    {
        CPU_Message("cmp %s, %Xh", x86_Name(o0), immValue);
        if (Support::isInt8(immValue))
        {
            writer.emit16(0xF883 + (o0.id() << 8));
            writer.emit8((int8_t)immValue);
            return kErrorOk;
        }
        else
        {
            __debugbreak();
            return kErrorInvalidInstruction;
        }
    }
    __debugbreak();
    return kErrorInvalidInstruction;
}

Error EmitterExplicitT<Assembler>::cmp(Mem const & o0, Imm const & o1) 
{
    X86BufferWriter writer(_emitter);
    if (!o0.hasBase() && !o0.hasIndex() && !o0.hasShift())
    {
        int32_t immValue = (uint32_t)o1.value();
        CPU_Message("cmp dword ptr [0x%X], 0x%X", (uint32_t)o0.offsetLo32(), immValue);
        if (Support::isInt8(immValue))
        {
            writer.emit16(0x3D80);
            writer.emit32((uint32_t)o0.offsetLo32());
            writer.emit8((uint8_t)immValue);
            return kErrorOk;
        }
        writer.emit16(0x3D81);
        writer.emit32((uint32_t)o0.offsetLo32());
        writer.emit32(immValue);
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidInstruction;
}

Error EmitterExplicitT<Assembler>::dec(Gp const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::inc(Gp const & o0)
{
    X86BufferWriter writer(_emitter);

    if (o0.size() == 4)
    {
        CPU_Message("inc %s", x86_Name(o0));
        if (o0.id() == Gp::kIdSp)
        {
            writer.emit8(0x44);
        }
        else if (o0.id() == Gp::kIdBp)
        {
            writer.emit8(0x45);
        }
        else
        {
            writer.emit16(0xC0FF | (o0.id() << 8));
        }
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidInstruction;
}

Error EmitterExplicitT<Assembler>::j(CondCode /*cc*/, const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::ja(const Label & o0)
{
    CPU_Message("ja L%u", o0.id());
    return emitJump(0x870F, 2, o0);
}

Error EmitterExplicitT<Assembler>::jae(const Label & /*o0*/)
{
    int a = 5;
    a = 7;
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jb(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jbe(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jc(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::je(const Label & o0)
{
    CPU_Message("je L%u", o0.id());
    return emitJump(0x840F, 2, o0);
}

Error EmitterExplicitT<Assembler>::jg(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jge(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jl(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jle(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jna(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jnae(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jnb(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jnbe(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jnc(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jne(const Label & o0) 
{
    CPU_Message("jne L%u", o0.id());
    return emitJump(0x850F, 2, o0);
}

Error EmitterExplicitT<Assembler>::jng(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jnge(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jnl(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jnle(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jno(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jnp(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jns(const Label & o0)
{
    CPU_Message("jns L%u", o0.id());
    return emitJump(0x890F, 2, o0);
}

Error EmitterExplicitT<Assembler>::jnz(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jo(const Label & o0)
{
    CPU_Message("jo L%u", o0.id());
    return emitJump(0x800F, 2, o0);
}

Error EmitterExplicitT<Assembler>::jp(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jpe(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jpo(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::js(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jz(const Label & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::jmp(const Label & o0)
{
    CPU_Message("jmp L%u", o0.id());
    return emitJump(0xE9, 1, o0);
}

Error EmitterExplicitT<Assembler>::lea(Gp const & o0, Mem const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && o1.hasBase())
    {
        if (o1.baseId() == Gp::kIdSp)
        {
            __debugbreak();
            return kErrorInvalidInstruction;
        }
        if (o1.baseId() != Gp::kIdBp && o1.offsetLo32() == 0)
        {
            __debugbreak();
            return kErrorInvalidInstruction;
        }
        if (Support::isInt8(o1.offsetLo32()))
        {
            CPU_Message("lea %s, [%s+0x%X]", x86_Name(o0), x86_Name(Gpd(o1.baseId())), (uint8_t)o1.offsetLo32());
            writer.emit16(0x408D + (o0.id() << 11) + (o1.baseId() << 8));
            writer.emit8(o1.offsetLo32());
            return kErrorOk;
        }
        __debugbreak();
        return kErrorInvalidInstruction;
    }
    __debugbreak();
    return kErrorInvalidInstruction;
}

Error EmitterExplicitT<Assembler>::mov(Gp const & o0, Gp const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && o1.size() == 4)
    {
        CPU_Message("mov %s, %s", x86_Name(o0), x86_Name(o1));
        writer.emit16(0xC089 + (o0.id() << 8) + (o1.id() << 11));
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidInstruction;
}

Error EmitterExplicitT<Assembler>::mov(Gp const & o0, Mem const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && !o1.hasBase() && !o1.hasIndex() && !o1.hasShift())
    {
        CPU_Message("mov %s, dword ptr [0x%X]", x86_Name(o0), (uint32_t)o1.offsetLo32());
        if (o0.id() == Gp::kIdAx)
        {
            writer.emit8(0xA1);
            writer.emit32(o1.offsetLo32());
            return kErrorOk;
        }
        writer.emit8(0x8b);
        writer.emit8(x86EncodeMod(0, o0.id(), 5));
        writer.emit32(o1.offsetLo32());
        return kErrorOk;
    }
    if (o0.size() == 4 && !o1.hasBase() && o1.hasIndex())
    {
        CPU_Message("mov %s, dword ptr [0x%X+%s%s]", x86_Name(o0), (uint32_t)o1.offsetLo32(), x86_Name(Gpd(o1.id())), o1.hasShift() ? stdstr_f("*%i", o1.shift() << 1).c_str() : "");

        writer.emit16(0x048B | (o0.id() << 11));
        writer.emit8(0x05 | (o1.indexId() << 3) | ((o1.hasShift() ? o1.shift() : 0) << 6));
        writer.emit32((uint32_t)o1.offsetLo32());
        return kErrorOk;
    }
    if (o0.size() == 4 && o1.hasBase() && o1.hasIndex() && !o1.hasShift())
    {
        if (o1.id() == Gp::kIdSp || o1.id() == Gp::kIdBp)
        {
            __debugbreak();
            return kErrorInvalidRegType;
        }
        CPU_Message("mov %s, dword ptr [%s+%s]", x86_Name(o0), x86_Name(Gpd(o1.id())), x86_Name(Gpd(o1.indexId())));
        writer.emit16(0x048B | (o0.id() << 11));
        writer.emit8(o1.id() | (o1.indexId() << 3));
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::mov(Gp const & o0, Imm const & o1)
{
    X86BufferWriter writer(_emitter);
    int32_t immValue = (uint32_t)o1.value();
    CPU_Message("mov %s, %Xh", x86_Name(o0), immValue);

    if (o0.size() == 4)
    {
        if (Support::isInt8(immValue))
        {
            __debugbreak();
            return kErrorInvalidRegType;
        }
        writer.emit16(0xC0C7 | (o0.id() << 8));
        writer.emit32(immValue);
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::mov(Mem const & o0, Gp const & o1)
{
    X86BufferWriter writer(_emitter);

    if (o0.hasBase() && !o0.hasIndex() && !o0.hasShift() && o1.size() == 4 )
    {
        CPU_Message("mov dword ptr [%s%s], %s", x86_Name(Gpd(o0.id())), (uint8_t)o0.offsetLo32() != 0 ? stdstr_f("+0x%X",(uint8_t)o0.offsetLo32()).c_str() : "", x86_Name(o1));
        writer.emit16(0x4089 | (o1.id() << 11) | (o0.id() << 8));
        writer.emit8((uint8_t)o0.offsetLo32());
        return kErrorOk;
    }
    if (o0.hasBase() && o0.hasIndex() && !o0.hasShift() && o1.size() == 4)
    {
        __debugbreak();
        return kErrorInvalidRegType;
    }
    if (!o0.hasBase() && !o0.hasIndex() && o1.size() == 4)
    {
        CPU_Message("mov dword ptr [0x%X], %s", (uint32_t)o0.offsetLo32(), x86_Name(o1));
        writer.emit16(0x0589 | (o1.id() << 11));
        writer.emit32((uint32_t)o0.offsetLo32());
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::mov(Mem const & o0, Imm const & o1)
{
    X86BufferWriter writer(_emitter);
    if (!o0.hasBase() && !o0.hasIndex() && !o0.hasShift())
    {
        CPU_Message("mov dword ptr [0x%X], 0x%X", (uint32_t)o0.offsetLo32(), (uint32_t)o1.value());
        writer.emit16(0x05C7);
        writer.emit32((uint32_t)o0.offsetLo32());
        writer.emit32((uint32_t)o1.value());
        return kErrorOk;
    }
    else if (o0.hasBase() && o0.hasIndex() && !o0.hasShift() && o0.size() == 4)
    {
        CPU_Message("mov dword ptr [%s+%s],%Xh", x86_Name(Gpd(o0.baseId())), x86_Name(Gpd(o0.indexId())), (uint32_t)o1.value());
        writer.emit16(0x04C7);
        writer.emit8(o0.baseId() + (o0.indexId() << 3));
        writer.emit32((uint32_t)o1.value());
        return kErrorOk;
    }
    else if (o0.hasBase() && !o0.hasIndex() && !o0.hasShift() && o0.size() == 4)
    {
        CPU_Message("mov dword ptr [%s+%Xh], %Xh", x86_Name(Gpd(o0.baseId())), o0.offsetLo32(), (uint32_t)o1.value());
        if (Support::isInt8(o0.offsetLo32()))
        {
            writer.emit16(0x40C7 + (o0.baseId() << 8));
            writer.emit8((int8_t)(o0.offsetLo32()));
        }
        else
        {
            writer.emit16(0x80C7 + (o0.baseId() << 8));
            writer.emit32(o0.offsetLo32());
        }
        writer.emit32((uint32_t)o1.value());
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::movsx(Gp const & /*o0*/, Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::movzx(Gp const & /*o0*/, Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::not_(Gp const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::or_(Gp const & o0, Gp const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && o1.size() == 4)
    {
        CPU_Message("or %s, %s", x86_Name(o0), x86_Name(o1));
        writer.emit16(0xC00B + (o0.id() << 11) + (o1.id() << 8));
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::or_(Gp const & /*o0*/, Mem const & /*o0*/)
{
    int a = 71;
    a = 71;
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::or_(Gp const & o0, Imm const & o1)
{
    if (o1.value() == 0)
    {
        return kErrorOk;
    }
    int32_t immValue = (int32_t)o1.value();
    CPU_Message("or %s, %Xh", x86_Name(o0), immValue);
    X86BufferWriter writer(_emitter);
    if (Support::isInt8(immValue))
    {
        writer.emit16(0xC883 | (o0.id() << 8));
        writer.emit8((uint8_t)immValue);
    }
    else
    {
        writer.emit16(0xC881 | (o0.id() << 8));
        writer.emit32(immValue);
    }
    return kErrorOk;
}

Error EmitterExplicitT<Assembler>::or_(Mem const & /*o0*/, Gp const & /*o0*/)
{
    int a = 73;
    a = 73;
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::or_(Mem const & /*o0*/, Imm const & /*o1*/)
{
    int a = 4;
    a = 74;
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::pop(Gp const & o0)
{
    CPU_Message("pop %s", x86_Name(o0));
    X86BufferWriter writer(_emitter);

    if (o0.size() == 4)
    {
        writer.emit8(0x58 | o0.id());
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::popad(void)
{
    CPU_Message("pushad");
    X86BufferWriter writer(_emitter);
    writer.emit8(0x61);
    return kErrorOk;
}

Error EmitterExplicitT<Assembler>::push(Gp const & o0)
{
    CPU_Message("push %s", x86_Name(o0));
    X86BufferWriter writer(_emitter);

    if (o0.size() == 4 || o0.size() == 8)
    {
        writer.emit8(0x50 | o0.id());
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::push(Imm const & o0)
{
    X86BufferWriter writer(_emitter);
    int32_t immValue = (int32_t)o0.value();
    CPU_Message("push 0x%X", (uint32_t)(immValue));
    if (Support::isInt8(immValue) == 1)
    {
        writer.emit8(0x6A);
        writer.emit8((int8_t)(immValue));
    }
    else
    {
        writer.emit8(0x68);
        writer.emit32(immValue);
    }
    return kErrorOk;
}

Error EmitterExplicitT<Assembler>::pushad()
{
    CPU_Message("pushad");
    X86BufferWriter writer(_emitter);
    writer.emit8(0x60);
    return kErrorOk;
}

Error EmitterExplicitT<Assembler>::sbb(Gp const & /*o0*/, Gp const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::sbb(Gp const & /*o0*/, Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::sbb(Gp const & /*o0*/, Imm const & /*o1*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::sar(Gp const & /*o0*/, Gp const & /*o1*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::sar(Gp const & o0, Imm const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4)
    {
        uint8_t immValue = (uint8_t)o1.value();
        CPU_Message("sar %s, %Xh", x86_Name(o0), immValue);
        writer.emit16(0xF8C1 | (o0.id() << 8));
        writer.emit8((uint8_t)immValue);
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::seta(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::setb(Mem const & o0)
{
    X86BufferWriter writer(_emitter);
    if (!o0.hasBase() && !o0.hasIndex() && !o0.hasShift())
    {
        CPU_Message("setb byte ptr [0x%X]", (uint32_t)o0.offsetLo32());
        writer.emit16(0x920F);
        writer.emit8(0x05);
        writer.emit32((uint32_t)o0.offsetLo32());
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::setg(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::setl(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}


Error EmitterExplicitT<Assembler>::setg(Gp const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::setl(Gp const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::setnz(Gp const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::shl(Gp const & o0, Gp const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && o1.size() == 1 && o1.isGpbLo() && o1.id() == Gp::kIdCx)
    {
        CPU_Message("shl %s, cl", x86_Name(o0));
        writer.emit16(0xE0D3 + (o0.id() << 8));
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::shl(Gp const & /*o0*/, Imm const & /*o1*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::shr(Gp const & o0, Gp const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && o1.size() == 1 && o1.isGpbLo() && o1.id() == Gp::kIdCx)
    {
        CPU_Message("shr %s, cl", x86_Name(o0));
        writer.emit16(0xE8D3 + (o0.id() << 8));
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::shr(Gp const & o0, Imm const & o1)
{
    X86BufferWriter writer(_emitter);
    uint32_t immValue = (uint32_t)o1.value();
    if (o0.size() == 4)
    {
        CPU_Message("shr %s, %Xh", x86_Name(o0), immValue);
        writer.emit16(0xE8C1 + (o0.id() << 8));
        writer.emit8((uint8_t)immValue);
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::shld(Gp const & /*o0*/, Gp const & /*o1*/, Gp const & /*o2*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::shld(Gp const & /*o0*/, Gp const & /*o1*/, Imm const & /*o2*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::shrd(Gp const & /*o0*/, Gp const & /*o1*/, Gp const & /*o2*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::shrd(Gp const & /*o0*/, Gp const & /*o1*/, Imm const & /*o1*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::sub(Gp const & o0, Gp const & o1)
{
    X86BufferWriter writer(_emitter);
    CPU_Message("sub %s, %s", x86_Name(o0), x86_Name(o1));
    if (o0.size() == 4 && o1.size() == 4)
    {
        writer.emit16(0xC02B + (o0.id() << 11) + (o1.id() << 8));
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::sub(Gp const & /*o0*/, Mem const & /*o0*/)
{
    int a = 5; 
    a = 7774;
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::sub(Gp const & /*o0*/, Imm const & /*o1*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::sub(Mem const & o0, Imm const & o1)
{
    X86BufferWriter writer(_emitter);
    if (!o0.hasBase() && !o0.hasIndex() && !o0.hasShift())
    {
        int32_t immValue = (uint32_t)o1.value();
        CPU_Message("sub dword ptr [0x%X], 0x%X", (uint32_t)o0.offsetLo32(), immValue);
        if (Support::isInt8(immValue))
        {
            writer.emit16(0x2D83);
            writer.emit32((uint32_t)o0.offsetLo32());
            writer.emit8((uint8_t)immValue);
            return kErrorOk;
        }
        writer.emit16(0x2D81);
        writer.emit32((uint32_t)o0.offsetLo32());
        writer.emit32(immValue);
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::test(Gp const & o0, Gp const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 1 && o1.size() == 1)
    {
        CPU_Message("test %s, %s", x86_Name(o0), x86_Name(o1));

        writer.emit8(0x84);
        writer.emit8(x86EncodeMod(3, o0.id(), o1.id()));
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::test(Gp const & o0, Imm const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4)
    {
        uint32_t immValue = (uint32_t)o1.value();
        CPU_Message("test %s, 0x%X", x86_Name(o0), immValue);
        if (o0.id() == Gp::kIdAx)
        {
            writer.emit8(0xA9);
        }
        else
        {
            writer.emit16(0xC0F7 + (o0.id() << 8));
        }
        writer.emit32(immValue);
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::test(Mem const & o0, Imm const & o1)
{
    X86BufferWriter writer(_emitter);
    if (!o0.hasBaseOrIndex())
    {
        CPU_Message("test dword ptr ds:[0x%X], 0x%X", o0.offsetLo32(), (uint32_t)(o1.value()));
        writer.emit16(0x05F7);
        writer.emit32(o0.offsetLo32());
        writer.emit32((uint32_t)(o1.value()));
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::xor_(Gp const & o0, Gp const & o1)
{    
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && o1.size() == 4)
    {
        CPU_Message("xor %s, %s", x86_Name(o0), x86_Name(o1));
        writer.emit16(0xC031 | (o0.id() << 8) | (o1.id() << 11));
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidInstruction;
}

Error EmitterExplicitT<Assembler>::xor_(Gp const & o0, Mem const & o1)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4 && !o1.hasBase() && !o1.hasIndex() && !o1.hasShift())
    {
        CPU_Message("xor %s, dword ptr [0x%X]", x86_Name(o0), (uint32_t)o1.offsetLo32());
        writer.emit16(0x0533 | (o0.id() << 11));
        writer.emit32((uint32_t)o1.offsetLo32());
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidInstruction;
}

Error EmitterExplicitT<Assembler>::xor_(Gp const & /*o0*/, Imm const & /*o1*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::sahf(Gp const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::ldmxcsr(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::stmxcsr(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::int3()
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fabs()
{
    __debugbreak();
    return 0;
}
Error EmitterExplicitT<Assembler>::fadd(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}
Error EmitterExplicitT<Assembler>::fchs()
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fclex()
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fcomp(St const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fcomp(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fcompp()
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fdiv(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::ffree(St const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fild(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fincstp()
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::finit()
{
    X86BufferWriter writer(_emitter);
    writer.emit8(0x9B);
    writer.emit16(0xE3DB);
    return kErrorInvalidRegType;
}

Error EmitterExplicitT<Assembler>::fist(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fistp(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fld(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fld(St const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fldcw(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fmul(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fnstcw(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fsqrt()
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fst(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fstp(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fsub(Mem const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fxch(St const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fstsw(Gp const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterExplicitT<Assembler>::fnstsw(Gp const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterImplicitT<Assembler>::div(Gp const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterImplicitT<Assembler>::idiv(Gp const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterImplicitT<Assembler>::imul(Gp const & /*o0*/)
{
    int a = 5;
    a = 533;
    __debugbreak();
    return 0;
}

Error EmitterImplicitT<Assembler>::jecxz(Label const & /*o0*/)
{
    __debugbreak();
    return 0;
}

Error EmitterImplicitT<Assembler>::mul(Gp const & o0)
{
    X86BufferWriter writer(_emitter);
    if (o0.size() == 4)
    {
        CPU_Message("mul %s", x86_Name(o0));
        writer.emit16(0xE0F7 | (o0.id() << 8));
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidRegType;
}

Error EmitterImplicitT<Assembler>::ret()
{
    CPU_Message("ret");
    X86BufferWriter writer(_emitter);
    writer.emit8(0xC3);
    return kErrorOk;
}

Error EmitterImplicitT<Assembler>::sahf()
{
    __debugbreak();
    return 0;
}

template<>
Error EmitterImplicitT<Assembler>::emitJump(uint16_t Opcode, uint8_t OpcodeSize, const Label & o0)
{
    X86BufferWriter writer(_emitter);
    CodeHolder *& code = _emitter->_code;
    Section *& section = _emitter->_section;
    LabelEntry * label = code->labelEntry(o0);
    if (ASMJIT_UNLIKELY(!label))
    {
        __debugbreak();
        return kErrorInvalidInstruction;
    }

    if (!label->isBoundTo(section))
    {
        if (OpcodeSize == 1)
        {
            writer.emit8((uint8_t)Opcode);
        }
        else if (OpcodeSize == 2)
        {
            writer.emit16(Opcode);
        }
        else
        {
            __debugbreak();
            return kErrorInvalidInstruction;
        }
        size_t offset = size_t(writer.offsetFrom(_emitter->_bufferData));
        OffsetFormat of;
        of.resetToSimpleValue(OffsetType::kSignedOffset, 4);
        LabelLink* link = code->newLabelLink(label, section->id(), offset, -4, of);
        if (ASMJIT_UNLIKELY(!link))
        {
            __debugbreak();
            return kErrorOutOfMemory;
        }
        writer.emit32(0);
        return kErrorOk;
    }
    __debugbreak();
    return kErrorInvalidInstruction;
}

template<>
const char * EmitterExplicitT<Assembler>::x86_Name(const asmjit::x86::Gp & Reg)
{
    if (Reg.size() == 1 && Reg.isGpbLo())
    {
        switch (Reg.id())
        {
        case Gp::kIdAx: return "al";
        case Gp::kIdCx: return "cl";
        case Gp::kIdDx: return "dl";
        case Gp::kIdBx: return "bl";
        }
    }
    else if (Reg.size() == 4)
    {
        switch (Reg.id())
        {
        case Gp::kIdAx: return "eax";
        case Gp::kIdCx: return "ecx";
        case Gp::kIdDx: return "edx";
        case Gp::kIdBx: return "ebx";
        case Gp::kIdSp: return "esp";
        case Gp::kIdBp: return "ebp";
        case Gp::kIdSi: return "esi";
        case Gp::kIdDi: return "edi";
        }
    }
    else if (Reg.size() == 8)
    {
        switch (Reg.id())
        {
        case Gp::kIdAx: return "rax";
        case Gp::kIdCx: return "rcx";
        case Gp::kIdDx: return "rdx";
        case Gp::kIdBx: return "rbx";
        case Gp::kIdSp: return "rsp";
        case Gp::kIdBp: return "rbp";
        case Gp::kIdSi: return "rsi";
        case Gp::kIdDi: return "rdi";
        case Gp::kIdR8: return "r8";
        case Gp::kIdR9: return "r9";
        case Gp::kIdR10: return "r10";
        case Gp::kIdR11: return "r11";
        case Gp::kIdR12: return "r12";
        case Gp::kIdR13: return "r13";
        case Gp::kIdR14: return "r14";
        case Gp::kIdR15: return "r15";
        }
    }
    __debugbreak();
    return "unknown";
}

ASMJIT_END_SUB_NAMESPACE
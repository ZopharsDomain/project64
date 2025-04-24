#pragma once
#include "../core/codewriter_p.h"

ASMJIT_BEGIN_SUB_NAMESPACE(x86)

class X86BufferWriter : public CodeWriter {
public:
    ASMJIT_FORCE_INLINE explicit X86BufferWriter(Assembler* a) noexcept
        : CodeWriter(a), _a(a)
    {
        ensureSpace(a, 16);
    }

    ~X86BufferWriter()
    {
        done(_a);
    }

private:
    Assembler * _a;
};

ASMJIT_END_SUB_NAMESPACE

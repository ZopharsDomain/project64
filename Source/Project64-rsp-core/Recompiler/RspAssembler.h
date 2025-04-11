#pragma once
#if defined(__amd64__) || defined(_M_X64)

#include <Project64-rsp-core/Recompiler/asmjit.h>

class RspAssembler :
    public asmjit::x86::Assembler,
    public asmjit::ErrorHandler,
    public asmjit::Logger
{
public:
    RspAssembler(asmjit::CodeHolder * CodeHolder, std::string & CodeLog);

    void Reset(void);

private:
    RspAssembler();
    RspAssembler(const RspAssembler &);
    RspAssembler & operator=(const RspAssembler &);

    void handleError(asmjit::Error err, const char * message, asmjit::BaseEmitter * origin);
    asmjit::Error _log(const char * data, size_t size) noexcept;
    std::string & m_CodeLog;
};

#endif
#if defined(__amd64__) || defined(_M_X64)

#pragma once
#include "asmjit.h"

class CRSPSystem;
class RspAssembler;

class CRSPRecompiler :
    public asmjit::ErrorHandler
{
public:
    CRSPRecompiler(CRSPSystem & System);
    ~CRSPRecompiler();

    void Reset();
    void CompileHLETask(uint32_t Address);
    void Log(_Printf_format_string_ const char * Text, ...);

private:
    CRSPRecompiler();
    CRSPRecompiler(const CRSPRecompiler &);
    CRSPRecompiler & operator=(const CRSPRecompiler &);

    void handleError(asmjit::Error err, const char * message, asmjit::BaseEmitter * origin);

    static uintptr_t GetAddressOf(int32_t value, ...);

    CRSPSystem & m_System;
    std::string m_CodeLog;
    uint32_t m_BlockID;
    asmjit::Environment m_Environment;
    asmjit::CodeHolder m_CodeHolder;
    RspAssembler * m_Assembler;
};

#endif
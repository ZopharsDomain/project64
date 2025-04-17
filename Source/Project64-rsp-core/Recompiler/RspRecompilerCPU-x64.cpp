#if defined(__amd64__) || defined(_M_X64)

#include "RspRecompilerCPU-x64.h"
#include <Common/Log.h>
#include <Project64-rsp-core/Recompiler/RspAssembler.h>
#include <Project64-rsp-core/Recompiler/RspProfiling.h>
#include <Project64-rsp-core/Settings/RspSettings.h>
#include <Project64-rsp-core/cpu/RspMemory.h>
#include <Project64-rsp-core/cpu/RspSystem.h>
#include <Settings/Settings.h>

#define AddressOf(Addr) CRSPRecompiler::GetAddressOf(5, (Addr))

extern CLog * CPULog;

CRSPRecompiler::CRSPRecompiler(CRSPSystem & System) :
    m_System(System),
    m_BlockID(0),
    m_Assembler(nullptr)
{
    m_Environment = asmjit::Environment::host();
}

CRSPRecompiler::~CRSPRecompiler()
{
    if (m_Assembler != nullptr)
    {
        delete m_Assembler;
        m_Assembler = nullptr;
    }
}

void CRSPRecompiler::Reset()
{
    if (m_Assembler != nullptr)
    {
        m_Assembler->Reset();
    }
}

void * CRSPRecompiler::CompileHLETask(uint32_t Address)
{
    void * funcPtr = RecompPos;
    Log("====== Block %d ======", m_BlockID++);
    Log("asm code at: %016llX", (uint64_t)funcPtr);
    Log("Jump table: %X", Table);
    Log("Start of block: %X", Address);
    Log("====== Recompiled code ======");

    if (m_Assembler != nullptr)
    {
        delete m_Assembler;
        m_Assembler = nullptr;
    }

    m_CodeHolder.reset();
    m_CodeHolder.init(m_Environment);
    m_CodeHolder.setErrorHandler(this);
    m_CodeHolder.setLogger(nullptr);

    m_Assembler = new RspAssembler(&m_CodeHolder, m_CodeLog);
    m_Assembler->push(asmjit::x86::rbp);
    m_Assembler->mov(asmjit::x86::rbp, asmjit::x86::rsp);
    m_Assembler->sub(asmjit::x86::rsp, 0x30);
    m_Assembler->mov(asmjit::x86::rcx, asmjit::imm((uintptr_t)Address));
    m_Assembler->mov(asmjit::x86::rax, asmjit::imm(AddressOf(&StartTimer)));
    m_Assembler->call(asmjit::x86::rax);
    m_Assembler->mov(asmjit::x86::rcx, asmjit::imm((uintptr_t)&m_System));
    m_Assembler->mov(asmjit::x86::edx, asmjit::imm(0x10000));
    m_Assembler->mov(asmjit::x86::r8d, asmjit::imm(0x118));
    m_Assembler->mov(asmjit::x86::rax, asmjit::imm(AddressOf(&CRSPSystem::ExecuteOps)));
    m_Assembler->call(asmjit::x86::rax);
    m_Assembler->mov(asmjit::x86::rax, asmjit::imm(AddressOf(&StopTimer)));
    m_Assembler->call(asmjit::x86::rax);
    m_Assembler->add(asmjit::x86::rsp, 0x30);
    m_Assembler->pop(asmjit::x86::rbp);
    m_Assembler->ret();
    m_Assembler->finalize();

    m_CodeHolder.relocateToBase((uint64_t)funcPtr);
    size_t codeSize = m_CodeHolder.codeSize();
    m_CodeHolder.copyFlattenedData(funcPtr, codeSize);
    RecompPos += codeSize;

    if (LogAsmCode && !m_CodeLog.empty() && CPULog != nullptr)
    {
        CPULog->Log(m_CodeLog.c_str());
        CPULog->Log("\r\n");
        CPULog->Flush();
        m_CodeLog.clear();
    }
    return funcPtr;
}

void CRSPRecompiler::Log(_Printf_format_string_ const char * Text, ...)
{
    if (!LogAsmCode)
    {
        return;
    }

    va_list args;
    va_start(args, Text);
#pragma warning(push)
#pragma warning(disable : 4996)
    size_t nlen = _vscprintf(Text, args) + 1;
    char * buffer = (char *)alloca(nlen * sizeof(char));
    buffer[nlen - 1] = 0;
    if (buffer != nullptr)
    {
        vsprintf(buffer, Text, args);
        m_CodeLog += buffer;
        m_CodeLog += "\n";
    }
#pragma warning(pop)
    va_end(args);
}

void CRSPRecompiler::handleError(asmjit::Error /*err*/, const char * /*message*/, asmjit::BaseEmitter * /*origin*/)
{
    g_Notify->BreakPoint(__FILE__, __LINE__);
}

uintptr_t CRSPRecompiler::GetAddressOf(int value, ...)
{
    void * Address;

    va_list ap;
    va_start(ap, value);
    Address = va_arg(ap, void *);
    va_end(ap);

    return (uintptr_t)Address;
}
#endif

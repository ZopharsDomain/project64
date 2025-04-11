#if defined(__amd64__) || defined(_M_X64)

#include "RspAssembler.h"
#include <Common/StdString.h>
#include <Project64-rsp-core/Settings/RspSettings.h>
#include <Settings/Settings.h>

RspAssembler::RspAssembler(asmjit::CodeHolder * CodeHolder, std::string & CodeLog) :
    asmjit::x86::Assembler(CodeHolder),
    m_CodeLog(CodeLog)
{
    setLogger(nullptr);
    setErrorHandler(this);
    addFlags(asmjit::FormatFlags::kHexOffsets);
    addFlags(asmjit::FormatFlags::kHexImms);
    addFlags(asmjit::FormatFlags::kExplainImms);
}

void RspAssembler::handleError(asmjit::Error /*err*/, const char * /*message*/, asmjit::BaseEmitter * /*origin*/)
{
    g_Notify->BreakPoint(__FILE__, __LINE__);
}

asmjit::Error RspAssembler::_log(const char * data, size_t size) noexcept
{
    stdstr AsmjitLog(std::string(data, size));
    AsmjitLog.Trim("\n");
    m_CodeLog.append(stdstr_f("      %s\n", AsmjitLog.c_str()));
    return asmjit::kErrorOk;
}

void RspAssembler::Reset(void)
{
    setLogger(LogAsmCode ? this : nullptr);
}

#endif
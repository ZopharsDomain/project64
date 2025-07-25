#pragma once
#if defined(__amd64__) || defined(_M_X64)

#include "asmjit.h"
#include <Project64-rsp-core/Recompiler/RspRecompilerOps-x64.h>
#include <Project64-rsp-core/cpu/RSPOpcode.h>
#include <Project64-rsp-core/cpu/RspPipelineStage.h>
#include <unordered_map>

class CRSPSystem;
class RspAssembler;
class RspCodeBlock;

class CRSPRecompiler :
    public asmjit::ErrorHandler
{
    typedef std::unordered_map<uint32_t, asmjit::Label> BranchTargets;

    friend CRSPRecompilerOps;

public:
    CRSPRecompiler(CRSPSystem & System);
    ~CRSPRecompiler();

    void Reset();
    void * CompileHLETask(uint32_t Address);
    void Log(_Printf_format_string_ const char * Text, ...);

    static void * GetAddressOf(int32_t value, ...);

private:
    CRSPRecompiler();
    CRSPRecompiler(const CRSPRecompiler &);
    CRSPRecompiler & operator=(const CRSPRecompiler &);

    void AddBranchJump(uint32_t Target, asmjit::Label Jump);
    bool FindBranchJump(uint32_t Target, asmjit::Label & Jump);
    void BuildRecompilerCPU(void);
    void CompileCodeBlock(RspCodeBlock & block);
    void handleError(asmjit::Error err, const char * message, asmjit::BaseEmitter * origin);
    void SetupRspAssembler();

    CRSPSystem & m_System;
    CRSPRecompilerOps m_RecompilerOps;
    RSPPIPELINE_STAGE m_NextInstruction;
    const RspCodeBlock * m_CurrentBlock;
    std::string m_CodeLog;
    uint32_t m_BlockID;
    uint32_t m_CompilePC;
    RSPOpcode m_OpCode;
    asmjit::Environment m_Environment;
    asmjit::CodeHolder m_CodeHolder;
    RspAssembler * m_Assembler;
    BranchTargets m_BranchTargets;
};

#define AddressOf(Addr) CRSPRecompiler::GetAddressOf(5, (Addr))

#endif
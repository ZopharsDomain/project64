#pragma once
#include <Project64-rsp-core/cpu/RSPInstruction.h>
#include <memory>
#include <set>
#include <stdint.h>
#include <unordered_map>
#include <vector>

class CRSPSystem;

enum RspCodeType
{
    RspCodeType_TASK,
    RspCodeType_SUBROUTINE,
};

class RspCodeBlock;
typedef std::unique_ptr<RspCodeBlock> RspCodeBlockPtr;
typedef std::unordered_map<uint32_t, RspCodeBlockPtr> RspCodeBlocks;
typedef std::vector<RSPInstruction> RSPInstructions;

class RspCodeBlock
{
public:
    typedef std::set<uint32_t> Addresses;

    RspCodeBlock(CRSPSystem & System, uint32_t StartAddress, RspCodeType type, RspCodeBlocks & Functions);

    const Addresses & GetBranchTargets() const;
    void * GetCompiledLocation() const;
    const Addresses & GetFunctionCalls() const;
    const RSPInstructions & GetInstructions() const;
    const RspCodeBlock * GetFunctionBlock(uint32_t Address) const;
    uint32_t GetStartAddress() const;
    void SetCompiledLocation(void * CompiledLoction);
    RspCodeType CodeType() const;
    bool IsEnd(uint32_t Address) const;
    bool IsValid() const;

private:
    RspCodeBlock();
    RspCodeBlock(const RspCodeBlock &);
    RspCodeBlock & operator=(const RspCodeBlock &);

    void Analyze();

    RspCodeBlocks & m_Functions;
    RSPInstructions m_Instructions;
    uint32_t m_StartAddress;
    RspCodeType m_CodeType;
    CRSPSystem & m_System;
    Addresses m_End;
    Addresses m_BranchTargets;
    Addresses m_FunctionCalls;
    void * m_CompiledLoction;
    bool m_Valid;
};
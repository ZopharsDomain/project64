#pragma once
#include <Project64-rsp-core/cpu/RSPInstruction.h>
#include <memory>
#include <set>
#include <stdint.h>
#include <vector>

class CRSPSystem;

enum RspCodeType
{
    RspCodeType_TASK,
    RspCodeType_SUBROUTINE,
};

class RspCodeBlock
{
    typedef std::vector<RSPInstruction> RSPInstructions;
    typedef std::set<uint32_t> Addresses;
    typedef std::unique_ptr<RspCodeBlock> RspCodeBlockPtr;
    typedef std::vector<RspCodeBlockPtr> RspCodeBlocks;

public:
    RspCodeBlock(CRSPSystem & System, uint32_t StartAddress, RspCodeType type);

    bool Valid() const;

private:
    RspCodeBlock();
    RspCodeBlock(const RspCodeBlock &);
    RspCodeBlock & operator=(const RspCodeBlock &);

    void Analyze();

    RSPInstructions m_Instructions;
    uint32_t m_StartAddress;
    RspCodeType m_CodeType;
    CRSPSystem & m_System;
    Addresses m_BranchTargets;
    Addresses m_FunctionCalls;
    RspCodeBlocks m_FunctionBlocks;
    bool m_Valid;
};
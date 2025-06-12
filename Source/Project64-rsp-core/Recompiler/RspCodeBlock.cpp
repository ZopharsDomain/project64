#include "RspCodeBlock.h"
#include <Common/StdString.h>
#include <Project64-rsp-core/cpu/RspSystem.h>

RspCodeBlock::RspCodeBlock(CRSPSystem & System, uint32_t StartAddress, RspCodeType type) :
    m_System(System),
    m_StartAddress(StartAddress),
    m_CodeType(type),
    m_Valid(true)
{
    Analyze();
}

bool RspCodeBlock::Valid() const
{
    return m_Valid;
}

void RspCodeBlock::Analyze(void)
{
    uint32_t Address = m_StartAddress;
    uint8_t * IMEM = m_System.m_IMEM;

    enum EndHleTaskOp
    {
        J_0x1118 = 0x09000446
    };

    bool FoundEnd = false;
    for (;;)
    {
        RSPInstruction Instruction(Address, *(uint32_t *)(IMEM + (Address & 0xFFF)));
        if (Instruction.IsConditionalBranch())
        {
            uint32_t target = Instruction.ConditionalBranchTarget();
            if (std::find(m_BranchTargets.begin(), m_BranchTargets.end(), target) == m_BranchTargets.end())
            {
                m_BranchTargets.insert(target);
            }
        }
        else if (Instruction.IsStaticCall())
        {
            uint32_t target = Instruction.StaticCallTarget();
            if (std::find(m_FunctionCalls.begin(), m_FunctionCalls.end(), target) == m_FunctionCalls.end())
            {
                m_FunctionCalls.insert(target);
            }
        }

        m_Instructions.push_back(Instruction);
        if (FoundEnd)
        {
            break;
        }
        if ((m_CodeType == RspCodeType_SUBROUTINE && Instruction.IsJumpReturn()) ||
           (m_CodeType == RspCodeType_TASK && Instruction.Value() == J_0x1118))
        {
            bool JumpBeyond = false;
            for (Addresses::iterator itr = m_BranchTargets.begin(); itr != m_BranchTargets.end(); itr++)
            {
                if (*itr > Address)
                { 
                    JumpBeyond = true;
                    break;
                }
            }
            FoundEnd = !JumpBeyond;
        }
        Address += 4;
        if (Address == 0x2000)
        {
            m_Valid = false;
            return;
        }
    }
    for (Addresses::iterator itr = m_FunctionCalls.begin(); itr != m_FunctionCalls.end(); itr++)
    {
        RspCodeBlockPtr FunctionCall = std::make_unique<RspCodeBlock>(m_System, *itr, RspCodeType_SUBROUTINE);
        if (!FunctionCall->Valid())
        {
            m_Valid = false;
            return;
        }
        m_FunctionBlocks.push_back(std::move(FunctionCall));
    }
}

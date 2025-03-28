#pragma once
#include "RSPOpcode.h"
#include <stdint.h>
#include <string>

class RSPInstruction
{
public:
    RSPInstruction(uint32_t Address, uint32_t Instruction);
    RSPInstruction & operator=(const RSPInstruction &);

    uint32_t Address() const;
    bool IsBranch() const;
    bool IsNop() const;
    const char * Name() const;
    const char * Param() const;
    std::string NameAndParam() const;
    uint32_t Value() const;

private:
    RSPInstruction(void);
    RSPInstruction(const RSPInstruction &);

    void DecodeName(void) const;
    void DecodeSpecialName(void) const;
    void DecodeRegImmName(void) const;
    void DecodeCop0Name(void) const;
    void DecodeCop2Name(void) const;
    void DecodeLSC2Name(const char LoadStoreIdent) const;

    static const char * ElementSpecifier(uint32_t Element);

    uint32_t m_Address;
    RSPOpcode m_Instruction;
    mutable char m_Name[40];
    mutable char m_Param[200];
};
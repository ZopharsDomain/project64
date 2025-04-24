#pragma once

ASMJIT_BEGIN_NAMESPACE

//! \cond INTERNAL
//! \addtogroup asmjit_core
//! \{

//! Utilities used by various emitters, mostly Assembler implementations.
namespace EmitterUtils {

#ifndef ASMJIT_NO_LOGGING
Error finishFormattedLine(String& sb, const FormatOptions& formatOptions, const uint8_t* binData, size_t binSize, size_t offsetSize, size_t immSize, const char* comment) noexcept;

void logLabelBound(BaseAssembler* self, const Label& label) noexcept;
#endif

}

ASMJIT_END_NAMESPACE

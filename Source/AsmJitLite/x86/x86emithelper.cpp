// This file is part of AsmJit project <https://asmjit.com>
//
// See asmjit.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include "../core/api-build_p.h"
#if !defined(ASMJIT_NO_X86)

#include "../core/formatter.h"
#include "../core/string.h"
#include "../core/support.h"
#include "../core/type.h"
#include "../x86/x86emithelper_p.h"
#include "../x86/x86emitter.h"

ASMJIT_BEGIN_SUB_NAMESPACE(x86)

void assignEmitterFuncs(BaseEmitter * /*emitter*/) {
}

ASMJIT_END_SUB_NAMESPACE

#endif // !ASMJIT_NO_X86

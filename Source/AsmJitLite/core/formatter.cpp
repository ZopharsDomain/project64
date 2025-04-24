#include "../core/api-build_p.h"
#ifndef ASMJIT_NO_LOGGING

#include "../core/archtraits.h"
#include "../core/codeholder.h"
#include "../core/emitter.h"
#include "../core/string.h"
#include "../core/support.h"
#include "../core/type.h"

ASMJIT_BEGIN_NAMESPACE
namespace Formatter {


Error formatLabel(
  String& sb,
  FormatFlags formatFlags,
  const BaseEmitter* emitter,
  uint32_t labelId) noexcept {

  DebugUtils::unused(formatFlags);

  const LabelEntry* le = emitter->code()->labelEntry(labelId);
  if (ASMJIT_UNLIKELY(!le))
    return sb.appendFormat("<InvalidLabel:%u>", labelId);

  if (le->hasName()) {
    if (le->hasParent()) {
      uint32_t parentId = le->parentId();
      const LabelEntry* pe = emitter->code()->labelEntry(parentId);

      if (ASMJIT_UNLIKELY(!pe))
        ASMJIT_PROPAGATE(sb.appendFormat("<InvalidLabel:%u>", labelId));
      else if (ASMJIT_UNLIKELY(!pe->hasName()))
        ASMJIT_PROPAGATE(sb.appendFormat("L%u", parentId));
      else
        ASMJIT_PROPAGATE(sb.append(pe->name()));

      ASMJIT_PROPAGATE(sb.append('.'));
    }

    if (le->type() == LabelType::kAnonymous)
      ASMJIT_PROPAGATE(sb.appendFormat("L%u@", labelId));
    return sb.append(le->name());
  }
  else {
    return sb.appendFormat("L%u", labelId);
  }
}

} // {Formatter}

ASMJIT_END_NAMESPACE

#endif

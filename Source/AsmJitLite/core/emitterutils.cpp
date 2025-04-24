#include "../core/api-build_p.h"
#include "../core/assembler.h"
#include "../core/emitterutils_p.h"
#include "../core/formatter_p.h"
#include "../core/logger.h"
#include "../core/support.h"

ASMJIT_BEGIN_NAMESPACE

namespace EmitterUtils {

#ifndef ASMJIT_NO_LOGGING

Error finishFormattedLine(String& sb, const FormatOptions& formatOptions, const uint8_t* binData, size_t binSize, size_t offsetSize, size_t immSize, const char* comment) noexcept {
  ASMJIT_ASSERT(binSize >= offsetSize);
  const size_t kNoBinSize = SIZE_MAX;

  size_t commentSize = comment ? Support::strLen(comment, Globals::kMaxCommentSize) : 0;

  if ((binSize != 0 && binSize != kNoBinSize) || commentSize) {
    char sep = ';';
    size_t padding = Formatter::paddingFromOptions(formatOptions, FormatPaddingGroup::kRegularLine);

    for (size_t i = (binSize == kNoBinSize); i < 2; i++) {
      ASMJIT_PROPAGATE(sb.padEnd(padding));

      if (sep) {
        ASMJIT_PROPAGATE(sb.append(sep));
        ASMJIT_PROPAGATE(sb.append(' '));
      }

      // Append binary data or comment.
      if (i == 0) {
        ASMJIT_PROPAGATE(sb.appendHex(binData, binSize - offsetSize - immSize));
        ASMJIT_PROPAGATE(sb.appendChars('.', offsetSize * 2));
        ASMJIT_PROPAGATE(sb.appendHex(binData + binSize - immSize, immSize));
        if (commentSize == 0) break;
      }
      else {
        ASMJIT_PROPAGATE(sb.append(comment, commentSize));
      }

      sep = '|';
      padding += Formatter::paddingFromOptions(formatOptions, FormatPaddingGroup::kMachineCode);
    }
  }

  return sb.append('\n');
}

void logLabelBound(BaseAssembler* self, const Label& label) noexcept {
  Logger* logger = self->logger();

  StringTmp<512> sb;
  size_t binSize = logger->hasFlag(FormatFlags::kMachineCode) ? size_t(0) : SIZE_MAX;

  sb.appendChars(' ', logger->indentation(FormatIndentationGroup::kLabel));
  Formatter::formatLabel(sb, logger->flags(), self, label.id());
  sb.append(':');
  finishFormattedLine(sb, logger->options(), nullptr, binSize, 0, 0, self->_inlineComment);
  logger->log(sb.data(), sb.size());
}

#endif

} // {EmitterUtils}

ASMJIT_END_NAMESPACE

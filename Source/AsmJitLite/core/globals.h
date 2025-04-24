#pragma once
#include <stdint.h>
#include "../core/api-config.h"

ASMJIT_BEGIN_NAMESPACE

#if defined(ASMJIT_NO_STDCXX)
namespace Support {
  ASMJIT_FORCE_INLINE void* operatorNew(size_t n) noexcept { return malloc(n); }
  ASMJIT_FORCE_INLINE void operatorDelete(void* p) noexcept { if (p) free(p); }
} // {Support}

#define ASMJIT_BASE_CLASS(TYPE)                                                  \
  ASMJIT_FORCE_INLINE void* operator new(size_t n) noexcept {                    \
    return Support::operatorNew(n);                                              \
  }                                                                              \
                                                                                 \
  ASMJIT_FORCE_INLINE void  operator delete(void* p) noexcept {                  \
    Support::operatorDelete(p);                                                  \
  }                                                                              \
                                                                                 \
  ASMJIT_FORCE_INLINE void* operator new(size_t, void* p) noexcept { return p; } \
  ASMJIT_FORCE_INLINE void  operator delete(void*, void*) noexcept {}
#else
#define ASMJIT_BASE_CLASS(TYPE)
#endif

//! \}
//! \endcond

//! \addtogroup asmjit_core
//! \{

//! Byte order.
enum class ByteOrder {
  //! Little endian.
  kLE = 0,
  //! Big endian.
  kBE = 1,
  //! Native byte order of the target architecture.
  kNative = ASMJIT_ARCH_LE ? kLE : kBE,
  //! Swapped byte order of the target architecture.
  kSwapped = ASMJIT_ARCH_LE ? kBE : kLE
};

//! A policy that can be used with some `reset()` member functions.
enum class ResetPolicy : uint32_t {
  //! Soft reset, doesn't deallocate memory (default).
  kSoft = 0,
  //! Hard reset, releases all memory used, if any.
  kHard = 1
};

//! Contains typedefs, constants, and variables used globally by AsmJit.
namespace Globals {

//! Host memory allocator overhead.
static constexpr uint32_t kAllocOverhead = uint32_t(sizeof(intptr_t) * 4);

//! Host memory allocator alignment.
static constexpr uint32_t kAllocAlignment = 8;

//! Aggressive growing strategy threshold.
static constexpr uint32_t kGrowThreshold = 1024 * 1024 * 16;
//! Maximum section name size.
static constexpr uint32_t kMaxSectionNameSize = 35;

//! Maximum size of comment.
static constexpr uint32_t kMaxCommentSize = 1024;

//! Invalid identifier.
static constexpr uint32_t kInvalidId = 0xFFFFFFFFu;

//! Returned by `indexOf()` and similar when working with containers that use 32-bit index/size.
static constexpr uint32_t kNotFound = 0xFFFFFFFFu;

//! Invalid base address.
static constexpr uint64_t kNoBaseAddress = ~uint64_t(0);

//! Number of virtual register groups.
static constexpr uint32_t kNumVirtGroups = 4;

struct Init_ {};
struct NoInit_ {};

static const constexpr Init_ Init {};
static const constexpr NoInit_ NoInit {};

} // {Globals}

//! \}

//! \addtogroup asmjit_error_handling
//! \{

//! AsmJit error type (uint32_t).
typedef uint32_t Error;

//! AsmJit error codes.
enum ErrorCode : uint32_t {
  // @EnumValuesBegin{"enum": "ErrorCode"}@

  //! No error (success).
  kErrorOk = 0,

  //! Out of memory.
  kErrorOutOfMemory,

  //! Invalid argument.
  kErrorInvalidArgument,

  //! Invalid state.
  //!
  //! If this error is returned it means that either you are doing something wrong or AsmJit caught itself by
  //! doing something wrong. This error should never be ignored.
  kErrorInvalidState,

  //! Invalid or incompatible architecture.
  kErrorInvalidArch,

  //! The object is not initialized.
  kErrorNotInitialized,
  //! The object is already initialized.
  kErrorAlreadyInitialized,
  //! Code generated is larger than allowed.
  kErrorTooLarge,
  //! Attempt to use uninitialized label.
  kErrorInvalidLabel,
  //! Label index overflow - a single \ref BaseAssembler instance can hold almost 2^32 (4 billion) labels. If
  //! there is an attempt to create more labels then this error is returned.
  kErrorTooManyLabels,
  //! Label is already bound.
  kErrorLabelAlreadyBound,

  //! Invalid section.
  kErrorInvalidSection,

  //! Relocation index overflow (too many relocations).
  kErrorTooManyRelocations,
  //! Invalid relocation entry.
  kErrorInvalidRelocEntry,
  //! Reloc entry contains address that is out of range (unencodable).
  kErrorRelocOffsetOutOfRange,

  //! Invalid instruction.
  kErrorInvalidInstruction,
  //! Invalid register type.
  kErrorInvalidRegType,
  //! Invalid register group.
  kErrorInvalidRegGroup,
  //! Invalid displacement (not encodable).
  kErrorInvalidDisplacement,

  //! Unbound label cannot be evaluated by expression.
  kErrorExpressionLabelNotBound,
};

//! Debugging utilities.
namespace DebugUtils {

//! \cond INTERNAL
//! Used to silence warnings about unused arguments or variables.
template<typename... Args>
static inline void unused(Args&&...) noexcept {}
//! \endcond

//! Returns the error `err` passed.
//!
//! Provided for debugging purposes. Putting a breakpoint inside `errored` can help with tracing the origin of any
//! error reported / returned by AsmJit.
static constexpr Error errored(Error err) noexcept { return err; }

//! Returns a printable version of `asmjit::Error` code.
ASMJIT_API const char* errorAsString(Error err) noexcept;

//! Called to output debugging message(s).
ASMJIT_API void debugOutput(const char* str) noexcept;

//! Called on assertion failure.
//!
//! \param file Source file name where it happened.
//! \param line Line in the source file.
//! \param msg Message to display.
//!
//! If you have problems with assertion failures a breakpoint can be put at \ref assertionFailed() function
//! (asmjit/core/globals.cpp). A call stack will be available when such assertion failure is triggered. AsmJit
//! always returns errors on failures, assertions are a last resort and usually mean unrecoverable state due to out
//! of range array access or totally invalid arguments like nullptr where a valid pointer should be provided, etc...
ASMJIT_API void ASMJIT_NORETURN assertionFailed(const char* file, int line, const char* msg) noexcept;

} // {DebugUtils}

//! \def ASMJIT_ASSERT(...)
//!
//! AsmJit's own assert macro used in AsmJit code-base.
#if defined(ASMJIT_BUILD_DEBUG)
#define ASMJIT_ASSERT(...)                                                     \
  do {                                                                         \
    if (ASMJIT_LIKELY(__VA_ARGS__))                                            \
      break;                                                                   \
    ::asmjit::DebugUtils::assertionFailed(__FILE__, __LINE__, #__VA_ARGS__);   \
  } while (0)
#else
#define ASMJIT_ASSERT(...) ((void)0)
#endif

//! \def ASMJIT_PROPAGATE(...)
//!
//! Propagates a possible `Error` produced by `...` to the caller by returning the error immediately. Used by AsmJit
//! internally, but kept public for users that want to use the same technique to propagate errors to the caller.
#define ASMJIT_PROPAGATE(...)               \
  do {                                      \
    ::asmjit::Error _err = __VA_ARGS__;     \
    if (ASMJIT_UNLIKELY(_err))              \
      return _err;                          \
  } while (0)

//! \}

ASMJIT_END_NAMESPACE
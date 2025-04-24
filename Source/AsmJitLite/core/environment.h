#pragma once
#include "../core/archtraits.h"
#include <stdio.h>
#include <string.h>

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_core
//! \{

//! Vendor.
//!
//! \note AsmJit doesn't use vendor information at the moment. It's provided for future use, if required.
enum class Vendor : uint8_t {
  //! Unknown or uninitialized platform vendor.
  kUnknown = 0,

  //! Maximum value of `PlatformVendor`.
  kMaxValue = kUnknown,

  //! Platform vendor detected at compile-time.
  kHost =
#if defined(_DOXYGEN)
    DETECTED_AT_COMPILE_TIME
#else
    kUnknown
#endif
};

//! Platform - runtime environment or operating system.
enum class Platform : uint8_t {
  //! Unknown or uninitialized platform.
  kUnknown = 0,

  //! Windows OS.
  kWindows,

  //! Other platform that is not Windows, most likely POSIX based.
  kOther,

  //! Linux OS.
  kLinux,
  //! GNU/Hurd OS.
  kHurd,

  //! FreeBSD OS.
  kFreeBSD,
  //! OpenBSD OS.
  kOpenBSD,
  //! NetBSD OS.
  kNetBSD,
  //! DragonFly BSD OS.
  kDragonFlyBSD,

  //! Haiku OS.
  kHaiku,

  //! Apple OSX.
  kOSX,
  //! Apple iOS.
  kIOS,
  //! Apple TVOS.
  kTVOS,
  //! Apple WatchOS.
  kWatchOS,

  //! Emscripten platform.
  kEmscripten,

  //! Maximum value of `Platform`.
  kMaxValue = kEmscripten,

  //! Platform detected at compile-time (platform of the host).
  kHost =
#if defined(_DOXYGEN)
    DETECTED_AT_COMPILE_TIME
#elif defined(__EMSCRIPTEN__)
    kEmscripten
#elif defined(_WIN32)
    kWindows
#elif defined(__linux__)
    kLinux
#elif defined(__gnu_hurd__)
    kHurd
#elif defined(__FreeBSD__)
    kFreeBSD
#elif defined(__OpenBSD__)
    kOpenBSD
#elif defined(__NetBSD__)
    kNetBSD
#elif defined(__DragonFly__)
    kDragonFlyBSD
#elif defined(__HAIKU__)
    kHaiku
#elif defined(__APPLE__) && TARGET_OS_OSX
    kOSX
#elif defined(__APPLE__) && TARGET_OS_TV
    kTVOS
#elif defined(__APPLE__) && TARGET_OS_WATCH
    kWatchOS
#elif defined(__APPLE__) && TARGET_OS_IPHONE
    kIOS
#else
    kOther
#endif
};

//! Platform ABI (application binary interface).
enum class PlatformABI : uint8_t {
  //! Unknown or uninitialied environment.
  kUnknown = 0,
  //! Microsoft ABI.
  kMSVC,
  //! GNU ABI.
  kGNU,
  //! Android Environment / ABI.
  kAndroid,
  //! Cygwin ABI.
  kCygwin,

  //! Maximum value of `PlatformABI`.
  kMaxValue,

  //! Host ABI detected at compile-time.
  kHost =
#if defined(_DOXYGEN)
    DETECTED_AT_COMPILE_TIME
#elif defined(_MSC_VER)
    kMSVC
#elif defined(__CYGWIN__)
    kCygwin
#elif defined(__MINGW32__) || defined(__GLIBC__)
    kGNU
#elif defined(__ANDROID__)
    kAndroid
#else
    kUnknown
#endif
};

//! Object format.
//!
//! \note AsmJit doesn't really use anything except \ref ObjectFormat::kUnknown and \ref ObjectFormat::kJIT at
//! the moment. Object file formats are provided for future extensibility and a possibility to generate object
//! files at some point.
enum class ObjectFormat : uint8_t {
  //! Unknown or uninitialized object format.
  kUnknown = 0,

  //! JIT code generation object, most likely \ref JitRuntime or a custom
  //! \ref Target implementation.
  kJIT,

  //! Executable and linkable format (ELF).
  kELF,
  //! Common object file format.
  kCOFF,
  //! Extended COFF object format.
  kXCOFF,
  //! Mach object file format.
  kMachO,

  //! Maximum value of `ObjectFormat`.
  kMaxValue
};

//! Represents an environment, which is usually related to a \ref Target.
//!
//! Environment has usually an 'arch-subarch-vendor-os-abi' format, which is sometimes called "Triple" (historically
//! it used to be 3 only parts) or "Tuple", which is a convention used by Debian Linux.
//!
//! AsmJit doesn't support all possible combinations or architectures and ABIs, however, it models the environment
//! similarly to other compilers for future extensibility.
class Environment {
public:
  //! \name Members
  //! \{

  //! Architecture.
  Arch _arch;
  //! Sub-architecture type.
  SubArch _subArch;
  //! Vendor type.
  Vendor _vendor;
  //! Platform.
  Platform _platform;
  //! Platform ABI.
  PlatformABI _platformABI;
  //! Object format.
  ObjectFormat _objectFormat;
  //! Reserved for future use, must be zero.
  uint8_t _reserved[2];

  //! \}

  //! \name Construction & Destruction
  //! \{

  inline Environment() noexcept :
    _arch(Arch::kUnknown),
    _subArch(SubArch::kUnknown),
    _vendor(Vendor::kUnknown),
    _platform(Platform::kUnknown),
    _platformABI(PlatformABI::kUnknown),
    _objectFormat(ObjectFormat::kUnknown),
    _reserved { 0, 0 } {}

  inline explicit Environment(
    Arch arch,
    SubArch subArch = SubArch::kUnknown,
    Vendor vendor = Vendor::kUnknown,
    Platform platform = Platform::kUnknown,
    PlatformABI abi = PlatformABI::kUnknown,
    ObjectFormat objectFormat = ObjectFormat::kUnknown) noexcept {

    init(arch, subArch, vendor, platform, abi, objectFormat);
  }

  inline Environment(const Environment& other) noexcept = default;

  //! Returns the host environment constructed from preprocessor macros defined by the compiler.
  //!
  //! The returned environment should precisely match the target host architecture, sub-architecture, platform,
  //! and ABI.
  static inline Environment host() noexcept {
    return Environment(Arch::kHost, SubArch::kHost, Vendor::kHost, Platform::kHost, PlatformABI::kHost, ObjectFormat::kUnknown);
  }

  //! \}

  //! \name Overloaded Operators
  //! \{

  inline Environment& operator=(const Environment& other) noexcept = default;

  inline bool operator==(const Environment& other) const noexcept { return  equals(other); }
  inline bool operator!=(const Environment& other) const noexcept { return !equals(other); }

  //! \}

  //! \name Accessors
  //! \{

  //! Tests whether the environment is not set up.
  //!
  //! Returns true if all members are zero, and thus unknown.
  inline bool empty() const noexcept {
    // Unfortunately compilers won't optimize fields are checked one by one...
    return _packed() == 0;
  }

  //! Tests whether the environment is initialized, which means it must have
  //! a valid architecture.
  inline bool isInitialized() const noexcept {
    return _arch != Arch::kUnknown;
  }

  inline uint64_t _packed() const noexcept {
    uint64_t x;
    memcpy(&x, this, 8);
    return x;
  }

  //! Resets all members of the environment to zero / unknown.
  inline void reset() noexcept {
    _arch = Arch::kUnknown;
    _subArch = SubArch::kUnknown;
    _vendor = Vendor::kUnknown;
    _platform = Platform::kUnknown;
    _platformABI = PlatformABI::kUnknown;
    _objectFormat = ObjectFormat::kUnknown;
    _reserved[0] = 0;
    _reserved[1] = 0;
  }

  inline bool equals(const Environment& other) const noexcept {
    return _packed() == other._packed();
  }

  //! Returns the architecture.
  inline Arch arch() const noexcept { return _arch; }
  //! Returns the sub-architecture.
  inline SubArch subArch() const noexcept { return _subArch; }
  //! Returns vendor.
  inline Vendor vendor() const noexcept { return _vendor; }
  //! Returns target's platform or operating system.
  inline Platform platform() const noexcept { return _platform; }
  //! Returns target's ABI.
  inline PlatformABI platformABI() const noexcept { return _platformABI; }
  //! Returns target's object format.
  inline ObjectFormat objectFormat() const noexcept { return _objectFormat; }

  inline void init(
    Arch arch,
    SubArch subArch = SubArch::kUnknown,
    Vendor vendor = Vendor::kUnknown,
    Platform platform = Platform::kUnknown,
    PlatformABI platformABI = PlatformABI::kUnknown,
    ObjectFormat objectFormat = ObjectFormat::kUnknown) noexcept {

    _arch = arch;
    _subArch = subArch;
    _vendor = vendor;
    _platform = platform;
    _platformABI = platformABI;
    _objectFormat = objectFormat;
    _reserved[0] = 0;
    _reserved[1] = 0;
  }

  //! Tests whether the architecture is 32-bit.
  inline bool is32Bit() const noexcept { return is32Bit(_arch); }
  //! Tests whether the architecture is 64-bit.
  inline bool is64Bit() const noexcept { return is64Bit(_arch); }

  //! Returns a native register size of this architecture.
  uint32_t registerSize() const noexcept { return registerSizeFromArch(_arch); }
  //! Tests whether the given architecture `arch` is 32-bit.
  static inline bool is32Bit(Arch arch) noexcept {
    return (uint32_t(arch) & uint32_t(Arch::k32BitMask)) == uint32_t(Arch::k32BitMask);
  }

  //! Tests whether the given architecture `arch` is 64-bit.
  static inline bool is64Bit(Arch arch) noexcept {
    return (uint32_t(arch) & uint32_t(Arch::k32BitMask)) == 0;
  }
  //! Returns a native general purpose register size from the given architecture.
  static inline uint32_t registerSizeFromArch(Arch arch) noexcept {
    return is32Bit(arch) ? 4u : 8u;
  }

  //! \}
};

static_assert(sizeof(Environment) == 8,
              "Environment must occupy exactly 8 bytes.");

//! \}

ASMJIT_END_NAMESPACE

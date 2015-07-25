#ifndef __MONSTER_PLATFORM_H__
#define __MONSTER_PLATFORM_H__

// Adapted from Branimir Karadžić's platform.h (https://github.com/bkaradzic/bx)

#define MONSTER_COMPILER_CLANG 0
#define MONSTER_COMPILER_GCC 0
#define MONSTER_COMPILER_MSVC 0

#define MONSTER_PLATFORM_ANDROID 0
#define MONSTER_PLATFORM_IOS 0
#define MONSTER_PLATFORM_LINUX 0
#define MONSTER_PLATFORM_OSX 0
#define MONSTER_PLATFORM_WINDOWS 0

#define MONSTER_CPU_ARM  0
#define MONSTER_CPU_JIT  0
#define MONSTER_CPU_MIPS 0
#define MONSTER_CPU_PPC  0
#define MONSTER_CPU_X86  0

#define MONSTER_ARCH_32BIT 0
#define MONSTER_ARCH_64BIT 0

#define MONSTER_CPU_ENDIAN_BIG 0
#define MONSTER_CPU_ENDIAN_LITTLE 0

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers
#if defined(_MSC_VER)
#undef MONSTER_COMPILER_MSVC
#define MONSTER_COMPILER_MSVC 1
#elif defined(__clang__)
// clang defines __GNUC__
#undef MONSTER_COMPILER_CLANG
#define MONSTER_COMPILER_CLANG 1
#elif defined(__GNUC__)
#undef MONSTER_COMPILER_GCC
#define MONSTER_COMPILER_GCC 1
#else
#error "MONSTER_COMPILER_* is not defined!"
#endif

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Operating_Systems
#if defined(_WIN32) || defined(_WIN64)
#undef MONSTER_PLATFORM_WINDOWS
// http://msdn.microsoft.com/en-us/library/6sehtctf.aspx
#if !defined(WINVER) && !defined(_WIN32_WINNT)
// Windows Server 2003 with SP1, Windows XP with SP2 and above
#define WINVER 0x0502
#define _WIN32_WINNT 0x0502
#endif // !defined(WINVER) && !defined(_WIN32_WINNT)
#define MONSTER_PLATFORM_WINDOWS 1
#elif defined(__ANDROID__)
// Android compiler defines __linux__
#undef MONSTER_PLATFORM_ANDROID
#define MONSTER_PLATFORM_ANDROID 1
#elif defined(__linux__)
#undef MONSTER_PLATFORM_LINUX
#define MONSTER_PLATFORM_LINUX 1
#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
#undef MONSTER_PLATFORM_IOS
#define MONSTER_PLATFORM_IOS 1
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#undef MONSTER_PLATFORM_OSX
#define MONSTER_PLATFORM_OSX 1
#else
#	error "MONSTER_PLATFORM_* is not defined!"
#endif

#define MONSTER_PLATFORM_POSIX (MONSTER_PLATFORM_ANDROID \
	|| MONSTER_PLATFORM_IOS \
	|| MONSTER_PLATFORM_LINUX \
	|| MONSTER_PLATFORM_OSX)

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Architectures
#if defined(__arm__)
#undef MONSTER_CPU_ARM
#define MONSTER_CPU_ARM 1
#define MONSTER_CACHE_LINE_SIZE 64
#elif defined(__MIPSEL__) || defined(__mips_isa_rev) // defined(mips)
#undef MONSTER_CPU_MIPS
#define MONSTER_CPU_MIPS 1
#define MONSTER_CACHE_LINE_SIZE 64
#elif defined(_M_PPC) || defined(__powerpc__) || defined(__powerpc64__)
#undef MONSTER_CPU_PPC
#define MONSTER_CPU_PPC 1
#define MONSTER_CACHE_LINE_SIZE 128
#elif defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
#undef MONSTER_CPU_X86
#define MONSTER_CPU_X86 1
#define MONSTER_CACHE_LINE_SIZE 64
#else // PNaCl doesn't have CPU defined.
#undef MONSTER_CPU_JIT
#define MONSTER_CPU_JIT 1
#define MONSTER_CACHE_LINE_SIZE 64
#endif //

#if defined(__x86_64__) || defined(_M_X64) || defined(__64BIT__) || defined(__powerpc64__) || defined(__ppc64__)
#undef MONSTER_ARCH_64BIT
#define MONSTER_ARCH_64BIT 64
#else
#undef MONSTER_ARCH_32BIT
#define MONSTER_ARCH_32BIT 32
#endif //

#if MONSTER_CPU_PPC
#undef MONSTER_CPU_ENDIAN_BIG
#define MONSTER_CPU_ENDIAN_BIG 1
#else
#undef MONSTER_CPU_ENDIAN_LITTLE
#define MONSTER_CPU_ENDIAN_LITTLE 1
#endif

#if MONSTER_COMPILER_GCC
#define MONSTER_COMPILER_NAME "GCC"
#elif MONSTER_COMPILER_MSVC
#define MONSTER_COMPILER_NAME "MSVC"
#endif

#if MONSTER_PLATFORM_ANDROID
#define MONSTER_PLATFORM_NAME "Android"
#elif MONSTER_PLATFORM_IOS
#define MONSTER_PLATFORM_NAME "iOS"
#elif MONSTER_PLATFORM_LINUX
#define MONSTER_PLATFORM_NAME "Linux"
#elif MONSTER_PLATFORM_OSX
#define MONSTER_PLATFORM_NAME "OSX"
#elif MONSTER_PLATFORM_WINDOWS
#define MONSTER_PLATFORM_NAME "Windows"
#endif // MONSTER_PLATFORM_

#if MONSTER_CPU_ARM
#define MONSTER_CPU_NAME "ARM"
#elif MONSTER_CPU_MIPS
#define MONSTER_CPU_NAME "MIPS"
#elif MONSTER_CPU_PPC
#define MONSTER_CPU_NAME "PowerPC"
#elif MONSTER_CPU_JIT
#define MONSTER_CPU_NAME "JIT-VM"
#elif MONSTER_CPU_X86
#define MONSTER_CPU_NAME "x86"
#endif // MONSTER_CPU_

#if MONSTER_ARCH_32BIT
#define MONSTER_ARCH_NAME "32-bit"
#elif MONSTER_ARCH_64BIT
#define MONSTER_ARCH_NAME "64-bit"
#endif // MONSTER_ARCH_

#endif
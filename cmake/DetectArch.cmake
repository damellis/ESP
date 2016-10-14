## Adapted from https://github.com/credentials/elvira

set(arch_detection_code "
#if defined(__arm__) || defined(__TARGET_ARCH_ARM) || defined(_M_ARM)
  #if defined(__ARM_ARCH_7__) \\
      || defined(__ARM_ARCH_7A__) \\
      || defined(__ARM_ARCH_7R__) \\
      || defined(__ARM_ARCH_7M__) \\
      || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 7) \\
      || (defined(_M_ARM) && _M_ARM-0 >= 7)
    #error cmake_ARCH armv7
  #elif defined(__ARM_ARCH_6__) \\
      || defined(__ARM_ARCH_6J__) \\
      || defined(__ARM_ARCH_6T2__) \\
      || defined(__ARM_ARCH_6Z__) \\
      || defined(__ARM_ARCH_6K__) \\
      || defined(__ARM_ARCH_6ZK__) \\
      || defined(__ARM_ARCH_6M__) \\
      || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 6) \\
      || (defined(_M_ARM) && _M_ARM-0 >= 6)
    #error cmake_ARCH armv6
  #elif defined(__ARM_ARCH_5TEJ__) \\
      || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 5) \\
      || (defined(_M_ARM) && _M_ARM-0 >= 5)
    #error cmake_ARCH armv5
  #else
    #error cmake_ARCH arm
  #endif
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
  #error cmake_ARCH x86_64
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
  #error cmake_ARCH x86_32
#elif defined(__mips) || defined(__mips__) || defined(_M_MRX000)
  #if defined(_MIPS_ARCH_MIPS64) || defined(__mips64) || (defined(__mips) && __mips >= 64)
    #error cmake_ARCH mips64
  #elif defined(_MIPS_ARCH_MIPS32) || defined(__mips32) || (defined(__mips) && __mips >= 32)
    #error cmake_ARCH mips32
  #elif defined(_MIPS_ARCH_MIPS5) || (defined(__mips) && __mips >= 5)
    #error cmake_ARCH mips5
  #elif defined(_MIPS_ARCH_MIPS4) || (defined(__mips) && __mips >= 4)
    #error cmake_ARCH mips4
  #elif defined(_MIPS_ARCH_MIPS3) || (defined(__mips) && __mips >= 3)
    #error cmake_ARCH mips3
  #elif defined(_MIPS_ARCH_MIPS2) || (defined(__mips) && __mips >= 2)
    #error cmake_ARCH mips2
  #elif defined(_MIPS_ARCH_MIPS1) || (defined(__mips) && __mips >= 1)
    #error cmake_ARCH mips1
  #else
    #error cmake_ARCH mips
  #endif
#endif

#error cmake_ARCH unknown
")

macro(set_arch_flags arch)
  if (${arch} STREQUAL "armv7")
    set(ARMV7 ON)
    set(ARMV6 ON)
    set(ARMV5 ON)
    set(ARM ON)
  endif()
  if (${arch} STREQUAL "armv6")
    set(ARMV6 ON)
    set(ARMV5 ON)
    set(ARM ON)
  endif()
  if (${arch} STREQUAL "armv5")
    set(ARMV5 ON)
    set(ARM ON)
  endif()
  if (${arch} STREQUAL "arm")
    set(ARM ON)
  endif()
  if (${arch} STREQUAL "x86_64")
    set(AMD64 ON)
    set(X64 ON)
    set(X86_64 ON)
  endif()
  if (${arch} STREQUAL "x86_32")
    set(I386 ON)
    set(X86 ON)
    set(X86_32 ON)
  endif()
  if (${arch} STREQUAL "mips64")
    set(MIPS64 ON)
    set(MIPS ON)
  endif()
  if (${arch} STREQUAL "mips32")
    set(MIPS32 ON)
    set(MIPS ON)
  endif()
  if (${arch} STREQUAL "mips5")
    set(MIPS5 ON)
    set(MIPS ON)
  endif()
  if (${arch} STREQUAL "mips4")
    set(MIPS4 ON)
    set(MIPS ON)
  endif()
  if (${arch} STREQUAL "mips3")
    set(MIPS3 ON)
    set(MIPS ON)
  endif()
  if (${arch} STREQUAL "mips2")
    set(MIPS2 ON)
    set(MIPS ON)
  endif()
  if (${arch} STREQUAL "mips1")
    set(MIPS1 ON)
    set(MIPS ON)
  endif()
  if (${arch} STREQUAL "mips")
    set(MIPS ON)
  endif()
endmacro()

macro(detect_arch)
  file(WRITE "${CMAKE_BINARY_DIR}/arch.c" "${arch_detection_code}")

  enable_language(C)

# Detect the architecture in a rather creative way...
# This compiles a small C program which is a series of ifdefs that selects a
# particular #error preprocessor directive whose message string contains the
# target architecture. The program will always fail to compile (both because
# file is not a valid C program, and obviously because of the presence of the
# #error preprocessor directives... but by exploiting the preprocessor in this
# way, we can detect the correct target architecture even when cross-compiling,
# since the program itself never needs to be run (only the compiler/preprocessor)
  try_run(
    run_result_unused
    compile_result_unused
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_BINARY_DIR}/arch.c"
    COMPILE_OUTPUT_VARIABLE ARCH
    CMAKE_FLAGS
  )

# Parse the architecture name from the compiler output
  string(REGEX MATCH "cmake_ARCH ([a-zA-Z0-9_]+)" ARCH "${ARCH}")

# Get rid of the value marker leaving just the architecture name
  string(REPLACE "cmake_ARCH " "" ARCH "${ARCH}")

# If we are compiling with an unknown architecture this variable should
# already be set to "unknown" but in the case that it's empty (i.e. due
# to a typo in the code), then set it to unknown
  if (NOT ARCH)
    set(ARCH unknown)
  endif()
  message(STATUS "Detected architecture: ${ARCH}")
  set_arch_flags(ARCH)
endmacro()

// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#pragma once
#ifndef DEBUG_MACROS_FOR_ARMADA_WRAPPER
#define DEBUG_MACROS_FOR_ARMADA_WRAPPER

#include <stdio.h>

#define ARMADA_PREFIX_A     "[ArmadaWrapper] "

#define MY_WIDEN2(x)        L##x
#define MY_WIDEN(x)         MY_WIDEN2(x)
#define ARMADA_PREFIX_W     MY_WIDEN(ARMADA_PREFIX_A)


#ifndef DEBUG_PRINTF_BUFFER_SIZE
#define DEBUG_PRINTF_BUFFER_SIZE    512
#endif // !DEBUG_PRINTF_BUFFER_SIZE


#ifdef _DEBUG

#define DEBUG_PRINT_W(message) OutputDebugStringW(ARMADA_PREFIX_W message)
#define DEBUG_PRINT_A(message) OutputDebugStringA(ARMADA_PREFIX_A message)

#define DEBUG_PRINTF_W(fmt, ...)               \
    do {                                       \
        wchar_t buf[DEBUG_PRINTF_BUFFER_SIZE]; \
        swprintf(buf, sizeof(buf) / sizeof(wchar_t), ARMADA_PREFIX_W fmt, __VA_ARGS__); \
        OutputDebugStringW(buf);                                                        \
    } while(0)

#define DEBUG_PRINTF_A(fmt, ...)            \
    do {                                    \
        char buf[DEBUG_PRINTF_BUFFER_SIZE]; \
        snprintf(buf, sizeof(buf) / sizeof(char), ARMADA_PREFIX_A fmt, __VA_ARGS__);    \
        OutputDebugStringA(buf);                                                        \
    } while(0)

#else // !_DEBUG

// Remove the debug print macros for release builds

#define DEBUG_PRINT_W(message)
#define DEBUG_PRINT_A(message)

#define DEBUG_PRINTF_W(fmt, ...)
#define DEBUG_PRINTF_A(fmt, ...)

#endif // _DEBUG

#ifdef UNICODE
#define DEBUG_PRINT     DEBUG_PRINT_W
#define DEBUG_PRINTF     DEBUG_PRINTF_W
#else
#define DEBUG_PRINT     DEBUG_PRINT_A
#define DEBUG_PRINTF     DEBUG_PRINTF_A
#endif

#endif // !DEBUG_MACROS_FOR_ARMADA_WRAPPER

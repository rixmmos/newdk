//--------------------------------------------------------------------------------
//
// Filename   : Assert.h
// Written By : Reiot
//
//--------------------------------------------------------------------------------

#ifndef __ASSERT_H__
#define __ASSERT_H__

// include files
#include "Exception.h"
#include "Types.h"

//--------------------------------------------------------------------------------
// Both helpers end in an unconditional throw (Assert.cpp), so neither can return
// to its caller. They are marked [[noreturn]] because they are defined in a
// different translation unit and the build uses no LTO: without the attribute
// the compiler is obliged to assume they return, so every `default: Assert(false)`
// and `else { Assert(false); }` fall-through reads as a live path and every
// variable the sibling branches assign looks possibly-uninitialised downstream.
// That accounts for most of the -O2 -Wmaybe-uninitialized inventory.
//
// SHARP EDGE: under -DNDEBUG, Assert(expr) below expands to ((void)0), the helper
// is never called, and those fall-throughs genuinely execute with the variables
// still uninitialised. In that configuration the warnings are not false positives
// and the attribute does nothing to help. NDEBUG is set only by `make release`
// (CMAKE_CXX_FLAGS_RELEASE); the default target, CI, and the deployed binaries
// are all Debug, so the fall-throughs really are unreachable there.
//--------------------------------------------------------------------------------
[[noreturn]] void __assert__(const char* file, uint line, const char* func, const char* expr) noexcept(false);

//--------------------------------------------------------------------------------
//


//
//--------------------------------------------------------------------------------
[[noreturn]] void __protocol_assert__(const char* file, uint line, const char* func, const char* expr) noexcept(false);

#if defined(NDEBUG)
#define Assert(expr) ((void)0)
#elif defined(__LINUX__) || defined(__APPLE__)
#define Assert(expr) ((void)((expr) ? 0 : (__assert__(__FILE__, __LINE__, __PRETTY_FUNCTION__, #expr), 0)))
#define ProtocolAssert(expr) \
    ((void)((expr) ? 0 : (__protocol_assert__(__FILE__, __LINE__, __PRETTY_FUNCTION__, #expr), 0)))
#elif __WIN_CONSOLE__ || __WIN32__
#define Assert(expr) ((void)((expr) ? 0 : (__assert__(__FILE__, __LINE__, __FUNCTION__, #expr), 0)))
#elif __MFC__
#define Assert(expr) ASSERT(expr)
#endif

#endif

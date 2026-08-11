//--------------------------------------------------------------------------------
//
// Filename   : Assert1.h
// Written By : Reiot
//
//--------------------------------------------------------------------------------

#ifndef __ASSERT_H__
#define __ASSERT_H__

// include files
#include "Exception.h"
#include "Types.h"

//--------------------------------------------------------------------------------
// This header is a near-duplicate of Assert.h and deliberately reuses its
// __ASSERT_H__ include guard, so in any translation unit exactly one of the two
// is in effect and the other expands to nothing. 101 files include this one
// rather than Assert.h, which is why the [[noreturn]] on the helpers has to be
// repeated here: without it those TUs would still be compiled against a
// declaration that may return, and none of the dead-fall-through analysis
// Assert.h describes would apply to them. The rationale and the NDEBUG caveat
// are written out once, in Assert.h -- keep the two declarations in step.
// Assert1.cpp is commented out of Core/CMakeLists.txt, so the definitions these
// resolve to are always the ones in Assert.cpp.
//--------------------------------------------------------------------------------
[[noreturn]] void __assert__(const char* file, uint line, const char* func, const char* expr) noexcept(false);

//--------------------------------------------------------------------------------
//


//
//--------------------------------------------------------------------------------
[[noreturn]] void __protocol_assert__(const char* file, uint line, const char* func, const char* expr);

#ifdef NDEBUG
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

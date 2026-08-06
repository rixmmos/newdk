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
//



//
//--------------------------------------------------------------------------------
void __assert__(const char* file, uint line, const char* func, const char* expr) noexcept(false);

//--------------------------------------------------------------------------------
//


//
//--------------------------------------------------------------------------------
void __protocol_assert__(const char* file, uint line, const char* func, const char* expr) noexcept(false);

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

//--------------------------------------------------------------------------------
//
// Filename   : Assert.h
// Written By : Reiot
//
//--------------------------------------------------------------------------------

#ifndef __ASSERT_H__
#define __ASSERT_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"

//--------------------------------------------------------------------------------
//


//
//--------------------------------------------------------------------------------
void __assert__ ( const char * file , uint line , const char * func , const char * expr ) throw ( AssertionError );

#if defined(NDEBUG)
	#define Assert(expr) ((void)0)
#elif __LINUX__
	#define Assert(expr) ((void)((expr)?0:(__assert__(__FILE__,__LINE__,__PRETTY_FUNCTION__,#expr),0)))
#elif __WIN_CONSOLE__ || __WIN32__ 
	#define Assert(expr) ((void)((expr)?0:(__assert__(__FILE__,__LINE__,"",#expr),0)))
#elif __MFC__
	#define Assert(expr) ASSERT(expr)
#endif

#endif

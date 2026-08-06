 

#pragma once

#ifdef PLATFORM_WINDOWS
extern LONG __stdcall RecordExceptionInfo( _EXCEPTION_POINTERS* pExp );
#else
// Declaration for non-Windows platforms (implementation in CrashReport.cpp)
// Note: Must match LPTOP_LEVEL_EXCEPTION_FILTER signature: long (*)(struct _EXCEPTION_POINTERS*)
LONG __stdcall RecordExceptionInfo( struct _EXCEPTION_POINTERS* pExp );
#endif


#define OLD_CRASH_LOG_FILENAME	"CrashReport.log"
#define CRASH_LOG_FILENAME		"CrashReport2.log"

#define InitCrashReport()		SetUnhandledExceptionFilter( RecordExceptionInfo )

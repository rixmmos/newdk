

#ifndef ___MIN_TRACE___
#define ___MIN_TRACE___

#ifdef PLATFORM_WINDOWS
#include <TCHAR.H>
#include <WTYPES.H>
#else
#include <wchar.h>
#include <ctype.h>
#include <string.h>
#endif
#include <stdio.h>
 

#define MIN_TRACE_ON        1      

//-----------------------------

//-----------------------------

#define MIN_CLRSCR			1		

#define MIN_TRANSPARENTWND	2		
									

#define MIN_HIDEWND			3		
#define MIN_SHOWWND			4		

#define MIN_TIME			5		
#define MIN_TIMELN			6		
#define MIN_DATE			7		
#define MIN_DATELN			8
#define MIN_DATETIME		9        
#define MIN_DATETIMELN		10      
									
									
                                    // 12:12:12
									

									
									

#define MIN_TOPMOST			11		
#define MIN_NOTOPMOST		12		

#define MIN_STOP            13      
#define MIN_RUN             14      




#ifdef PLATFORM_WINDOWS
static LPCTSTR g_pszMinTraceTitle     = _T("MinTrace 2003");
static LPCTSTR g_pszMinTraceClassName = _T("__MinTrace Window__");
#else
static const char* g_pszMinTraceTitle     = "MinTrace 2003";
static const char* g_pszMinTraceClassName = "__MinTrace Window__";
#endif

#define MIN_UNICOD		0xFFFF0000
#define MIN_NOTUNI		0x00000000
#define MIN_COL         0x1100
#define MIN_CMD         0x2200
#define MIN_ERR			0x0033
#define MIN_WAR			0x0044

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
inline void _MinTraceA(LPCSTR p)
{
    COPYDATASTRUCT cd;
    HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle);
    if (hWnd)
    {
        cd.dwData = MIN_NOTUNI;
        cd.cbData = (strlen(p)+1)*sizeof(char);
        cd.lpData = (void *)p;
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);
    }
}
#else
inline void _MinTraceA(LPCSTR p)
{
    // Non-Windows: output to stderr
    fprintf(stderr, "[MinTrace] %s\n", p ? p : "");
}
#endif

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
inline void _MinTraceW(LPCWSTR p)
{
    COPYDATASTRUCT cd;
    HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle);
    if (hWnd)
    {
        cd.dwData = MIN_UNICOD;
        cd.cbData = (wcslen(p)+1)*sizeof(wchar_t);
        cd.lpData = (void *)p;
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);
    }
}
#else
inline void _MinTraceW(LPCWSTR p)
{
    // Non-Windows: output to stderr
    fprintf(stderr, "[MinTrace] %ls\n", p ? p : L"");
}
#endif

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline void _MinTrace(LPCTSTR pFormat, ...)
{
    va_list args;
	va_start(args, pFormat);
		
    _TCHAR buffer [1024*sizeof(_TCHAR)];

#ifdef UNICODE
	vswprintf(buffer, pFormat, args);
    _MinTraceW(buffer);
#else
	vsprintf(buffer, pFormat, args);
    _MinTraceA(buffer);
#endif
	
    va_end(args);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
inline void _MinTraceErrA(LPCSTR p)
{
    COPYDATASTRUCT cd; 
    HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle); 
    if (hWnd)
    {  
        cd.dwData = MIN_NOTUNI | MIN_ERR;
        cd.cbData = (strlen(p)+1)*sizeof(char);
        cd.lpData = (void *)p; 
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);  
    } 
}
#else
inline void _MinTraceErrA(LPCSTR p)
{
    fprintf(stderr, "[_MinTraceErrA] %s\n", p ? p : "");
}
#endif

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
inline void _MinTraceErrW(LPCWSTR p)
{
    COPYDATASTRUCT cd; 
    HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle); 
    if (hWnd)
    {  
        cd.dwData = MIN_UNICOD | MIN_ERR;
        cd.cbData = (wcslen(p)+1)*sizeof(wchar_t);
        cd.lpData = (void *)p; 
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);  
    } 
}
#else
inline void _MinTraceErrW(LPCWSTR p)
{
    // On non-Windows platforms, just note that a wide char error occurred
    fprintf(stderr, "[_MinTraceErrW] (wide char error)\n");
    (void)p; // Suppress unused parameter warning
}
#endif

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline void _MinTraceErr(LPCTSTR pFormat, ...)
{
    va_list args;
	va_start(args, pFormat);
	
    _TCHAR buffer [1024*sizeof(_TCHAR)];
	
#ifdef UNICODE
	vswprintf(buffer, pFormat, args);
    _MinTraceErrW(buffer);
#else
	vsprintf(buffer, pFormat, args);
    _MinTraceErrA(buffer);
#endif
	
    va_end(args);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
inline void _MinTraceWarA(LPCSTR p)
{
    COPYDATASTRUCT cd; 
    HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle); 
    if (hWnd)
    {  
        cd.dwData = MIN_NOTUNI | MIN_WAR;
        cd.cbData = (strlen(p)+1)*sizeof(char);
        cd.lpData = (void *)p; 
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);  
    } 
}
#else
inline void _MinTraceWarA(LPCSTR p)
{
    fprintf(stderr, "[_MinTraceWarA] %s\n", p ? p : "");
}
#endif

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
inline void _MinTraceWarW(LPCWSTR p)
{
    COPYDATASTRUCT cd; 
    HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle); 
    if (hWnd)
    {  
        cd.dwData = MIN_UNICOD | MIN_WAR;
        cd.cbData = (wcslen(p)+1)*sizeof(wchar_t);
        cd.lpData = (void *)p; 
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);  
    } 
}
#else
inline void _MinTraceWarW(LPCWSTR p)
{
    // On non-Windows platforms, just note that a wide char warning occurred
    fprintf(stderr, "[_MinTraceWarW] (wide char warning)\n");
    (void)p; // Suppress unused parameter warning
}
#endif
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline void _MinTraceWar(LPCTSTR pFormat, ...)
{
    va_list args;
	va_start(args, pFormat);
	
    _TCHAR buffer [1024*sizeof(_TCHAR)];
	
#ifdef UNICODE
	vswprintf(buffer, pFormat, args);
    _MinTraceWarW(buffer);
#else
	vsprintf(buffer, pFormat, args);
    _MinTraceWarA(buffer);
#endif
	
    va_end(args);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
inline void _ClMinTraceA(COLORREF col, LPCSTR p)
{
    COPYDATASTRUCT cd;
    HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle);
    if (hWnd)
    {
		char buf[1024];
		sprintf(buf, "%8d,%s", col, p);
        cd.dwData = MIN_NOTUNI | MIN_COL;
        cd.cbData = (strlen(buf)+1)*sizeof(char);
        cd.lpData = (void *)buf;
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);
    }
}
#endif

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
inline void _ClMinTraceW(COLORREF col, LPCWSTR p)
{
#ifdef UNICODE
    COPYDATASTRUCT cd;
    HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle);
    if (hWnd)
    {
		_TCHAR buf[1024*sizeof(_TCHAR)];
		swprintf(buf, _T("%8d,%s"), col, p);
        cd.dwData = MIN_UNICOD | MIN_COL;
        cd.cbData = (wcslen(buf)+1)*sizeof(_TCHAR);
        cd.lpData = (void *)buf;
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);
    }
#endif
}
#endif

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
inline void _ClMinTrace(COLORREF col, LPCTSTR pFormat, ...)
{
    va_list args;
	va_start(args, pFormat);

    _TCHAR buffer [1024*sizeof(_TCHAR)];

#ifdef UNICODE
	vswprintf(buffer, pFormat, args);
    _ClMinTraceW(col, buffer);
#else
	vsprintf(buffer, pFormat, args);
    _ClMinTraceA(col, buffer);
#endif

    va_end(args);
}
#else
// Stub for non-Windows platforms
inline void _ClMinTrace(COLORREF col, LPCTSTR pFormat, ...) { (void)col; (void)pFormat; }
#endif

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
inline void _CmdMinTraceA(int nCmd, LPCSTR p)
{
    COPYDATASTRUCT cd;
    HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle);
    if (hWnd)
    {
		char buf[1024];
		sprintf(buf, "%8d,%s", nCmd, p);
        cd.dwData = MIN_NOTUNI | MIN_CMD;
        cd.cbData = (strlen(buf)+1)*sizeof(char);
        cd.lpData = (void *)&buf;
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);
    }
}
#else
inline void _CmdMinTraceA(int nCmd, LPCSTR p)
{
    fprintf(stderr, "[CmdMinTrace] cmd=%d %s\n", nCmd, p ? p : "");
}
#endif

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline void _CmdMinTraceW(int nCmd, LPCWSTR p)
{
#ifdef UNICODE
    COPYDATASTRUCT cd; 
    HWND hWnd = ::FindWindow (g_pszMinTraceClassName, g_pszMinTraceTitle); 
    if (hWnd)
    {  
		_TCHAR buf[1024];
		wsprintf(buf, _T("%8d,%s"), nCmd, p);
        cd.dwData = MIN_UNICOD | MIN_CMD;
        cd.cbData = (wcslen(buf)+1)*sizeof(_TCHAR);
        cd.lpData = (void *)&buf;
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);  
    } 
#endif
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline void _CmdMinTrace(int nCmd, LPCTSTR pFormat, ...)
{
    va_list args;
	va_start(args, pFormat);
	
    _TCHAR buffer [1024*sizeof(_TCHAR)];
	
#ifdef UNICODE
	vswprintf(buffer, pFormat, args);
    _CmdMinTraceW(nCmd, buffer);
#else
	vsprintf(buffer, pFormat, args);
    _CmdMinTraceA(nCmd, buffer);
#endif
	
    va_end(args);
}

#ifdef _DEBUG
  #if MIN_TRACE_ON
    #define MinTrace ::_MinTrace
  #else
    #define MinTrace TRACE
  #endif
#else
  #define MinTrace ((void)0)
#endif

#ifdef _DEBUG
  #if MIN_TRACE_ON
    #define MinTraceErr ::_MinTraceErr
  #else
    #define MinTraceErr TRACE
  #endif
#else
  #define MinTraceErr ((void)0)
#endif

#ifdef _DEBUG
  #if MIN_TRACE_ON
    #define MinTraceWar ::_MinTraceWar
  #else
    #define MinTraceWar TRACE
  #endif
#else
  #define MinTraceWar ((void)0)
#endif

#ifdef _DEBUG
  #if MIN_TRACE_ON
    #define ClMinTrace ::_ClMinTrace
  #else
    #define ClMinTrace ((void)0)
  #endif
#else
  #define ClMinTrace ((void)0)
#endif

#ifdef _DEBUG
  #if MIN_TRACE_ON
    #define CmdMinTrace ::_CmdMinTrace
  #else
    #define CmdMinTrace ((void)0)
  #endif
#else
  #define CmdMinTrace ((void)0)
#endif

#define MinTraceRelease	  	::_MinTrace
#define MinTraceWarRelease	::_MinTraceWar
#define MinTraceErrRelease	::_MinTraceErr 
#define ClMinTraceRelease   ::_ClMinTrace
#define CmdMinTraceRelease  ::_CmdMinTrace

#endif


//----------------------------------------------------------------------
// CMessageArray.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "CMessageArray.h"
#include "DebugInfo.h"

#include <mutex>

#ifdef OUTPUT_DEBUG
	//#define OUTPUT_FILE_LOG
#endif

#define __LOGGING__

// Platform-specific includes
#ifdef PLATFORM_WINDOWS
	#include <io.h>
	#include <fcntl.h>
#else
	#include <unistd.h>
	#include <fcntl.h>
#endif

// Platform-specific I/O functions
#ifdef PLATFORM_WINDOWS
	#define PLATFORM_WRITE(fd, buf, len)	_write(fd, buf, len)
	#define PLATFORM_OPEN	_open
	#define PLATFORM_CLOSE	_close
	#define PLATFORM_LSEEK	_lseek
#else
	#define PLATFORM_WRITE(fd, buf, len)	write(fd, buf, len)
	#define PLATFORM_OPEN	open
	#define PLATFORM_CLOSE	close
	#define PLATFORM_LSEEK	lseek
#endif

// Platform-specific file flags
#ifndef PLATFORM_WINDOWS
	#define _O_WRONLY    O_WRONLY
	#define _O_TEXT      0
	#define _O_APPEND    O_APPEND
	#define _O_CREAT     O_CREAT
	#define _O_TRUNC     O_TRUNC
#endif

#if defined(OUTPUT_DEBUG) && defined(__GAME_CLIENT__)
	// std::recursive_mutex, not std::mutex: RequestServerPlayerManager::
	// AddRequestServerPlayer() holds g_Lock across g_pGameMessage->AddFormat(),
	// which takes g_Lock again (CRITICAL_SECTION allowed that re-entry).
	std::recursive_mutex		g_Lock;

	#define __BEGIN_LOCK	g_Lock.lock();
	#define __END_LOCK		g_Lock.unlock();
#else
	#define __BEGIN_LOCK	((void)0);
	#define __END_LOCK		((void)0);
#endif


//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
CMessageArray::CMessageArray()
{
	m_Max		= 0;
	m_Length	= 0;
	m_Current	= 0;
	m_ppMessage = NULL;

	m_bLog		= false;
	m_LogFile	= 0;
	m_Filename	= NULL;
}

CMessageArray::~CMessageArray()
{
	Release();
}


//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void
CMessageArray::Init(int max, int length, const char* filename)
{
	
	Release();

	m_Max		= max;
	m_Length	= length;
	m_Current	= 0;

	// new
	m_ppMessage = new char* [ m_Max ];

	for (int i=0; i<m_Max; i++)
	{
		m_ppMessage[i] = new char [ m_Length+1 ];
		m_ppMessage[i][0] = NULL;
	}

	// file Log
	if (filename!=NULL)
	{
		
		m_Filename = new char [strlen(filename)+1];
		strcpy(m_Filename, filename);

		m_LogFile = PLATFORM_OPEN(filename, _O_WRONLY | _O_TEXT | _O_CREAT | _O_TRUNC);

		if (m_LogFile!=-1)
		{
			m_bLog = true;
		}
	}
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
CMessageArray::Release()
{
	
	
	
	//#ifndef _DEBUG
		if (m_ppMessage!=NULL)
		{
			for (int i=0; i<m_Max; i++)
			{
				if (m_ppMessage[i]!=NULL)
				{			
					delete [] m_ppMessage[i];			
				}
			}

			delete [] m_ppMessage;

			m_Max		= 0;
			m_Length	= 0;
			m_Current	= 0;
			m_ppMessage = NULL;
		}
	//#endif

	// file log
	if (m_bLog)
	{
		PLATFORM_CLOSE(m_LogFile);
		m_bLog = false;

		if (m_Filename != NULL)
		{
			delete [] m_Filename;
		}
	}
}

//----------------------------------------------------------------------
// Add 
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
CMessageArray::Add(const char *str)
{	
	#ifndef __LOGGING__
		return;
	#endif

	__BEGIN_LOCK	

	int len = strlen(str);
	
	// file log
	if (m_bLog)
	{ 
		
		//sprintf(g_MessageBuffer, "[%4d] ", timeGetTime() % 10000);
		//PLATFORM_WRITE( m_LogFile, g_MessageBuffer, strlen(g_MessageBuffer) );

		//m_LogFile << str << endl;
		PLATFORM_WRITE( m_LogFile, str, len );
		PLATFORM_WRITE( m_LogFile, "\n", 1 );

		
		#ifdef OUTPUT_FILE_LOG
			PLATFORM_CLOSE( m_LogFile );
			m_LogFile = PLATFORM_OPEN(m_Filename, _O_WRONLY | _O_TEXT | _O_APPEND | _O_CREAT);
		#endif
	}	

	if (len >= m_Length)
	{		
		for (int i=0; i<m_Length; i++)
		{
			m_ppMessage[m_Current][i] = str[i];
		}	
		m_ppMessage[m_Current][m_Length] = NULL;		
		//strcpy(m_ppMessage[m_Current], "[ERROR] CMessageArray::Add() - String too long");		
	}
	else
	{
		
		strcpy(m_ppMessage[m_Current], str);
	}

	m_Current++;
	if (m_Current==m_Max) m_Current=0;

	__END_LOCK
}

//----------------------------------------------------------------------
// Add To File
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
CMessageArray::AddToFile(const char *str)
{
	#ifndef __LOGGING__
		return;
	#endif

	__BEGIN_LOCK
	
	// file log
	if (m_bLog)
	{
		
		//sprintf(g_MessageBuffer, "[%4d] ", timeGetTime() % 10000);
		//PLATFORM_WRITE( m_LogFile, g_MessageBuffer, strlen(g_MessageBuffer) );

		//m_LogFile << str << endl;
		PLATFORM_WRITE( m_LogFile, str, strlen( str ) );
		PLATFORM_WRITE( m_LogFile, "\n", 1 );

		
		#ifdef OUTPUT_FILE_LOG
			PLATFORM_CLOSE( m_LogFile );
			m_LogFile = PLATFORM_OPEN(m_Filename, _O_WRONLY | _O_TEXT | _O_APPEND | _O_CREAT);
		#endif
	}	

	__END_LOCK
}

//--------------------------------------------------------------------------
// Add Format VL
//--------------------------------------------------------------------------
void
CMessageArray::AddFormatVL(const char* format, va_list& vl)
{
	__BEGIN_LOCK

	//AddFormat( format, vl );
//	va_list		vl;
	static char Buffer[4096];

//    va_start(vl, format);
	vsprintf(Buffer, format, vl);    
 //   va_end(vl);

	int len = strlen(Buffer);
 
	// file log
	if (m_bLog)
	{
		
		//sprintf(g_MessageBuffer, "[%4d] ", timeGetTime() % 10000);
		//PLATFORM_WRITE( m_LogFile, g_MessageBuffer, strlen(g_MessageBuffer) );

		//m_LogFile << str << endl;
		PLATFORM_WRITE( m_LogFile, Buffer, len );
		PLATFORM_WRITE( m_LogFile, "\n", 1 );

		
		#ifdef OUTPUT_FILE_LOG
			PLATFORM_CLOSE( m_LogFile );
			m_LogFile = PLATFORM_OPEN(m_Filename, _O_WRONLY | _O_TEXT | _O_APPEND | _O_CREAT);
		#endif
	}	

	
	if (len >= m_Length)
	{		
		for (int i=0; i<m_Length; i++)
		{
			m_ppMessage[m_Current][i] = Buffer[i];
		}	

		m_ppMessage[m_Current][m_Length] = NULL;	
	}
	else
	{
		
		strcpy(m_ppMessage[m_Current], Buffer);
	}
	

	m_Current++;
	if (m_Current==m_Max) m_Current=0;

	__END_LOCK
}

//--------------------------------------------------------------------------
// Add Format
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
CMessageArray::AddFormat(const char* format, ...)
{
	#ifndef __LOGGING__
		return;
	#endif

	__BEGIN_LOCK
	

	va_list		vl;
	static char Buffer[4096];

    va_start(vl, format);
	vsprintf(Buffer, format, vl);    
    va_end(vl);

	int len = strlen(Buffer);
 
	// file log
	if (m_bLog)
	{
		
		//sprintf(g_MessageBuffer, "[%4d] ", timeGetTime() % 10000);
		//PLATFORM_WRITE( m_LogFile, g_MessageBuffer, strlen(g_MessageBuffer) );

		//m_LogFile << str << endl;
		PLATFORM_WRITE( m_LogFile, Buffer, len );
		PLATFORM_WRITE( m_LogFile, "\n", 1 );

		
		#ifdef OUTPUT_FILE_LOG
			PLATFORM_CLOSE( m_LogFile );
			m_LogFile = PLATFORM_OPEN(m_Filename, _O_WRONLY | _O_TEXT | _O_APPEND | _O_CREAT);
		#endif
	}	

	
	if (len >= m_Length)
	{		
		for (int i=0; i<m_Length; i++)
		{
			m_ppMessage[m_Current][i] = Buffer[i];
		}	

		m_ppMessage[m_Current][m_Length] = NULL;	
	}
	else
	{
		
		strcpy(m_ppMessage[m_Current], Buffer);
	}
	

	m_Current++;
	if (m_Current==m_Max) m_Current=0;

	__END_LOCK
}

//----------------------------------------------------------------------
// Next
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CMessageArray::Next()
{	
	#ifndef __LOGGING__
		return;
	#endif

	__BEGIN_LOCK

	// file log
	if (m_bLog)
	{
		//m_LogFile << str << endl;
		PLATFORM_WRITE( m_LogFile, m_ppMessage[m_Current], strlen( m_ppMessage[m_Current] ) );
		PLATFORM_WRITE( m_LogFile, "\n", 1 );
	}	

	m_Current++; 
	if (m_Current==m_Max) m_Current=0;

	__END_LOCK
}

//----------------------------------------------------------------------
// operator []
//----------------------------------------------------------------------
// 0 ~ MAX-1


//----------------------------------------------------------------------
const char*	
CMessageArray::operator [] (int i)
{ 
	
	//m_Current - (3-[0]) = m_Current;
	//m_Current - (3-[1]) = m_Current - 2;
	//m_Current - (3-[2]) = m_Current - 1;

	__BEGIN_LOCK

	BYTE gap = m_Max - i;

	if (gap > m_Current)
	{

		const char* pMessage = m_ppMessage[m_Current + m_Max - gap];
		__END_LOCK

		return pMessage;
	}
	
	const char* pMessage = m_ppMessage[m_Current - gap];	

	__END_LOCK

	return pMessage;
}

//----------------------------------------------------------------------
// Clear
//----------------------------------------------------------------------
void
CMessageArray::Clear()
{
	for (int i=0; i<m_Max; i++)
	{
		m_ppMessage[i][0] = NULL;
	}
}
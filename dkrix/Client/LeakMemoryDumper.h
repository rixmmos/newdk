//////////////////////////////////////////////////////////////////////////////
/// \file LeakMemoryDumper.h
/// \author sonee
/// \date 2004.12.30
//////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef _DEBUG
#include <cstddef>
#include <cstdlib>
#include <cstdint>

void AddTrack(std::uintptr_t addr, std::size_t asize, const char *fname, DWORD lnum);
void DumpUnfreed();
void RemoveTrack(std::uintptr_t addr);

__inline void * __cdecl operator new(std::size_t size,
								   const char *file, int line)
{
	void *ptr = (void *)malloc(size);
	AddTrack(reinterpret_cast<std::uintptr_t>(ptr), size, file, line);
	return(ptr);
}

__inline void __cdecl operator delete(void *p)
{
	RemoveTrack(reinterpret_cast<std::uintptr_t>(p));
	free(p);
}

#endif

#ifdef _DEBUG
	#define DEBUG_NEW new(__FILE__, __LINE__)
#else
	#define DEBUG_NEW new
#endif

#define new DEBUG_NEW

#pragma warning(disable:4291)

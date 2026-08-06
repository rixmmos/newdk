#include <fstream>
//----------------------------------------------------------------------
// CStorageSurface.h
//----------------------------------------------------------------------


//----------------------------------------------------------------------

#ifndef	__CSTORAGESURFACE_H__
#define	__CSTORAGESURFACE_H__


#pragma warning(disable:4786)


#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif
class CSpriteSurface;


class CStorageSurface {
	public :
		CStorageSurface();
		~CStorageSurface();

		//--------------------------------------------------------
		// Init / Release
		//--------------------------------------------------------
		
		void					Init(int size, int width, int height);
		void					Release();

		//--------------------------------------------------------
		// Store / Restore		
		//--------------------------------------------------------
		void					Store(int i, CSpriteSurface* pSurface, POINT* pPoint);
		void					Restore(int i, CSpriteSurface* pSurface, POINT* pPoint=NULL) const;

		//--------------------------------------------------------
		// Get
		//--------------------------------------------------------
		int						GetSize() const		{ return m_Size; }

	protected :
		int						m_Size;				
		POINT*					m_pPoint;			
		CSpriteSurface*			m_pStorageSurface;	
};


#endif

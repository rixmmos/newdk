#include <fstream>
//----------------------------------------------------------------------
// CFilter.h
//----------------------------------------------------------------------
//

//

//
//----------------------------------------------------------------------

#ifndef	__CFILTER_H__
#define	__CFILTER_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif
#include "CTypePack.h"

class CFilter {
	public :
		CFilter();
		~CFilter();

		bool		IsNotInit() const	{ return m_ppFilter==NULL; }
		bool		IsInit() const		{ return m_ppFilter==NULL; }

		//---------------------------------------------------------
		// Init / Release
		//---------------------------------------------------------
		void		Init(WORD width, WORD height);
		void		Release();

		//---------------------------------------------------------
		// Set Filter Value
		//---------------------------------------------------------
		
		void		SetFilter(WORD* pSurface, WORD pitch, WORD width, WORD height);
		
		
		void		SetFilter(const CFilter& filter);

		
		void		SetFilterDarkness(const CFilter& filter, BYTE DarkBits);

		
		void		SetFilter(WORD x, WORD y, BYTE value)	{ m_ppFilter[y][x]=value; }

		//---------------------------------------------------------
		// Blt
		//---------------------------------------------------------
		void		BltFilter(int x, int y, const CFilter& filter);
		void		BltFilterAdd(int x, int y, const CFilter& filter);
		void		BltFilterSub(int x, int y, const CFilter& filter);
		void		Blt4444(WORD* pSurface, WORD pitch);
		void		Blt4444Color(WORD* pSurface, WORD pitch, WORD color);

		//---------------------------------------------------------
		// File I/O
		//---------------------------------------------------------
		bool		SaveToFile(std::ofstream& file);
		bool		LoadFromFile(std::ifstream& file);

		//---------------------------------------------------------
		// Get
		//---------------------------------------------------------
		WORD		GetWidth() const		{ return m_Width; }
		WORD		GetHeight() const		{ return m_Height; }
		BYTE*		GetFilter(WORD y) const	{ return m_ppFilter[y]; }


		

	protected :
		WORD		m_Width;
		WORD		m_Height;
		BYTE**		m_ppFilter;
};

// Not platform-guarded: MTopView.h:687 declares a CFilterPack member
// unconditionally, and the sibling aliases (CAlphaSpritePalPack,
// CShadowSpritePack, CSpritePalPack) are all unguarded too. The #ifdef here
// was an oversight from a partial port -- it made this the only one of the
// four missing on Linux.
typedef CTypePack<CFilter>			CFilterPack;

#endif


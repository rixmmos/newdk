#include <fstream>
//----------------------------------------------------------------------
// CSprite.h
//----------------------------------------------------------------------

#ifndef	__CSPRITE_H__
#define	__CSPRITE_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif

#include "CSpriteSurface.h"
#include "CSpriteDef.h"

#ifdef SPRITELIB_BACKEND_SDL
#include "SpriteLibBackend.h"
#endif

class CFilter;

#ifdef SPRITELIB_BACKEND_SDL
/* Forward declaration for friend declaration */
class CSpriteSurface;
#endif

std::ofstream;
std::ifstream;


//----------------------------------------------------------------------

//----------------------------------------------------------------------

//
//    [1] 5:5:5 --> 2 bytes
//    [2] 5:6:5 --> 2 bytes
//    [3] R,G,B --> 3 bytes
//


//


//



//
//----------------------------------------------------------------------

//

//

//
//----------------------------------------------------------------------

class CSprite
{
	public :
		CSprite();
		virtual ~CSprite();

		bool		IsNotInit() const	{ return !m_bInit; }
		bool		IsInit() const		{ return m_bInit; }
		bool		IsLoading() const	{ return m_bLoading; }

		void		operator = (const CSprite& Sprite);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		//static void	InitBuffer(WORD width, WORD height);
		//static void ReleaseBuffer();

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		Release();

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		static void	SetColorkey(WORD color)			{ s_Colorkey = color; }
		static WORD	GetColorkey() 					{ return s_Colorkey; }

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		virtual bool		SaveToFile(std::ofstream& file) = 0;
		virtual bool		LoadFromFile(std::ifstream& file) = 0;		
		//virtual bool		LoadFromFileToBuffer(std::ifstream& file) = 0;
		//void		LoadFromBuffer();
			
		DWORD		GetFileSize();
		
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		static void	GetTightColorRect(WORD *pSource, WORD pitch, WORD width, WORD height, WORD colorkey, RECT& rect);
		void		SetPixel(WORD* pSource, WORD pitch, WORD width, WORD height);
		void		SetPixelNoColorkey(WORD* pSource, WORD pitch, WORD width, WORD height);

		//---------------------------------------------------------
		// get functions		
		//---------------------------------------------------------
		WORD		GetWidth()		const			{ return m_Width; }
		WORD		GetHeight()		const			{ return m_Height; }
		WORD		GetPixel(int x, int y) const;
		WORD*		GetPixelLine(WORD y)	const	{ return m_Pixels[y]; }	

#ifdef SPRITELIB_BACKEND_SDL
	/* Backend sprite management */
	spritectl_sprite_t GetBackendSprite() const	{ return m_backend_sprite; }
	bool IsBackendDirty() const			{ return m_backend_dirty; }
	void SetBackendSprite(spritectl_sprite_t sprite)	{ m_backend_sprite = sprite; }
	void SetBackendDirty(bool dirty)		{ m_backend_dirty = dirty; }
#endif

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		bool		IsColorPixel(short x, short y);		
	
		//---------------------------------------------------------
		//
		// Blt
		//
		//---------------------------------------------------------

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		Blt(WORD *pDest, WORD pitch);		
		void		BltClip(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltClipLeft(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltClipRight(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltClipWidth(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltClipHeight(WORD *pDest, WORD pitch, RECT* pRect);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
	

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		BltHalf(WORD *pDest, WORD pitch);		
		void		BltHalfClipLeft(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltHalfClipRight(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltHalfClipWidth(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltHalfClipHeight(WORD *pDest, WORD pitch, RECT* pRect);

		//---------------------------------------------------------
		// Alpha Blending
		//---------------------------------------------------------
		void		BltAlpha(WORD *pDest, WORD pitch, BYTE alpha);		
		void		BltAlphaClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha);
		void		BltAlphaClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha);
		void		BltAlphaClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha);
		void		BltAlphaClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		BltColor(WORD *pDest, WORD pitch, BYTE rgb);		
		void		BltColorClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE rgb);
		void		BltColorClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE rgb);
		void		BltColorClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE rgb);
		void		BltColorClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE rgb);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		BltScale(WORD *pDest, WORD pitch, BYTE scale);		
		void		BltScaleClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE scale);
		void		BltScaleClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE scale);
		void		BltScaleClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE scale);
		void		BltScaleClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE scale);


		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		BltDarkness(WORD *pDest, WORD pitch, BYTE DarkBits);		
		void		BltDarknessClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltDarknessClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltDarknessClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltDarknessClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);


		//---------------------------------------------------------
		// Effect
		//---------------------------------------------------------
		void		BltEffect(WORD *pDest, WORD pitch);				
		void		BltEffectClipLeft(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltEffectClipRight(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltEffectClipWidth(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltEffectClipHeight(WORD *pDest, WORD pitch, RECT* pRect);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		BltColorSet(WORD *pDest, WORD pitch, WORD colorSet);		
		void		BltColorSetClipLeft(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet);
		void		BltColorSetClipRight(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet);
		void		BltColorSetClipWidth(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet);
		void		BltColorSetClipHeight(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet);


		//---------------------------------------------------------
		
		//---------------------------------------------------------
		//void		BltDarknessFilter(WORD *pDest, WORD pitch, BYTE DarkBits, char cx, char cy, CSprite* pFilter);		
		//void		BltDarknessFilterClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits, char cx, char cy, CSprite* pFilter);
		//void		BltDarknessFilterClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits, char cx, char cy, CSprite* pFilter);
		//void		BltDarknessFilterClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits, char cx, char cy, CSprite* pFilter);
		//void		BltDarknessFilterClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits, char cx, char cy, CSprite* pFilter);

		//---------------------------------------------------------		
		//
		
		//
		//---------------------------------------------------------		
		bool		IsIntersectFilter();
		static void	SetFilter(CFilter* pFilter)
		{
			s_pFilter	= pFilter;
		}

		static void	SetFilter(short x, short y, CFilter* pFilter)
		{
			s_X			= x;
			s_Y			= y;
			s_pFilter	= pFilter;
		}

		static CFilter* GetFilter()
		{
			return s_pFilter;
		}

		//---------------------------------------------------------
		
		//---------------------------------------------------------		
		void		BltAlphaFilter(WORD *pDest, WORD pitch);
		void		BltAlphaFilterClipLeft(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltAlphaFilterClipRight(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltAlphaFilterClipWidth(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltAlphaFilterClipHeight(WORD *pDest, WORD pitch, RECT* pRect);

		//---------------------------------------------------------
		
		//---------------------------------------------------------		
		void		BltAlphaFilterDarkness(WORD *pDest, WORD pitch, BYTE DarkBits);
		void		BltAlphaFilterDarknessClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltAlphaFilterDarknessClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltAlphaFilterDarknessClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltAlphaFilterDarknessClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);

		//---------------------------------------------------------
		
		//---------------------------------------------------------		
		void		BltDarkerFilter(WORD *pDest, WORD pitch);
		void		BltDarkerFilterClipLeft(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltDarkerFilterClipRight(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltDarkerFilterClipWidth(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltDarkerFilterClipHeight(WORD *pDest, WORD pitch, RECT* pRect);
	
		//---------------------------------------------------------		
		// Alpha 4444 Small Not Trans
		//---------------------------------------------------------		
		void		BltAlpha4444SmallNotTrans(WORD *pDest, WORD pitch, BYTE alpha, BYTE shift);
		void		BltAlpha4444NotTrans(WORD *pDest, WORD pitch, BYTE alpha);

		//---------------------------------------------------------		
		// 1555 
		//---------------------------------------------------------		
		void		Blt1555SmallNotTrans(WORD *pDest, WORD pitch, BYTE shift);
		void		Blt1555NotTrans(WORD *pDest, WORD pitch);

	

		//---------------------------------------------------------
		// Utility Functions
		//---------------------------------------------------------
		//void		memcpyDarknessFilter(WORD* pDest, WORD* pSource, WORD* pFilter, WORD pixels);

		static void		memcpyAlphaFilter(WORD* pDest, WORD* pSource, BYTE* pFilter, WORD pixels);
		static void		memcpyAlphaFilterDarkness(WORD* pDest, WORD* pSource, BYTE* pFilter, WORD pixels);
		static void		memcpyDarkerFilter(WORD* pDest, WORD* pSource, BYTE* pFilter, WORD pixels);
		
		static void		memcpyAlpha4444(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpyAlpha4444Small(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpy1555(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpy1555Small(WORD* pDest, WORD* pSource, WORD pixels);




	protected :
		WORD			m_Width;		
		WORD			m_Height;		
		WORD**			m_Pixels;		// pixels
		bool			m_bInit;		
		bool			m_bLoading;		

#ifdef SPRITELIB_BACKEND_SDL
		spritectl_sprite_t	m_backend_sprite;	// Backend sprite handle
		bool			m_backend_dirty;	// True if m_Pixels changed but not synced to backend

		/* Allow CSpriteSurface to access backend members for blitting */
		friend class CSpriteSurface;
#endif

	public :
		static WORD		s_Colorkey;

		// Blt Value
		static DWORD	s_Value1;
		static DWORD	s_Value2;
		static DWORD	s_Value3;
	
		
		static short	s_X;
		static short	s_Y;
		static CFilter*	s_pFilter;
};


#endif


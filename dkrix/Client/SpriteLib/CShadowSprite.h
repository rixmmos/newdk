#include <fstream>
//----------------------------------------------------------------------
// CShadowSprite.h
//----------------------------------------------------------------------

#ifndef	__CSHADOWSPRITE_H__
#define	__CSHADOWSPRITE_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif

#ifdef SPRITELIB_BACKEND_SDL
#include "SpriteLibBackend.h"
#endif

#include "CSpriteDef.h"
#include "CIndexSprite.h"
#include "CSprite.h"


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
//
//----------------------------------------------------------------------

class CShadowSprite
{
	public :
		CShadowSprite();
		~CShadowSprite();

		bool		IsNotInit() const	{ return !m_bInit; }
		bool		IsInit() const		{ return m_bInit; }

		void		operator = (const CShadowSprite& Sprite);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		Release();

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		static void	SetColorkey(WORD color)			{ s_Colorkey = color; }
		static WORD	GetColorkey() 					{ return s_Colorkey; }

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		bool		SaveToFile(std::ofstream& file);
		bool		LoadFromFile(std::ifstream& file);
	
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		SetPixel(WORD* pSource, WORD sourcePitch, 							 
							 WORD width, WORD height);

		//---------------------------------------------------------
		// Sprite --> ShadowSprite
		//---------------------------------------------------------
		void		SetPixel(CIndexSprite& ispr);
		void		SetPixel(CSprite& spr);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		bool		IsColorPixel(short x, short y);

		//---------------------------------------------------------
		// get functions		
		//---------------------------------------------------------
		WORD		GetWidth()		const			{ return m_Width; }
		WORD		GetHeight()		const			{ return m_Height; }
		WORD		GetPixel(WORD x, WORD y)const	{ return m_Pixels[y][x]; }
		WORD*		GetPixelLine(WORD y)	const	{ return m_Pixels[y]; }

#ifdef SPRITELIB_BACKEND_SDL
		/* Backend sprite management */
		spritectl_sprite_t GetBackendSprite() const	{ return m_backend_sprite; }
		bool IsBackendDirty() const			{ return m_backend_dirty; }
		void SetBackendSprite(spritectl_sprite_t sprite)	{ m_backend_sprite = sprite; }
		void SetBackendDirty(bool dirty)		{ m_backend_dirty = dirty; }
#endif

		//---------------------------------------------------------
		//
		// Blt
		//
		//---------------------------------------------------------

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		Blt(WORD *pDest, WORD pitch);				
		void		BltClipLeft(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltClipRight(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltClipWidth(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltClipHeight(WORD *pDest, WORD pitch, RECT* pRect);

		//---------------------------------------------------------
		
		// Test Code~~
		//---------------------------------------------------------
		void		BltSmall(WORD *pDest, WORD pitch, BYTE shift);
		void		BltSmallClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE shift);
		void		BltSmallClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE shift);
		void		BltSmallClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE shift);
		void		BltSmallClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE shift);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		//void		BltRotate(WORD *pDest, WORD pitch, short deg);
		//void		BltRotateClipLeft(WORD *pDest, WORD pitch, RECT* pRect);
		//void		BltRotateClipRight(WORD *pDest, WORD pitch, RECT* pRect);
		//void		BltRotateClipWidth(WORD *pDest, WORD pitch, RECT* pRect);
		//void		BltRotateClipHeight(WORD *pDest, WORD pitch, RECT* pRect);
		

		//---------------------------------------------------------
		
		//---------------------------------------------------------
	

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		//void		BltHalf(WORD *pDest, WORD pitch);		
		//void		BltHalfClipLeft(WORD *pDest, WORD pitch, RECT* pRect);
		//void		BltHalfClipRight(WORD *pDest, WORD pitch, RECT* pRect);
		//void		BltHalfClipHeight(WORD *pDest, WORD pitch, RECT* pRect);

		//---------------------------------------------------------
		// Alpha Blending
		//---------------------------------------------------------
		//void		BltAlpha(WORD *pDest, WORD pitch);
		//void		BltAlphaClipLeft(WORD *pDest, WORD pitch, RECT* pRect);
		//void		BltAlphaClipRight(WORD *pDest, WORD pitch, RECT* pRect);
		//void		BltAlphaClipHeight(WORD *pDest, WORD pitch, RECT* pRect);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		//void		BltColor(WORD *pDest, WORD pitch, BYTE rgb);		
		//void		BltColorClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE rgb);
		//void		BltColorClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE rgb);
		//void		BltColorClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE rgb);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		BltDarkness(WORD *pDest, WORD pitch, BYTE DarkBits);		
		void		BltDarknessClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltDarknessClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltDarknessClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltDarknessClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		Blt4444(WORD *pDest, WORD pitch, WORD pixel);				
		void		Blt4444ClipLeft(WORD *pDest, WORD pitch, RECT* pRect, WORD pixel);
		void		Blt4444ClipRight(WORD *pDest, WORD pitch, RECT* pRect, WORD pixel);
		void		Blt4444ClipWidth(WORD *pDest, WORD pitch, RECT* pRect, WORD pixel);
		void		Blt4444ClipHeight(WORD *pDest, WORD pitch, RECT* pRect, WORD pixel);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		BltSmall4444(WORD *pDest, WORD pitch, WORD pixel, BYTE shift);				
		void		BltSmall4444ClipLeft(WORD *pDest, WORD pitch, RECT* pRect, WORD pixel, BYTE shift);
		void		BltSmall4444ClipRight(WORD *pDest, WORD pitch, RECT* pRect, WORD pixel, BYTE shift);
		void		BltSmall4444ClipWidth(WORD *pDest, WORD pitch, RECT* pRect, WORD pixel, BYTE shift);
		void		BltSmall4444ClipHeight(WORD *pDest, WORD pitch, RECT* pRect, WORD pixel, BYTE shift);


		//---------------------------------------------------------
		// Effect
		//---------------------------------------------------------
		//void		BltEffect(WORD *pDest, WORD pitch);				
		//void		BltEffectClipLeft(WORD *pDest, WORD pitch, RECT* pRect);
		//void		BltEffectClipRight(WORD *pDest, WORD pitch, RECT* pRect);
		//void		BltEffectClipHeight(WORD *pDest, WORD pitch, RECT* pRect);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		//void		BltShadowFilter(WORD *pDest, WORD pitch, CFilter* pFilter);		

		//---------------------------------------------------------
		// Utility Functions
		//---------------------------------------------------------
		void		memcpyShadowDarkness(WORD* pDest, WORD pixels);
		inline void		memcpyShadow4444(WORD* pDest, WORD pixels);
		//void			memcpyHalf(WORD* pDest, WORD* pSource, WORD pixels);
		//void		memcpyShadow(WORD* pDest, WORD* pSource, WORD pixels);
		//void			memcpyColor(WORD* pDest, WORD* pSource, WORD pixels);
		//void			memcpyDarkness(WORD* pDest, WORD* pSource, WORD pixels);

		//void			memcpyEffect(WORD* pDest, WORD* pSource, WORD pixels);
		//void		memcpyDarknessFilter(WORD* pDest, WORD* pSource, WORD* pFilter, WORD pixels);

	protected :
		WORD			m_Width;		
		WORD			m_Height;		
		WORD**			m_Pixels;		// pixels
		bool			m_bInit;		

#ifdef SPRITELIB_BACKEND_SDL
		spritectl_sprite_t	m_backend_sprite;	// Backend sprite handle
		bool			m_backend_dirty;	// True if m_Pixels changed but not synced to backend
#endif

		static WORD		s_Colorkey;

		// Blt Value
		static BYTE		s_Value1;
		static BYTE		s_Value2;
		static WORD		s_wValue1;
};

typedef CTypePack<CShadowSprite>			CShadowSpritePack;

#endif



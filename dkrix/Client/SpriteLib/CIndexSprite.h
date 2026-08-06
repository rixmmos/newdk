//-----------------------------------------------------------------------------
// CIndexSprite.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//
//    [1] 5:5:5 --> 2 bytes
//    [2] 5:6:5 --> 2 bytes
//    [3] R,G,B --> 3 bytes
//


//


//



//
//-----------------------------------------------------------------------------


//
//

//




//                              .     
//                              .     
//                              .     




//                              .     
//                              .     
//                              .     

//    )
//
//

//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------

#ifndef	__CINDEXSPRITE_H__
#define	__CINDEXSPRITE_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif

#ifdef SPRITELIB_BACKEND_SDL
#include "SpriteLibBackend.h"
#endif

#include <fstream>
using namespace std;

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define	MAX_COLORSET_SEED			33	
#define	MAX_COLORSET_SEED_MODIFY	15	
#define	MAX_COLORSET				495 // MAX_COLORSET_SEED * MAX_COLORSET_SEED_MODIFY
#define	MAX_COLORGRADATION			30
#define	MAX_COLOR_TO_GRADATION		93	// R+G+B
#define	MAX_COLORGRADATION_HALF		15	// == MAX_COLORSET_SEED_MODIFY
#define	MAX_DARKBIT					5


#define	MAX_COLORSET_USE			256


#define	INDEX_NULL					0xFF
#define	INDEX_SELECT				0xFE
#define	INDEX_TRANS					0xFD
#define	INDEXSET_NULL				0xFF

class CIndexSprite {
	public :
		CIndexSprite();
		~CIndexSprite();
		
		bool		IsNotInit() const	{ return !m_bInit; }
		bool		IsInit() const		{ return m_bInit; }

		void		operator = (const CIndexSprite& Sprite);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		static	BOOL	SaveIndexTableToFile(std::ofstream& file);
		static	BOOL	LoadIndexTableFromFile(std::ifstream& file);
		static	void	SetColorSet();
		static	void	SetUsingColorSet(int set1, int set2);
		static	void	SetUsingColorSetOnly(BYTE setNumber, int colorSet);
		static	int		GetUsingColorSet(BYTE n)	{ return s_IndexValue[n]; }		
		static	void	GetIndexColor(WORD* pColor, int step, int r0, int g0, int b0, int r1, int g1, int b1);
		


		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		SetPixel(WORD* pSource, WORD sourcePitch, 
							 WORD* pIndex1, WORD indexPitch1,	
							 WORD* pIndex2, WORD indexPitch2,	
							 WORD width, WORD height);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		static	BYTE	GetIndexColor(WORD color);
		static	BYTE	GetColorToGradation(BYTE color);

		//---------------------------------------------------------
		// Generate Index
		//---------------------------------------------------------
		void			GenerateFromIndex(WORD** ppColor, WORD width, WORD height, WORD** ppIndex);
		void			GetIndexInfo(WORD**& ppIndex);

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
		// Index Blt
		//---------------------------------------------------------
		void		Blt(WORD *pDest, WORD pitch);				
		void		BltClipLeft(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltClipRight(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltClipWidth(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltClipHeight(WORD *pDest, WORD pitch, RECT* pRect);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		BltDarkness(WORD *pDest, WORD pitch, BYTE DarkBits);		
		void		BltDarknessClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltDarknessClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltDarknessClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltDarknessClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		void		BltBrightness(WORD *pDest, WORD pitch, BYTE DarkBits);		
		void		BltBrightnessClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltBrightnessClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltBrightnessClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);
		void		BltBrightnessClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits);

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
		void		BltColorSet(WORD *pDest, WORD pitch, WORD colorSet);		
		void		BltColorSetClipLeft(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet);
		void		BltColorSetClipRight(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet);
		void		BltColorSetClipWidth(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet);
		void		BltColorSetClipHeight(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet);

		//---------------------------------------------------------
		// Effect
		//---------------------------------------------------------
		void		BltEffect(WORD *pDest, WORD pitch);				
		void		BltEffectClipLeft(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltEffectClipRight(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltEffectClipWidth(WORD *pDest, WORD pitch, RECT* pRect);
		void		BltEffectClipHeight(WORD *pDest, WORD pitch, RECT* pRect);


		//---------------------------------------------------------
		// Blt Wave - Test - -;;
		//---------------------------------------------------------
		//void		BltWave(WORD *pDest, WORD pitch);
		
	public :
		//--------------------------------------------------------
		// ColorSet Table
		//--------------------------------------------------------
		// 25 Set ,  13 Gradation
		static WORD		ColorSet[MAX_COLORSET][MAX_COLORGRADATION];			
		static WORD		GradationValue[MAX_COLORGRADATION];					
		static WORD		ColorSetDarkness[MAX_DARKBIT][MAX_COLORSET][MAX_COLORGRADATION];	
		static BYTE		ColorToGradation[MAX_COLOR_TO_GRADATION];			

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
	
		
		static int		s_IndexValue[MAX_COLORSET_USE];

	
	public :		
		typedef void (*FUNCTION_MEMCPYEFFECT)(WORD*, WORD*, WORD);

		
		enum FUNCTION_EFFECT
		{
			EFFECT_DARKER = 0,
			EFFECT_GRAY_SCALE,
			EFFECT_LIGHTEN,
			EFFECT_DARKEN,
			EFFECT_COLOR_DODGE,
			EFFECT_SCREEN,
			EFFECT_DODGE_BURN,
			EFFECT_DIFFERENT,
			EFFECT_GRADATION,
			EFFECT_SIMPLE_OUTLINE,
			EFFECT_WIPE_OUT,
			EFFECT_NET,
			EFFECT_GRAY_SCALE_VARIOUS,
			MAX_EFFECT
		};

		static void		SetEffect(enum FUNCTION_EFFECT func)	{ s_pMemcpyEffectFunction = s_pMemcpyEffectFunctionTable[func]; }
		static void		memcpyEffect(WORD* pDest, WORD* pSource, WORD pixels)
		{
			(*s_pMemcpyEffectFunction)(pDest, pSource, pixels);
		}

		// memcpyEffect
		static void		memcpyEffectDarker(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpyEffectGrayScale(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpyEffectLighten(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpyEffectDarken(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpyEffectColorDodge(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpyEffectScreen(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpyEffectDodgeBurn(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpyEffectDifferent(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpyEffectGradation(WORD* pDest, WORD* pSource, WORD pixels);		
		static void		memcpyEffectSimpleOutline(WORD* pDest, WORD* pSource, WORD pixels);	
		static void		memcpyEffectWipeOut(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpyEffectNet(WORD* pDest, WORD* pSource, WORD pixels);
		static void		memcpyEffectGrayScaleVarious(WORD* pDest, WORD* pSource, WORD pixels);


	protected :
		static FUNCTION_MEMCPYEFFECT		s_pMemcpyEffectFunction;
		static FUNCTION_MEMCPYEFFECT		s_pMemcpyEffectFunctionTable[MAX_EFFECT];
	
};

#endif



#include <fstream>
#ifndef __CALPHASPRITEPAL_H__
#define __CALPHASPRITEPAL_H__

#ifdef PLATFORM_WINDOWS
	#include <windows.h>
#else
	#include "../basic/Platform.h"
#endif
#include "CTypePack.h"
#include "CSpritePalBase.h"

class CAlphaSpritePal : public CSpritePalBase
{
public:
	// Constructor and destructor (for vtable)
	CAlphaSpritePal();
	virtual ~CAlphaSpritePal();

	void SetPixel(BYTE *pSource, WORD pitch, BYTE *pSourceAlpha, WORD alphaPitch, WORD width, WORD height);
	
	//---------------------------------------------------------
	// Blt functions
	//---------------------------------------------------------
	void		Blt(int x, int y, WORD* pDest, int pitch, MPalette &pal);

	//---------------------------------------------------------
	// �������� Blt
	//---------------------------------------------------------
	void		Blt(WORD *pDest, WORD pitch, MPalette &pal);				
	void		BltClip(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal);
	void		BltClipLeft(WORD *pDest, WORD pitch, RECT* pRect, MPalette &pal);
	void		BltClipRight(WORD *pDest, WORD pitch, RECT* pRect, MPalette &pal);
	void		BltClipWidth(WORD *pDest, WORD pitch, RECT* pRect, MPalette &pal);
	void		BltClipHeight(WORD *pDest, WORD pitch, RECT* pRect, MPalette &pal);
	
	//---------------------------------------------------------
	// Phase 4B: Blt4444* family deleted — only callers lived in
	// the orphaned CSpriteSurface.cpp (removed in 4B.1).
	//---------------------------------------------------------

	//---------------------------------------------------------
	// Alpha Blending
	//---------------------------------------------------------
	void		BltAlpha(WORD *pDest, WORD pitch, BYTE alpha, MPalette &pal);
	void		BltAlphaClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha, MPalette &pal);
	void		BltAlphaClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha, MPalette &pal);
	void		BltAlphaClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha, MPalette &pal);
	void		BltAlphaClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha, MPalette &pal);
	
	//---------------------------------------------------------
	// Utility Functions
	//---------------------------------------------------------
	void		memcpyAlphaValue(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal);
	void		memcpyAlpha(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal);
	
	bool		IsColorPixel(short x, short y );
	WORD		GetPixel( short x, short y , MPalette &pal);
	
protected:
	// Blt Value
	static int		s_Value1;
	static int		s_Value2;
};

typedef CTypePack<CAlphaSpritePal>	CAlphaSpritePalPack;

#endif
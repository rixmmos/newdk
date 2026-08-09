#include <fstream>
#include <cstdint>
//----------------------------------------------------------------------
// CShadowSpritePack.h
//----------------------------------------------------------------------



//
//----------------------------------------------------------------------

//

//


//

//
//----------------------------------------------------------------------
// 

//
// CShadowSpritePack sp;
//
// sp.Init( 3 );
//
// sp[0].SetPixel( ... );
// sp[1].SetPixel( ... );
// sp[2].SetPixel( ... );
//
// Surface->CopyShadowSprite( sp[1], ... )
//
//----------------------------------------------------------------------
//


//
//----------------------------------------------------------------------

#ifndef	__CSHADOWSPRITEPACK_H__
#define	__CSHADOWSPRITEPACK_H__

#include "DrawTypeDef.h"
#include "CShadowSprite.h"
#include "CIndexSpritePack.h"
#include <list>

class CShadowSpritePack {
	public :
		CShadowSpritePack();
		~CShadowSpritePack();

		//--------------------------------------------------------
		// Init/Release
		//--------------------------------------------------------
		void			Init(TYPE_SPRITEID count);				
		void			Release();
		void			ReleasePart(TYPE_SPRITEID firstShadowSpriteID, TYPE_SPRITEID lastShadowSpriteID);
		void			ReleaseLoaded();

		//--------------------------------------------------------
		// file I/O
		//--------------------------------------------------------
		bool			SaveToFile(std::ofstream& spkFile, std::ofstream& indexFile);
		bool			SaveToFileSpriteOnly(std::ofstream& spkFile, int32_t &filePosition);
		void			LoadFromFile(std::ifstream& file);		
		void			LoadFromFilePart(std::ifstream& file, int32_t filePosition,
										 TYPE_SPRITEID firstShadowSpriteID, TYPE_SPRITEID lastShadowSpriteID);
		void			LoadFromFilePart(std::ifstream& spkFile, const CSpriteFilePositionArray& fpArray);

		//--------------------------------------------------------
		// Convert
		//--------------------------------------------------------
		void			InitPart(CIndexSpritePack& ISPK, TYPE_SPRITEID firstShadowSpriteID, TYPE_SPRITEID lastShadowSpriteID);

		//--------------------------------------------------------
		// size
		//--------------------------------------------------------
		TYPE_SPRITEID	GetSize() const		{ return m_nSprites; }

		//--------------------------------------------------------
		// operator
		//--------------------------------------------------------
		CShadowSprite&		operator [] (TYPE_SPRITEID n) { return m_pSprites[n]; }

	protected :
		TYPE_SPRITEID		m_nSprites;		
		CShadowSprite*		m_pSprites;		

		INT_LIST		m_listLoad;		
		
};

#endif



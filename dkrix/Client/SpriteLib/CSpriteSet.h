#include <fstream>
//----------------------------------------------------------------------
// CSpriteSet.h
//----------------------------------------------------------------------
//

//


//


//
//----------------------------------------------------------------------

#ifndef	__CSPRITESET_H__
#define	__CSPRITESET_H__


#include "DrawTypeDef.h"
#include "CSpritePack.h"


class CSpriteSet {
	public :
		CSpriteSet();
		~CSpriteSet();

		//--------------------------------------------------------
		// Init/Release
		//--------------------------------------------------------
		void		Init(TYPE_SPRITEID count);		
		void		Release();

		//--------------------------------------------------------
		// file I/O		
		//--------------------------------------------------------
		
		// spkFile = SpritePack File, indexFile = FilePointer File
		bool		LoadFromFile(std::ifstream& indexFile, std::ifstream& packFile);
		

		//--------------------------------------------------------
		// operator
		//--------------------------------------------------------
		CSprite&	operator [] (TYPE_SPRITEID n) { return m_pSprites[n]; }


	protected :
		TYPE_SPRITEID		m_nSprites;			
		CSprite*			m_pSprites;			
};

#endif


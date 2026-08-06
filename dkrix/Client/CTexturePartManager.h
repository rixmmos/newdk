//----------------------------------------------------------------------
// CTexturePartManager.h
//----------------------------------------------------------------------



//----------------------------------------------------------------------

#ifndef	__CTEXTUREPARTMANAGER_H__
#define	__CTEXTUREPARTMANAGER_H__

#include "CPartManager.h"
#include "SpriteLib/CSpriteSurface.h"
#include <list>
#include <map>
//std::ifstream;
//class CFileIndexTable;
//class CAlphaSpritePack;
#include "SpriteLib/CAlphaSpritePal.h"
#include "SpriteLib/MPalettePack.h"

class CTexturePartManager : public CPartManager<WORD, WORD, CSpriteSurface*>
{
	public :
		typedef std::list<CSpriteSurface*>	SPRITESURFACE_LIST;
		typedef std::map<int, int>			SPRITE_PALETTE;			

	public :
		//CTexturePartManager(CAlphaSpritePack* pASPK, WORD partSize=100);
		CTexturePartManager(const char* aspkFilename, WORD partSize=100);
		~CTexturePartManager();

		//---------------------------------------------------
		// Init / Release
		//---------------------------------------------------
		//void				Init(CAlphaSpritePack* pASPK, WORD partSize);
		void				Init(const char* aspkFilename, WORD partSize);
		void				Release();
	
		
		void				Clear();

		//---------------------------------------------------
		
		//---------------------------------------------------
		CSpriteSurface*		GetTexture(TYPE_SPRITEID id, int index);

		
		int					GetWidth(TYPE_SPRITEID id)	{ return m_pWidth[id]; }
		int					GetHeight(TYPE_SPRITEID id)	{ return m_pHeight[id]; }

		int					GetSpriteWidth(TYPE_SPRITEID id)	{ return m_ASPK[id].GetWidth(); }
		int					GetSpriteHeight(TYPE_SPRITEID id)	{ return m_ASPK[id].GetHeight(); }

		//---------------------------------------------------
		
		//---------------------------------------------------
		void				DeleteRemoved();
		int					GetUsedPalette(int spriteID);

	protected :

//		std::ifstream		m_ASPKFile;		// ASPK File

		CAlphaSpritePalPack	m_ASPK;			// ASPK

		//CAlphaSpritePack	*m_pASPK;			// ASPK
		
		
		int					*m_pWidth;
		int					*m_pHeight;

		
		SPRITESURFACE_LIST	m_listRemoved;
		SPRITE_PALETTE		m_historySprPal;
		MPalettePack		m_EffectAlphaPPK;
};


#endif


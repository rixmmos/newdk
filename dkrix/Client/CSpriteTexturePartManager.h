//----------------------------------------------------------------------
// CSpriteTexturePartManager.h
//----------------------------------------------------------------------



//----------------------------------------------------------------------

#ifndef	__CSPRITETEXTUREPARTMANAGER_H__
#define	__CSPRITETEXTUREPARTMANAGER_H__

#include "CPartManager.h"
#include "SpriteLib/CSpriteSurface.h"
#include <list>
#include <map>
//std::ifstream;
//class CFileIndexTable;
#include "SpriteLib/CSpritePack.h"
#include "SpriteLib/CSpritePal.h"
#include "SpriteLib/MPalettePack.h"

class CSpriteTexturePartManager : public CPartManager<WORD, WORD, CSpriteSurface*>
{
	public :
		typedef std::list<CSpriteSurface*>	SPRITESURFACE_LIST;
		typedef std::map<int, int>			SPRITE_PALETTE;			

	public :
		//CTexturePartManager(CAlphaSpritePack* pSPK, WORD partSize=100);
		CSpriteTexturePartManager(const char* spkFilename, WORD partSize=100);
		~CSpriteTexturePartManager();

		//---------------------------------------------------
		// Init / Release
		//---------------------------------------------------
		//void				Init(CAlphaSpritePack* pSPK, WORD partSize);
		void				Init(const char* spkFilename, WORD partSize);
		void				Release();
	
		
		void				Clear();

		//---------------------------------------------------
		
		//---------------------------------------------------
		CSpriteSurface*		GetTexture(TYPE_SPRITEID id, int index);

		
		int					GetWidth(TYPE_SPRITEID id)	{ return m_pWidth[id]; }
		int					GetHeight(TYPE_SPRITEID id)	{ return m_pHeight[id]; }

		int					GetSpriteWidth(TYPE_SPRITEID id)	{ return m_SPK[id].GetWidth(); }
		int					GetSpriteHeight(TYPE_SPRITEID id)	{ return m_SPK[id].GetHeight(); }

		//---------------------------------------------------
		
		//---------------------------------------------------
		void				DeleteRemoved();

		int					GetUsedPalette(int spriteID);

	protected :

//		std::ifstream		m_SPKFile;		// SPK File

		CSpritePalPack			m_SPK;			// SPK

		//CAlphaSpritePack	*m_pSPK;			// SPK
		
		
		int					*m_pWidth;
		int					*m_pHeight;

		
		SPRITESURFACE_LIST	m_listRemoved;
		SPRITE_PALETTE		m_historySprPal;
		MPalettePack		m_EffectScreenPPK;
};

class CNormalSpriteTexturePartManager : public CPartManager<WORD, WORD, CSpriteSurface*>
{
	public :
		typedef std::list<CSpriteSurface*>	SPRITESURFACE_LIST;
		typedef std::map<int, int>			SPRITE_PALETTE;			

	public :
		CNormalSpriteTexturePartManager(CSpritePack* pSPK, WORD partSize=100);
//		CNormalSpriteTexturePartManager(const char* spkFilename, WORD partSize=100);
		~CNormalSpriteTexturePartManager();

		//---------------------------------------------------
		// Init / Release
		//---------------------------------------------------
		//void				Init(CAlphaSpritePack* pSPK, WORD partSize);
		void				Init(CSpritePack *pSPK, WORD partSize);
		void				Release();
	
		
		void				Clear();

		//---------------------------------------------------
		
		//---------------------------------------------------
		CSpriteSurface*		GetTexture(TYPE_SPRITEID id);

		
		int					GetWidth(TYPE_SPRITEID id)	{ return m_pWidth[id]; }
		int					GetHeight(TYPE_SPRITEID id)	{ return m_pHeight[id]; }

		int					GetSpriteWidth(TYPE_SPRITEID id)	{ return (*m_pSPK)[id].GetWidth(); }
		int					GetSpriteHeight(TYPE_SPRITEID id)	{ return (*m_pSPK)[id].GetHeight(); }

		//---------------------------------------------------
		
		//---------------------------------------------------
		void				DeleteRemoved();

	protected :

//		std::ifstream		m_SPKFile;		// SPK File

		CSpritePack			*m_pSPK;			// SPK
		
		
		int					*m_pWidth;
		int					*m_pHeight;

		
		SPRITESURFACE_LIST	m_listRemoved;		
};


#endif


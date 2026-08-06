//----------------------------------------------------------------------
// CShadowPartManager.h
//----------------------------------------------------------------------



//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------

#ifndef	__CSHADOWPARTMANAGER_H__
#define	__CSHADOWPARTMANAGER_H__

#include "CPartManager.h"
#include "SpriteLib/CSpriteSurface.h"
#include "DrawTypeDef.h"
#include <list>
#include <fstream>
#include "SpriteLib/CShadowSprite.h"
class CFileIndexTable;

using namespace std;


class CShadowPartManager : public CPartManager<WORD, WORD, CSpriteSurface*>
{
	public :
		typedef std::list<CSpriteSurface*>	SPRITESURFACE_LIST;

	public :
		//CShadowPartManager(CShadowSpritePack* pSSPK, WORD partSize=100);
		CShadowPartManager(const char* SSPKFilename, WORD partSize=100);
		~CShadowPartManager();

		//---------------------------------------------------
		// Init / Release
		//---------------------------------------------------
		//void				Init(CShadowSpritePack* pSSPK, WORD partSize);
		void				Init(const char* SSPKFilename, WORD partSize);
		void				Release();
	
		
		void				Clear();

		//---------------------------------------------------
		
		//---------------------------------------------------
		CSpriteSurface*		GetTexture(TYPE_SPRITEID id);

		
		int					GetWidth(TYPE_SPRITEID id)	{ return m_pWidth[id]; }
		int					GetHeight(TYPE_SPRITEID id)	{ return m_pHeight[id]; }

		
		CShadowSprite&		GetSprite(TYPE_SPRITEID id);

		//---------------------------------------------------
		
		//---------------------------------------------------
		void				DeleteRemoved();


	protected :

//		std::ifstream		m_SSPKFile;		// SSPK File

		CShadowSpritePack	m_SSPK;			// SSPK

		//CShadowSpritePack	*m_pSSPK;			// SSPK
		

		
		int					*m_pWidth;
		int					*m_pHeight;

		
		SPRITESURFACE_LIST	m_listRemoved;
};


#endif


//----------------------------------------------------------------------
// CSpriteTexturePartManager.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
// DX3D.h and CDirect3D.h removed (SDL2) - Direct3D has been replaced with SDL2
#include "CFileIndexTable.h"
#include "CSpriteTexturePartManager.h"

#include "DebugInfo.h"
//#include "MFileDef.h"
#include "Packet/Properties.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

#ifdef OUTPUT_DEBUG
	#undef OUTPUT_DEBUG
#endif

//----------------------------------------------------------------------
//
// contructor/destructor
//
//----------------------------------------------------------------------
//CSpriteTexturePartManager::CSpriteTexturePartManager(CAlphaSpritePack* pSPK, WORD partSize)
CSpriteTexturePartManager::CSpriteTexturePartManager(const char* spkFilename, WORD partSize)
{
	//m_pSPK = NULL;
	m_pWidth = NULL;
	m_pHeight = NULL;

//	m_bOpen = false;
	Init(spkFilename, partSize);	

	m_historySprPal.clear();

	//Init(pSPK, partSize);	
}

CSpriteTexturePartManager::~CSpriteTexturePartManager()
{
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Init(spkFilename, indexFilename)
//----------------------------------------------------------------------


//


//----------------------------------------------------------------------
void
//CSpriteTexturePartManager::Init(CAlphaSpritePack* pSPK, WORD partSize)
CSpriteTexturePartManager::Init(const char* spkFilename, WORD partSize)
{
	Release();

	///*
	
//	std::ifstream indexFile(indexFilename, ios::binary);
//	m_SPKIndex.LoadFromFile( indexFile );
//	indexFile.close();
//

//

//	m_SPKFile.open(spkFilename, ios::binary);
//	
//	TYPE_SPRITEID size;
//	m_SPKFile.read((char*)&size, SIZE_SPRITEID);
//
	m_SPK.LoadFromFileRunning(spkFilename);

	int allSize = m_SPK.GetSize(); 
	//*/


	//m_pSPK = pSPK;

	//int allSize = pSPK->GetSize();

	
	
	CPartManager<WORD, WORD, CSpriteSurface*>::Init( allSize, partSize );
	
	
	for (int i=0; i<m_nPart; i++)
	{
		m_pData[i] = NULL;
	}

	
	m_pWidth = new int [allSize];
	m_pHeight = new int [allSize];

	for (int i=0; i<allSize; i++)
	{
		m_pWidth[i] = 0;
		m_pHeight[i] = 0;
	}

	 

// 	m_bOpen = true;
	m_EffectScreenPPK.LoadFromFile( g_pFileDef->getProperty("FILE_PALETTE_SCREENEFFECT").c_str() );
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
CSpriteTexturePartManager::Release()
{
	///*
//	if (m_bOpen)
//	{
//		m_SPKFile.close();
//		m_bOpen = false;
//	}
	//*/	
	if (m_pData!=NULL)
	{
		for (int i=0; i<m_nPart; i++)
		{
			if (m_pData[i]!=NULL)
			{
				delete m_pData[i];
				m_pData[i] = NULL;
			}
		}
	}

	if (m_pWidth != NULL)
	{
		delete [] m_pWidth;
		m_pWidth = NULL;
	}

	if (m_pHeight != NULL)
	{
		delete [] m_pHeight;
		m_pHeight = NULL;
	}

	DeleteRemoved();
}

//----------------------------------------------------------------------
// Delete Removed
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void				
CSpriteTexturePartManager::DeleteRemoved()
{
	SPRITESURFACE_LIST::iterator iSurface = m_listRemoved.begin();

	while (iSurface != m_listRemoved.end())
	{
		CSpriteSurface*	pSurface = *iSurface;

		delete pSurface;
		
		iSurface++;
	}

	m_listRemoved.clear();
}

//----------------------------------------------------------------------
// Clear
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSpriteTexturePartManager::Clear()
{	
	DeleteRemoved();

	
	for (int i=0; i<m_nPart; i++)
	{
		if (m_pData[i] != NULL)			
		{
			m_pData[i]->Restore();
			delete m_pData[i];
			m_pData[i] = NULL;
		}
	}

	
	
	CPartManager<WORD, WORD, CSpriteSurface*>::Init( m_nIndex, m_nPart );

	
	for (int i=0; i<m_nPart; i++)
	{
		m_pData[i] = NULL;
	}

}

//----------------------------------------------------------------------
// GetTexture( id )
//----------------------------------------------------------------------

//




//

//----------------------------------------------------------------------
CSpriteSurface*		
CSpriteTexturePartManager::GetTexture(TYPE_SPRITEID id, int index)
{
	//static int reload	= 0;
	//static int reuse	= 0;

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (IsDataNotNULL(id) && GetUsedPalette( id ) == index)
	{
#ifdef OUTPUT_DEBUG
		if (g_pDebugMessage)
		{
			DEBUG_ADD_FORMAT("CSpriteTexturePartManager::GetTexture(%d)", id);
		}
#endif
		
		CSpriteSurface* pTextureSurface;
		m_historySprPal[id] = index;
		if (GetData(id, pTextureSurface))
		{
			return pTextureSurface;
		}
		
#ifdef OUTPUT_DEBUG
		if (g_pDebugMessage)
		{
			DEBUG_ADD("No Exist!");
		}
#endif
		
		//reuse ++;
	}
	else
	{
		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD_FORMAT("CSpriteTexturePartManager::GetTexture New(%d)", id);
			}
		#endif

		CSpritePal* pSprite = &m_SPK[id];
		//CAlphaSprite* pSprite = &(*m_pSPK)[id];

		
		///*
		//----------------------------------------
		
		//----------------------------------------
//		if (pSprite->IsInit())
//		{
//		}
//		//----------------------------------------

//		//----------------------------------------
//		else
//		{
//			//-----------------------------------------------------------

//			//-----------------------------------------------------------

//			int temp = m_SPKIndex[id];
//			m_SPKFile.seekg(m_SPKIndex[id], ios::beg);
//			pSprite->LoadFromFile( m_SPKFile );
//		}
		//*/
		
		int spWidth, spHeight;
		int width, height;
		BYTE smallShift = 0;

		spWidth = pSprite->GetWidth();
		spHeight = pSprite->GetHeight();

		//-----------------------------------------------------
		
		//-----------------------------------------------------
		if (spWidth==0 || spHeight==0)
		{
			return NULL;
		}

		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		CSpriteSurface* pTextureSurface = new CSpriteSurface;
	

		width = spWidth;
		height = spHeight;

		bool bDifferentSize = false;

		//---------------------------------------------------
		
		//---------------------------------------------------		
		pTextureSurface->InitTextureSurface(width, height, 0, nullptr);

		POINT point;
		point.x = 0;
		point.y = 0;
		
		if (!pTextureSurface->Lock())
		{
			delete pTextureSurface;
			return NULL;
		}

		int i;

		//---------------------------------------------------
		
		//---------------------------------------------------
		//if (bDifferentSize)
		{
			//---------------------------------------------------
			
			//---------------------------------------------------
			WORD *pSurface = (WORD*)pTextureSurface->GetSurfacePointer();
					//,	*pSurfaceTemp;
			long pitch	= pTextureSurface->GetSurfacePitch();

			int width2 = width << 1;

			for (int i=0; i<height; i++)
			{
				memset(pSurface, 0, width2);
				pSurface = (WORD*)((BYTE*)pSurface + pitch);
			}
			
			 
		}

		//---------------------------------------------------
		
		//---------------------------------------------------
		
		if (smallShift==0)
		{
			//pTextureSurface->BltAlphaSprite4444NotTrans(&point, pSprite);
			pTextureSurface->BltSpritePal1555NotTrans(&point, pSprite, m_EffectScreenPPK[index]);
		}
		else
		{
			pTextureSurface->BltSpritePal1555SmallNotTrans(&point, pSprite, smallShift, m_EffectScreenPPK[index]);
		}
		
		pTextureSurface->Unlock();


		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		if (m_pWidth[id]==0 || m_pHeight[id]==0)
		{
			spWidth = pSprite->GetWidth();
			spHeight = pSprite->GetHeight();
			
			if (false)
			{	
				
				for (int i=0; i<smallShift; i++)
				{
					spWidth >>= 1;
					spHeight >>= 1;
				}

				
				width = width * pSprite->GetWidth() / spWidth;
				height = height * pSprite->GetHeight() / spHeight;		
			}
			else
			{
				width = spWidth;
				height = spHeight;

				
				if (false)
				{				
					if (width > height)
					{
						height = width;				
					}
					else if (width < height)
					{
						width = height;				
					}	
				}
			}			

			m_pWidth[id] = width;
			m_pHeight[id] = height;		
		}
		
		//---------------------------------------------------
		
		//---------------------------------------------------
		CSpriteSurface* pOld = NULL;
		WORD oldIndex = SetData( id, pTextureSurface, pOld );
		m_historySprPal[id] = index;
		
		if (oldIndex!=m_IndexNULL)
		{
			if (pOld != NULL)
			{
				//------------------------------------------------------------
				
				
				
				//------------------------------------------------------------
				//delete pOld;
				//------------------------------------------------------------
				m_listRemoved.push_back( pOld );
			}

			
			//m_SPK[oldIndex].Release();
		}

		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("OK");
			}
		#endif

		// return
		return pTextureSurface;

		//reload++;
	}
	
	return NULL;
}

int		CSpriteTexturePartManager::GetUsedPalette(int spriteID)
{
	if( m_historySprPal.empty() )
		return  -1;

	SPRITE_PALETTE::iterator itr;
	if ( (itr = m_historySprPal.find( spriteID )) != m_historySprPal.end() )
		return itr->second;

	return -1;
}

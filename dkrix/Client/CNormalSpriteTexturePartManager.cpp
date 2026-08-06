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

//----------------------------------------------------------------------
//
// contructor/destructor
//
//----------------------------------------------------------------------
//CNormalSpriteTexturePartManager::CSpriteTexturePartManager(CAlphaSpritePack* pSPK, WORD partSize)
CNormalSpriteTexturePartManager::CNormalSpriteTexturePartManager(CSpritePack* pSPK, WORD partSize)
{
	m_pSPK = NULL;
	m_pWidth = NULL;
	m_pHeight = NULL;

//	m_bOpen = false;
	Init(pSPK, partSize);	

	//Init(pSPK, partSize);	
}

CNormalSpriteTexturePartManager::~CNormalSpriteTexturePartManager()
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
CNormalSpriteTexturePartManager::Init(CSpritePack* pSPK, WORD partSize)
//CNormalSpriteTexturePartManager::Init(const char* spkFilename, WORD partSize)
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
//	m_SPK.LoadFromFileRunning(spkFilename);
	m_pSPK = pSPK;
	int allSize = m_pSPK->GetSize(); 
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
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
CNormalSpriteTexturePartManager::Release()
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
CNormalSpriteTexturePartManager::DeleteRemoved()
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
CNormalSpriteTexturePartManager::Clear()
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
CNormalSpriteTexturePartManager::GetTexture(TYPE_SPRITEID id)
{
	//static int reload	= 0;
	//static int reuse	= 0;

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (IsDataNotNULL(id))
	{
#ifdef OUTPUT_DEBUG
		if (g_pDebugMessage)
		{
			DEBUG_ADD_FORMAT("CNormalSpriteTexturePartManager::GetTexture(%d)", id);
		}
#endif
		
		CSpriteSurface* pTextureSurface;
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
				DEBUG_ADD_FORMAT("CNormalSpriteTexturePartManager::GetTexture New(%d)", id);
			}
		#endif

		CSprite* pSprite = &(*m_pSPK)[id];
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
			pTextureSurface->BltSprite1555NotTrans(&point, pSprite);
		}
		else
		{
			pTextureSurface->BltSprite1555SmallNotTrans(&point, pSprite, smallShift);
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

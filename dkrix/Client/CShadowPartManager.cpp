//----------------------------------------------------------------------
// CShadowPartManager.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
// DX3D.h and CDirect3D.h removed (SDL2) - Direct3D has been replaced with SDL2
#include "DXLib/CDirectDraw.h"  // For LPDDPIXELFORMAT
#include "CFileIndexTable.h"
#include "CShadowPartManager.h"
#include "MGameTime.h"
#include "MWeather.h"
#include <fstream>

#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

extern bool FileOpenBinary(const char* filename, ifstream& fil);

extern bool		g_bZonePlayerInLarge;



#define	MAX_SIZE_TEXTURE_SHADOW		128


#ifdef OUTPUT_DEBUG
	#undef OUTPUT_DEBUG
#endif


//----------------------------------------------------------------------
//
// contructor/destructor
//
//----------------------------------------------------------------------
//CShadowPartManager::CShadowPartManager(CShadowSpritePack* pSSPK, WORD partSize)
CShadowPartManager::CShadowPartManager(const char* SSPKFilename, WORD partSize)
{
	//m_pSSPK		= NULL;
	m_pWidth	= NULL;
	m_pHeight	= NULL;

//	m_bOpen = false;
//	Init(SSPKFilename, indexFilename, partSize);	
	Init(SSPKFilename, partSize);	

	//Init(pSSPK, partSize);
}

CShadowPartManager::~CShadowPartManager()
{
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Init(SSPKFilename, indexFilename)
//----------------------------------------------------------------------


//


//----------------------------------------------------------------------
void
//CShadowPartManager::Init(CShadowSpritePack* pSSPK, WORD partSize)
CShadowPartManager::Init(const char* SSPKFilename, WORD partSize)
{
	Release();
	///*
	
	std::ifstream indexFile;//(indexFilename, ios::binary);
//	if (!FileOpenBinary(indexFilename, indexFile))
//		return;
//
//	m_SSPKIndex.LoadFromFile( indexFile );
//	indexFile.close();
//

//	m_SSPK.Init( m_SSPKIndex.GetSize() );
//

//	m_SSPKFile.open(SSPKFilename, ios::binary);
//	
//	TYPE_SPRITEID size;
//	m_SSPKFile.read((char*)&size, SIZE_SPRITEID);
	m_SSPK.LoadFromFileRunning(SSPKFilename);

	int allSize = m_SSPK.GetSize();
	//*/

	//m_pSSPK = pSSPK;

	//int allSize = pSSPK->GetSize();

	
	
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

	 

//	m_bOpen = true;
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
CShadowPartManager::Release()
{
	///*
//	if (m_bOpen)
//	{
//		m_SSPKFile.close();
//		m_bOpen = false;
//	}
	//*/	
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

	DeleteRemoved();
}

//----------------------------------------------------------------------
// Delete Removed
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void				
CShadowPartManager::DeleteRemoved()
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
CShadowPartManager::Clear()
{
	
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

	DeleteRemoved();
}

//----------------------------------------------------------------------
// GetTexture( id )
//----------------------------------------------------------------------

//




//

//----------------------------------------------------------------------
CSpriteSurface*		
CShadowPartManager::GetTexture(TYPE_SPRITEID id)
{
	//static int reload	= 0;
	//static int reuse	= 0;

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (IsDataNULL(id))
	{
		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD_FORMAT("CShadowPartManager::GetTexture New(%d)", id);
			}
		#endif	

		CShadowSprite* pSprite = &m_SSPK[id];
		//CShadowSprite* pSprite = &(*m_pSSPK)[id];


		
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

//			int temp = m_SSPKIndex[id];
//			m_SSPKFile.seekg(m_SSPKIndex[id], ios::beg);
//			pSprite->LoadFromFile( m_SSPKFile );
//		}
		//*/
		
		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("Loading ShadowSprite OK");
			}
		#endif	

		
		
		
		
		int spWidth, spHeight;
		int width, height;


		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("Determine Texture GetSize OK");
			}
		#endif	

		spWidth = pSprite->GetWidth();
		spHeight = pSprite->GetHeight();
		
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		if (spWidth==0 || spHeight==0)
		{
			return NULL;
		}

		width = spWidth;
		height = spHeight;


		int shift = 0;
		
		while (width>MAX_SIZE_TEXTURE_SHADOW || height>MAX_SIZE_TEXTURE_SHADOW)
		{
			width >>= 1;
			height >>= 1;
			shift ++;
		}

		bool bDifferentSize = false;

		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("Determine Texture Size OK");
			}
		#endif

		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		CSpriteSurface* pTextureSurface = new CSpriteSurface;

		if (pTextureSurface==NULL)
		{
			#ifdef OUTPUT_DEBUG
				if (g_pDebugMessage)
				{
					DEBUG_ADD("[Error] Texture Surface is NULL");
				}
			#endif

			return NULL;
		}
		
		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("New Texture Surface OK");
			}
		#endif
		

		//---------------------------------------------------
		
		//---------------------------------------------------	
		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("Before Get Pixel Format");
			}
		#endif
		const LPDDPIXELFORMAT pixel4444 = nullptr;

		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("Before InitTexture Surface");
			}
		#endif

		pTextureSurface->InitTextureSurface(width, height, 0, pixel4444);
		POINT point;
		point.x = 0;
		point.y = 0;

		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD_FORMAT("Init Texture OK. num=%d, size=(%d, %d)", m_nPartUsed, pTextureSurface->GetWidth(), pTextureSurface->GetHeight());
			}
		#endif

		if (!pTextureSurface->Lock())
		{
			delete pTextureSurface;
			return NULL;
		}
			/*
		DDSURFACEDESC2 ddsd;

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);

		while (1)
		{
			HRESULT hr = pTextureSurface->GetSurface()->Lock(NULL, &ddsd,  DDLOCK_WAIT, NULL);

			if (hr==DD_OK)
			{
				break;
			}

			if( hr == DDERR_SURFACELOST )
			{
				bool ret = pTextureSurface->Restore();
				if (ret == false)
					break;
				else
					continue;
			}

			if( hr != DDERR_WASSTILLDRAWING )
			{
				#ifdef OUTPUT_DEBUG
					if (g_pDebugMessage)
					{
						DEBUG_ADD("Lock Failed");
					}
				#endif
				
				return false;
			}			
		}
		*/
		

		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("Lock OK");
			}
		#endif

		int i;

		//---------------------------------------------------
		
		//---------------------------------------------------
		//static WORD trans[1024] = { 0, };

		WORD *pSurface = (WORD*)pTextureSurface->GetSurfacePointer();
				//,	*pSurfaceTemp;
		long pitch	= pTextureSurface->GetSurfacePitch();

		
		//if (bDifferentSize)
		{
			int width2 = width << 1;
			i = pTextureSurface->GetHeight();

			if (i>0)
			{
				do
				{			
					memset(pSurface, 0, width2);
					//memcpy(pSurface, trans, width);
					pSurface = (WORD*)((BYTE*)pSurface + pitch);
				} while (--i);
			}
		}
	
		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("Make Texture OK");
			}
		#endif
		
		//---------------------------------------------------
		
		//---------------------------------------------------
		const int timeShadow[] =
		{
			0x4000,		
			0x4000,		
			0x4000,		
			0x4000,		
			0x4000,		
			0x4000,		
			0x4000,		
			0x5000,		
			0x6000,		
			0x7000,		
			0x8000,		
			0x9000,		
			0xA000,		
			0xB000,		
			0xC000,		
			0xB000,		
			0xA000,		
			0x9000,		
			0x8000,		
			0x7000,		
			0x6000,		
			0x5000,		
			0x4000,		
			0x4000		
		};

		WORD shadowColor = timeShadow[(*g_pGameTime).GetHour()];


		//---------------------------------------------------
		
		//---------------------------------------------------
		if (!g_bZonePlayerInLarge)
		{
			shadowColor = 0x8000;
		}
		else
		{
			//---------------------------------------------------
			
			//---------------------------------------------------
			if (g_pWeather->GetWeatherType()==MWeather::WEATHER_RAIN)
			{
				int test = (int)shadowColor - (int)0x4000;

				if (test < 0x3000)
				{
					test = 0x3000;
				}

				shadowColor = test;
			}
		}


		//---------------------------------------------------
		
		//---------------------------------------------------
		pTextureSurface->BltShadowSpriteSmall4444(&point, pSprite, shadowColor, shift);
		
		
	  
		pTextureSurface->Unlock();

		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("Blt Small4444 OK");
			}
		#endif
	
			
		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		
		//-----------------------------------------------------------

			// SDL Migration: IsTexturePow2() always returns false in SDL2
		if (m_pWidth[id]==0 || m_pHeight[id]==0)
		{
			spWidth = pSprite->GetWidth();
			spHeight = pSprite->GetHeight();

			if (false)
			{	
				
				for (int i=0; i<shift; i++)
				{
					spWidth >>= 1;
					spHeight >>= 1;
				}

				
				width = width * pSprite->GetWidth() / spWidth;
				height = height * pSprite->GetHeight() / spHeight;		

				// SDL Migration: IsTextureSquareOnly() always returns false in SDL2
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

		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD("Set Data OK");
			}
		#endif

		if (oldIndex != m_IndexNULL)
		{
			if (pOld != NULL)
			{
				#ifdef OUTPUT_DEBUG
					if (g_pDebugMessage)
					{
						DEBUG_ADD_FORMAT("Delete Old Surface(%d), (%d,%d)", oldIndex, m_SSPK[oldIndex].GetWidth(), m_SSPK[oldIndex].GetHeight());
					}
				#endif
				
				//------------------------------------------------------------
				
				
				
				//------------------------------------------------------------
				//delete pOld;
				//------------------------------------------------------------
				m_listRemoved.push_back( pOld );
			}

			
			
			//m_SSPK[oldIndex].Release();			
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
	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	else
	{
		#ifdef OUTPUT_DEBUG
			if (g_pDebugMessage)
			{
				DEBUG_ADD_FORMAT("CShadowPartManager::GetTexture(%d)", id);
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

	return NULL;
}


//----------------------------------------------------------------------
// Get Sprite
//----------------------------------------------------------------------
CShadowSprite&		
CShadowPartManager::GetSprite(TYPE_SPRITEID id)
{
	//------------------------------------------------------
	
	//------------------------------------------------------
//	if (m_SSPK[id].IsNotInit())
//	{
//		int temp = m_SSPKIndex[id];
//		m_SSPKFile.seekg(m_SSPKIndex[id], ios::beg);
//		m_SSPK[id].LoadFromFile( m_SSPKFile );
//	}
//
	return m_SSPK[id];
}
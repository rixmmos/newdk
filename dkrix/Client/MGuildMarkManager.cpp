//----------------------------------------------------------------------
// MGuildMarkManager.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MGuildMarkManager.h"
#include "MGuildInfoMapper.h"
#include "CSprite555.h"
#include "CSprite565.h"
#include "UtilityFunction.h"
#include "basic/ColorDraw.h"

#if defined(__GAME_CLIENT__) || defined(__GUILD_MANAGER_TOOL__)
	#include "UtilityFunction.h"
	#include "AppendPatchInfo.h"
	#include "DebugInfo.h"
#endif

//----------------------------------------------------------------------
// global
//----------------------------------------------------------------------
MGuildMarkManager* g_pGuildMarkManager = NULL;

extern bool g_bUseProgressBar;


//----------------------------------------------------------------------
//
//						MGuildMarkManager
//
//----------------------------------------------------------------------
MGuildMarkManager::MGuildMarkManager(const char* pGuildMarkSPKFilename, 
									const char* pGuildInfoMapperFilename,
									const char* pSlayerGradeMarkSPKFilename,
									const char* pVampireGradeMarkSPKFilename,
									const char* pOustersGradeMarkSPKFilename,
									const char* pLevelMarkSPKFilename)
{
	m_GuildMarkSPKFilename = pGuildMarkSPKFilename;

//	m_SlayerGradeMarkSPKFilename = pSlayerGradeMarkSPKFilename;
//	m_VampireGradeMarkSPKFilename = pVampireGradeMarkSPKFilename;

	
	if (pGuildInfoMapperFilename!=NULL)
	{
		m_GuildMarkSpriteMapperFilename = pGuildInfoMapperFilename;		
	}

//	m_GradeMarks.Init(GRADE_MARK_MAX<<2, ColorDraw::Is565());
	m_GradeMarks[RACE_SLAYER].LoadFromFileRunning(pSlayerGradeMarkSPKFilename);
	m_GradeMarks[RACE_VAMPIRE].LoadFromFileRunning(pVampireGradeMarkSPKFilename);
	m_GradeMarks[RACE_OUSTERS].LoadFromFileRunning(pOustersGradeMarkSPKFilename);
	m_LevelMarks.LoadFromFileRunning(pLevelMarkSPKFilename);
}

MGuildMarkManager::~MGuildMarkManager()
{
	Release();
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void		
MGuildMarkManager::Release()
{
	GUILDMARK_MAP::iterator iMark = m_GuildMarks.begin();

	while (iMark != m_GuildMarks.end())
	{
		CSprite* pSprite = iMark->second.pSprite;
		CSprite* pSpriteSmall = iMark->second.pSpriteSmall;

		if (pSprite!=NULL)
		{
			delete pSprite;
		}

		if (pSpriteSmall!=NULL)
		{
			delete pSpriteSmall;
		}

		iMark++;
	}
	m_GuildMarks.clear();
}

//----------------------------------------------------------------------
// Create GuildMark( guildID, pFilename )
//

//




//----------------------------------------------------------------------
bool
MGuildMarkManager::CreateGuildMark(WORD guildID, const char* pFilename)
{
#if defined(__GAME_CLIENT__) || defined(__GUILD_MANAGER_TOOL__)
#ifdef PLATFORM_WINDOWS
	if (pFilename==NULL)
	{
		return false;
	}

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	CDirectDrawSurface bmpSurface;
	if (!LoadImageToSurface(pFilename, bmpSurface))
	{
		return false;
	}

	const POINT bigSize = { 40, 40 };
	const POINT smallSize = { 20, 20 };

	CDirectDrawSurface surface;
	surface.InitOffsurface( bigSize.x, bigSize.y, DDSCAPS_SYSTEMMEMORY );

	RECT destBigRect = { 0, 0, bigSize.x, bigSize.y };
	RECT destSmallRect = { 0, 0, smallSize.x, smallSize.y };

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	CSprite* pSprite = NULL;
	CSprite* pSpriteSmall = NULL;

	if (ColorDraw::Is565())
	{
		pSprite = new CSprite565;
		pSpriteSmall = new CSprite565;
	}
	else
	{
		pSprite = new CSprite555;
		pSpriteSmall = new CSprite555;
	}

	//-----------------------------------------------------
	// surface --> CSprite
	//-----------------------------------------------------
	WORD* lpSurface;
	unsigned short pitch;

	RECT bmpRect = { 0, 0, bmpSurface.GetWidth(), bmpSurface.GetHeight() };

	
	CSprite::SetColorkey( 0x001F );

	// BigSize
	surface.FillSurface( 0 );
	surface.Blt(&destBigRect, &bmpSurface, &bmpRect);
	surface.LockW(lpSurface, pitch);
	pSprite->SetPixel(lpSurface, pitch, bigSize.x, bigSize.y);
	surface.Unlock();

	// SmallSize
	surface.FillSurface( 0 );
	surface.Blt(&destSmallRect, &bmpSurface, &bmpRect);
	surface.LockW(lpSurface, pitch);
	pSpriteSmall->SetPixel(lpSurface, pitch, smallSize.x, smallSize.y);
	surface.Unlock();

	//-----------------------------------------------------
	// add
	//-----------------------------------------------------
	AddGuildMark(guildID, pSprite, pSpriteSmall);
#endif // PLATFORM_WINDOWS
#endif
	return true;
}

//----------------------------------------------------------------------
// A GuildMark
//----------------------------------------------------------------------
bool		
MGuildMarkManager::AddGuildMark(WORD guildID, CSprite* pSprite, CSprite* pSpriteSmall)
{
	if (!SaveGuildMark(guildID, pSprite, pSpriteSmall))
	{
		
		//return false;
	}

	GUILDMARK_MAP::iterator iMark = m_GuildMarks.find(guildID);

	
	if (iMark != m_GuildMarks.end())
	{
		CSprite* pSprite = iMark->second.pSprite;
		CSprite* pSpriteSmall = iMark->second.pSpriteSmall;

		if (pSprite!=NULL)
		{
			delete pSprite;
		}

		if (pSpriteSmall!=NULL)
		{
			delete pSpriteSmall;
		}
	}

	m_GuildMarks[guildID] = GUILD_SPRITES( pSprite, pSpriteSmall );

	return true;
}

//----------------------------------------------------------------------
// Get GuildMark
//----------------------------------------------------------------------
CSprite*	
MGuildMarkManager::GetGuildMark(WORD guildID)
{
	GUILDMARK_MAP::iterator iMark = m_GuildMarks.find(guildID);

	if (iMark != m_GuildMarks.end())
	{
		return m_GuildMarks[guildID].pSprite;
	}

	return NULL;
}

//----------------------------------------------------------------------
// Get GuildMarkSmall
//----------------------------------------------------------------------
CSprite*	
MGuildMarkManager::GetGuildMarkSmall(WORD guildID)
{
	GUILDMARK_MAP::iterator iMark = m_GuildMarks.find(guildID);

	if (iMark != m_GuildMarks.end())
	{
		return m_GuildMarks[guildID].pSpriteSmall;
	}

	return NULL;
}

//----------------------------------------------------------------------
// Has GuildMark
//----------------------------------------------------------------------
bool			
MGuildMarkManager::HasGuildMark(WORD guildID) const
{
	GUILDMARK_MAP::const_iterator iMark = m_GuildMarks.find(guildID);

	if (iMark != m_GuildMarks.end())
	{
		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Load GuildMark
//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool		
MGuildMarkManager::LoadGuildMark(WORD guildID)
{
	
	GUILDMARK_MAP::iterator iMark = m_GuildMarks.find(guildID);

	if (iMark != m_GuildMarks.end())
	{
		return true;
	}

	MGuildInfoMapper::const_iterator iMapper = g_pGuildInfoMapper->find( guildID );

	if (iMapper!=g_pGuildInfoMapper->end())
	{
		GUILD_INFO* pInfo		= iMapper->second;
		TYPE_SPRITEID spriteID	= pInfo->GetSpriteID();

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		char spkiFilename[256];

		// filename.spki
		strcpy(spkiFilename, m_GuildMarkSPKFilename.GetString());
		strcat(spkiFilename, "i");

		std::ifstream spkFile(m_GuildMarkSPKFilename.GetString(), ios::binary );
		std::ifstream spkiFile(spkiFilename, ios::binary );

		TYPE_SPRITEID maxSpkSize = 0;

		if (spkFile.is_open()
			&& spkiFile.is_open())
		{			
			//---------------------------------------------------------
			
			//---------------------------------------------------------
			spkiFile.seekg( 0, ios::beg );
			spkiFile.read((char*)&maxSpkSize, 2);			

			if (spriteID < maxSpkSize)	
			{
				long fp = 0;

				//-----------------------------------------------------
				
				//-----------------------------------------------------
				spkiFile.seekg( 2 + spriteID*sizeof(long), ios::beg );
				spkiFile.read((char*)&fp, 4);				
				spkiFile.close();

				spkFile.seekg( fp, ios::beg );

				//-----------------------------------------------------
				
				//-----------------------------------------------------
				CSprite* pSprite = NULL;
				CSprite* pSpriteSmall = NULL;

				if (ColorDraw::Is565())
				{
					pSprite = new CSprite565;
					pSpriteSmall = new CSprite565;
				}
				else
				{
					pSprite = new CSprite555;
					pSpriteSmall = new CSprite555;
				}

				//-----------------------------------------------------
				
				//-----------------------------------------------------
				pSprite->LoadFromFile( spkFile );
				pSpriteSmall->LoadFromFile( spkFile );	
				
				spkFile.close();

				m_GuildMarks[guildID] = GUILD_SPRITES( pSprite, pSpriteSmall );

				return true;
			}
		}		
	}

	
	
	m_GuildMarks[guildID] = GUILD_SPRITES( NULL, NULL );

	return false;
}

//----------------------------------------------------------------------
// Save GuildMark
//----------------------------------------------------------------------



//

//----------------------------------------------------------------------
bool		
MGuildMarkManager::SaveGuildMark(WORD guildID, CSprite* pSprite, CSprite* pSpriteSmall)
{
	if (IsInit()
		&& pSprite!=NULL
		&& pSpriteSmall!=NULL)
	{
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		char spkiFilename[256];

		// filename.spki
		strcpy(spkiFilename, m_GuildMarkSPKFilename.GetString());
		strcat(spkiFilename, "i");

		
		//---------------------------------------------------------
		
		
		//---------------------------------------------------------
		std::ifstream spkInputFile(m_GuildMarkSPKFilename.GetString(), ios::binary );

		TYPE_SPRITEID maxSpkSize = 0;

		if (spkInputFile.is_open())
		{			
			//---------------------------------------------------------
			
			//---------------------------------------------------------
			spkInputFile.seekg( 0, ios::beg );
			spkInputFile.read((char*)&maxSpkSize, 2);
			spkInputFile.close();
		}

		
		std::ofstream spkFile(m_GuildMarkSPKFilename.GetString(), ios::binary | ios::ate);
		std::ofstream spkiFile(spkiFilename, ios::binary | ios::ate);
		
		if (spkFile.is_open()
			&& spkiFile.is_open())
		{
			//---------------------------------------------------------
			//
			//					SpritePack
			//
			//---------------------------------------------------------			

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			TYPE_SPRITEID newSpkSize = maxSpkSize + 2;
			
			spkFile.seekp( 0, ios::beg );
			spkFile.write((const char*)&newSpkSize, 2);
			
			//---------------------------------------------------------
			
			//---------------------------------------------------------
			spkFile.seekp( 0, ios::end );

			long fp = spkFile.tellp();		
			pSprite->SaveToFile( spkFile );

			long fpSmall = spkFile.tellp();		
			pSpriteSmall->SaveToFile( spkFile );

			
			//---------------------------------------------------------
			//
			//					SpritePack Index
			//
			//---------------------------------------------------------					
			
			//---------------------------------------------------------
			spkiFile.seekp( 0, ios::beg );
			spkiFile.write((const char*)&newSpkSize, 2);

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			spkiFile.seekp( 0, ios::end );
			spkiFile.write((const char*)&fp, 4);
			spkiFile.write((const char*)&fpSmall, 4);
	
			spkFile.close();
			spkiFile.close();

			//---------------------------------------------------------
			//
			//					GuildSpriteIDMapper
			//
			//---------------------------------------------------------
			// BigSpriteID = maxSpkSize
			// SmallSpriteID = maxSpkSize + 1
			GUILD_INFO* pInfo = g_pGuildInfoMapper->Get(guildID);

			if (pInfo!=NULL)
			{
				pInfo->SetSpriteID( maxSpkSize );
			}
			else
			{
				
				pInfo = new GUILD_INFO;

				// default
//				pInfo->SetGuildName("Guild");
//				pInfo->SetLeaderName("Leader");				
				pInfo->SetSpriteID( maxSpkSize );

				g_pGuildInfoMapper->Set(guildID, pInfo);
			}			
			
			
			//class fstream mapperFile(m_GuildMarkSpriteMapperFilename.GetString(), ios::binary | ios::ate | ios::in | ios::out);
			//g_pGuildInfoMapper->ChangeValueToFile(m_GuildMarkSpriteMapperFilename.GetString(), guildID);

			std::ofstream file(m_GuildMarkSpriteMapperFilename.GetString(), ios::binary);

			if (file.is_open())
			{
				g_pGuildInfoMapper->SaveToFile(file);
			}

			
			
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------
// MergeGuildMark
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool		
MGuildMarkManager::MergeGuildMark(const char* pSPKFilenameOrg, 
									const char* pIDMapperFilenameOrg,
									const char* pSPKFilenameApp, 
									const char* pIDMapperFilenameApp)
{
	#if defined(__GAME_CLIENT__) || defined(__GUILD_MANAGER_TOOL__)
		if (pSPKFilenameOrg==NULL
			|| pIDMapperFilenameOrg==NULL
			|| pSPKFilenameApp==NULL
			|| pIDMapperFilenameApp==NULL)
		{
			return false;
		}

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		char pSPKIFilenameOrg[256];
		char pSPKIFilenameApp[256];

		sprintf(pSPKIFilenameOrg, "%si", pSPKFilenameOrg);
		sprintf(pSPKIFilenameApp, "%si", pSPKFilenameApp);
		

		//---------------------------------------------------------
		
		
		
		//---------------------------------------------------------
		std::ifstream spkInputFile(pSPKFilenameOrg, ios::binary );
		std::ifstream spkInputFileApp(pSPKFilenameApp, ios::binary );
		std::ifstream spkiInputFileApp(pSPKIFilenameApp, ios::binary );

		TYPE_SPRITEID orgSize = 0;			
		TYPE_SPRITEID appSize = 0;			
		TYPE_SPRITEID orgiSize = 0;			
		TYPE_SPRITEID appiSize = 0;			
		long orgFP = 0;						
		long appFP = 0;						

		if (!spkInputFileApp.is_open()
			|| !spkiInputFileApp.is_open())	
		{
			return false;
		}

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		if (spkInputFile.is_open())
		{				
			spkInputFile.read((char*)&orgSize, 2);		

			spkInputFile.seekg( 0, ios::end );		
			orgFP = spkInputFile.tellg();

			spkInputFile.close();		
		}

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		spkInputFileApp.read((char*)&appSize, 2);
		spkiInputFileApp.read((char*)&appiSize, 2);

		spkInputFileApp.seekg( 0, ios::end );		
		appFP = spkInputFileApp.tellg();

		spkInputFileApp.close();
		spkiInputFileApp.close();

		if (appSize <= 0
			|| appSize!=appiSize)		
		{
			return false;
		}

		//-----------------------------------------------------------------------------
		// SpritePack Append
		//-----------------------------------------------------------------------------
		AppendPatch ap;
		
		ap.SetOriginalInfo( pSPKFilenameOrg, orgFP, orgSize );
		ap.AddAppendInfo( pSPKFilenameApp, appFP, appSize );
		ap.SetFinalInfo( orgFP+appFP-2, orgSize+appSize );

		int errorCode = 0;	
		
		g_bUseProgressBar = false;	

		if (ap.ExecutePatch())
		{
			if (!ap.CheckFinalInfo())
			{
				errorCode = 2;
			}
		}
		else
		{
			errorCode = 1;
		}

		g_bUseProgressBar = true;

		if (errorCode)
		{
			DEBUG_ADD_FORMAT("[Error] GuildMarkPatch Terrible! code=%d", errorCode); 
		}

		//-----------------------------------------------------------------------------
		
		//-----------------------------------------------------------------------------
		std::ofstream spkiFileOrg(pSPKIFilenameOrg, ios::binary | ios::ate);
		std::ifstream spkiFileApp(pSPKIFilenameApp, ios::binary );

		TYPE_SPRITEID newSize = orgSize + appSize;

		
		spkiFileOrg.seekp( 0, ios::beg );
		spkiFileOrg.write((char*)&newSize, 2);
		
		spkiFileOrg.seekp( 0, ios::end );		
		spkiFileApp.seekg( 2 );					

		long fp;
		long addFP = orgFP - 2;					
		for (int i=0; i<appiSize; i++)
		{
			spkiFileApp.read((char*)&fp, 4);

			fp += addFP;

			spkiFileOrg.write((char*)&fp, 4);
		}

		spkiFileOrg.close();
		spkiFileApp.close();
		
		//-----------------------------------------------------------------------------
		
		//
		
		
		//-----------------------------------------------------------------------------
		MGuildInfoMapper mapperOrg;
		MGuildInfoMapper mapperApp;

		std::ifstream mapperFileOrg(pIDMapperFilenameOrg, ios::binary );
		std::ifstream mapperFileApp(pIDMapperFilenameApp, ios::binary );

		if (mapperFileOrg.is_open())
		{
			mapperOrg.LoadFromFile( mapperFileOrg );
			mapperFileOrg.close();
		}

		if (mapperFileApp.is_open())
		{
			mapperApp.LoadFromFile( mapperFileApp );
			mapperFileApp.close();
		}

		
		MGuildInfoMapper::const_iterator iMapper = mapperApp.begin();

		while (iMapper != mapperApp.end())
		{
			WORD guildID				= iMapper->first;
			GUILD_INFO*		pAppInfo	= iMapper->second;
			TYPE_SPRITEID	newSpriteID = pAppInfo->GetSpriteID() + orgSize;

			
			GUILD_INFO* pInfo = mapperOrg.Get(guildID);

			if (pInfo!=NULL)
			{
				
				pInfo->SetSpriteID( newSpriteID );
			}
			else
			{
				
				pInfo = new GUILD_INFO;

				*pInfo = *pAppInfo;

				
				pInfo->SetSpriteID( newSpriteID );

				mapperOrg.Set( guildID, pInfo );
			}		

			iMapper ++;
		}

		
		std::ofstream mapperOutputFileOrg(pIDMapperFilenameOrg, ios::binary);
		mapperOrg.SaveToFile( mapperOutputFileOrg );
		mapperOutputFileOrg.close();

		//-----------------------------------------------------------------------------
		
		//-----------------------------------------------------------------------------
		
		remove(pSPKIFilenameApp);
		remove(pIDMapperFilenameApp);
	#endif

	return true;
}


////----------------------------------------------------------------------
//// Load GradeMark
////----------------------------------------------------------------------
//bool		
//MGuildMarkManager::LoadGradeMark(WORD gradeSpriteID)
//{

//	if(m_GradeMarks[gradeSpriteID].IsInit())
//		return true;
//
//	MString *filename = NULL;
//	int fileSpriteID = 0;
//

//	if(gradeSpriteID < GRADE_MARK_MAX*2)
//	{
//		filename = &m_SlayerGradeMarkSPKFilename;
//		fileSpriteID = gradeSpriteID;
//	}
//	else
//	{
//		filename = &m_VampireGradeMarkSPKFilename;
//		fileSpriteID = gradeSpriteID - GRADE_MARK_MAX*2;
//	}
//

//	if(filename->GetLength() == 0)
//		return false;
//	
//	//---------------------------------------------------------

//	//---------------------------------------------------------
//	char spkiFilename[256];
//	
//	// filename.spki
//	strcpy(spkiFilename, filename->GetString());
//	strcat(spkiFilename, "i");
//	
//	m_GradeMarks.LoadFromFileSprite(gradeSpriteID, fileSpriteID, filename->GetString(), spkiFilename);
//
//	return true;
//}

//----------------------------------------------------------------------
// Get GradeMark
//----------------------------------------------------------------------
CSprite*	
MGuildMarkManager::GetGradeMark(WORD gradeID, Race race)
{
	if(race >= RACE_MAX)
		return NULL;

	return &m_GradeMarks[race][gradeID*2];
}

//----------------------------------------------------------------------
// Get GradeMarkSmall
//----------------------------------------------------------------------
CSprite*	
MGuildMarkManager::GetGradeMarkSmall(WORD gradeID, Race race)
{
	if(race >= RACE_MAX)
		return NULL;
	
	return &m_GradeMarks[race][gradeID*2+1];
}

//----------------------------------------------------------------------
// Get LevelMark
//----------------------------------------------------------------------
CSprite*	
MGuildMarkManager::GetLevelMark(WORD level)
{
	return &m_LevelMarks[level*2];
}

//----------------------------------------------------------------------
// Get LevelMarkSmall
//----------------------------------------------------------------------
CSprite*	
MGuildMarkManager::GetLevelMarkSmall(WORD level)
{
	return &m_LevelMarks[level*2+1];
}



bool
MGuildMarkManager::CreateGuildMark(const char* pFilename, CSprite *&pSprite, CSprite *&pSpriteSmall)
{
#ifdef PLATFORM_WINDOWS
	if (pFilename==NULL)
	{
		return false;
	}

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	CDirectDrawSurface bmpSurface;
	if (!LoadImageToSurface(pFilename, bmpSurface))
	{
		return false;
	}

	const POINT bigSize = { 40, 40 };
	const POINT smallSize = { 20, 20 };

	CDirectDrawSurface surface;
	surface.InitOffsurface( bigSize.x, bigSize.y, DDSCAPS_SYSTEMMEMORY );

	RECT destBigRect = { 0, 0, bigSize.x, bigSize.y };
	RECT destSmallRect = { 0, 0, smallSize.x, smallSize.y };

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (ColorDraw::Is565())
	{
		pSprite = new CSprite565;
		pSpriteSmall = new CSprite565;
	}
	else
	{
		pSprite = new CSprite555;
		pSpriteSmall = new CSprite555;
	}

	//-----------------------------------------------------
	// surface --> CSprite
	//-----------------------------------------------------
	WORD* lpSurface;
	unsigned short pitch;

	RECT bmpRect = { 0, 0, bmpSurface.GetWidth(), bmpSurface.GetHeight() };

	
	CSprite::SetColorkey( 0x001F );

	// BigSize
	surface.FillSurface( 0 );
	surface.Blt(&destBigRect, &bmpSurface, &bmpRect);
	surface.LockW(lpSurface, pitch);
	pSprite->SetPixel(lpSurface, pitch, bigSize.x, bigSize.y);
	surface.Unlock();

	// SmallSize
	surface.FillSurface( 0 );
	surface.Blt(&destSmallRect, &bmpSurface, &bmpRect);
	surface.LockW(lpSurface, pitch);
	pSpriteSmall->SetPixel(lpSurface, pitch, smallSize.x, smallSize.y);
	surface.Unlock();

	//-----------------------------------------------------
	// add
	//-----------------------------------------------------
//	AddGuildMark(guildID, pSprite, pSpriteSmall);
	return true;
#else
	// SDL backend: not implemented
	return false;
#endif
}
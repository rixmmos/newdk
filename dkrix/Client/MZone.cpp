//----------------------------------------------------------------------
// MZone.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#pragma warning(disable:4786)

#include <math.h>
#include "MZoneDef.h"
#include "MSector.h"
#include "MCreature.h"
#include "MFakeCreature.h"
#include "MPlayer.h"
#include "MImageObject.h"
#include "MShadowObject.h"
#include "MAnimationObject.h"
#include "MShadowAnimationObject.h"
#include "MInteractionObject.h"
#include "MPortal.h"
#include "MItem.h"
#include "MEffect.h"
#include "MZone.h"
// Note: MTopView.h removed - MZone doesn't actually use MTopView (decoupled for demo/game flexibility)
#include "UserOption.h"
#include "MEffectGeneratorTable.h"
#include "MZoneTable.h"
#include "CMessageArray.h"
#include "DebugInfo.h"
#include "ServerInfo.h"
#include "Client.h"
#include "EffectSpriteTypeDef.h"
#include "SoundDef.h"
#include "MEffectSpriteTypeTable.h"
#include "MHelicopterManager.h"
#include "MParty.h"
#include "MTestDef.h"
#include <fstream>
#include "SkillDef.h"
#include "mintr.h"
#include "MNPC.h"
#include "MGuildType.h"
#include "MEffectStatusDef.h"

// Note: VS_UI.h removed - MZone doesn't actually use VS_UI classes (decoupled for demo/game flexibility)
#include "MHelpDef.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MZone*				g_pZone				= NULL;

BOOL g_bPlayPropeller = FALSE;

extern HWND					g_hWnd;
extern DWORD				g_ZoneRandomSoundTime;

extern void		SendPositionInfoToParty();
extern void		SendStatusInfoToParty();
//GameNew Mode  add by sonc 2006.9.27
extern	LONG g_SECTOR_WIDTH;
extern	LONG g_SECTOR_HEIGHT;
extern	LONG g_SECTOR_WIDTH_HALF;
extern	LONG g_SECTOR_HEIGHT_HALF;
extern	LONG g_SECTOR_SKIP_PLAYER_LEFT;
extern	LONG g_SECTOR_SKIP_PLAYER_UP;
extern	LONG g_TILESURFACE_SECTOR_WIDTH;
extern	LONG g_TILESURFACE_SECTOR_HEIGHT;
extern	LONG g_TILESURFACE_SECTOR_OUTLINE_RIGHT;
extern	LONG g_TILESURFACE_SECTOR_OUTLINE_DOWN;
extern	LONG g_TILESURFACE_WIDTH;
extern	LONG g_TILESURFACE_HEIGHT;
extern	LONG g_TILESURFACE_OUTLINE_RIGHT;
extern	LONG g_TILESURFACE_OUTLINE_DOWN;
//end 
#ifdef __METROTECH_TEST__
	extern int		g_iSpeed;
#endif

#ifdef	OUTPUT_DEBUG
	//extern CMessageArray		g_DebugMessage;
	#ifdef __METROTECH_TEST__
//		#define OUTPUT_DEBUG_UPDATE_ZONE
	#endif
//	#define OUTPUT_DEBUG_UPDATE_EFFECT
#endif


//#ifdef __OUTPUT_IMAGEOBJECT__

#define	VISION_SECTOR_WIDTH_HALF				12
#define	VISION_SECTOR_HEIGHT_HALF				13

#define NEW_VISION_SECTOR_WIDTH_HALF			17
#define NEW_VISION_SECTOR_HEIGHT_HALF_UP		24
#define NEW_VISION_SECTOR_HEIGHT_HALF_DOWN		24

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
IsRelicTable( MItem* pItem )
{
	if (pItem->GetItemClass()==ITEM_CLASS_CORPSE)
	{
		MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();
		
		return pCreature->GetCreatureType()>=371 && pCreature->GetCreatureType()<=376 ||
				pCreature->GetCreatureType()>=560 && pCreature->GetCreatureType()<=563 ||
				pCreature->GetCreatureType()>=526 && pCreature->GetCreatureType()<=549;
	}

	return false;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
#define	CheckCreatureInDarkness( sector, x, y )						\
		{															\
			if (g_pPlayer->GetX()==x && g_pPlayer->GetY()==y)		\
			{														\
				g_pPlayer->CheckInDarkness();						\
			}														\
																	\
			int numCreature = sector.GetCreatureSize();				\
																	\
			if (numCreature > 0)									\
			{														\
				OBJECT_MAP::iterator iCreature = sector.GetCreatureIterator();	\
																		\
				for (int i=0; i<numCreature; i++, iCreature ++)			\
				{														\
					if (iCreature==sector.GetObjectEnd()					\
						|| iCreature->first > MSector::POSITION_FLYINGCREATURE_MAX)	\
						break;												\
																			\
					MCreature* pCreature = (MCreature*)iCreature->second;	\
																		\
					if (pCreature!=NULL)								\
					{													\
						pCreature->CheckInDarkness();					\
					}													\
				}														\
			}															\
																		\
			if (sector.IsExistItem())									\
			{															\
				MItem* pItem = sector.GetItem();						\
																		\
				if (pItem->GetItemClass()==ITEM_CLASS_CORPSE)	\
				{														\
					MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();	\
																		\
					if (pCreature!=NULL)								\
					{													\
						pCreature->CheckInDarkness();					\
					}													\
				}														\
			}															\
		}
				
//----------------------------------------------------------------------

//----------------------------------------------------------------------
#define RemoveSectorEffect( sX, sY, id )						\
		{														\
			MSector& sector = m_ppSector[sY][sX];				\
																\
			bool bSanctuary = sector.HasSanctuary();			\
			sector.RemoveEffect( id );							\
																\
			if (bSanctuary && !sector.HasSanctuary())			\
			{													\
				for (int i=-1; i<=1; i++)						\
				{												\
					for (int j=-1; j<=1; j++)					\
					{											\
						int x0 = sX + i;						\
						int y0 = sY + j;						\
																\
						if (i==0 && j==0						\
							|| x0<0 || x0>=m_Width				\
							|| y0<0 || y0>=m_Height)			\
						{										\
							continue;							\
						}										\
																\
						MSector& sector2 = m_ppSector[y0][x0];	\
						sector2.UnSetSanctuary();				\
					}											\
				}												\
			}													\
		}


//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

MZone::MZone(TYPE_SECTORPOSITION width, TYPE_SECTORPOSITION height)
{
	m_Width = 0;
	m_Height =0;

	m_ppSector	= NULL;
	//m_pPlayer	= NULL;

	Init(width, height);

	m_bZoneSoundLoaded = false;

	DEBUG_ADD("Load HornInfo");

	// Skip horn loading if g_pFileDef is not initialized (e.g., in demo mode)
	if (g_pFileDef != nullptr) {
		int zoneNum, portalNum;
		UI_PORTAL_FLAG portal;
		UI_PORTAL_LIST portalList;

		ifstream hornInfoFile(g_pFileDef->getProperty("FILE_INFO_HORN").c_str(), ios::binary);
		hornInfoFile.read((char *)&zoneNum, sizeof(int));
		for(int zone = 0; zone < zoneNum; zone++)
		{
			hornInfoFile.read((char *)&portalNum, sizeof(int));
			for(int p = 0; p < portalNum; p++)
			{
				hornInfoFile.read((char *)&portal.zone_id, sizeof(int));
				hornInfoFile.read((char *)&portal.x, sizeof(int));
				hornInfoFile.read((char *)&portal.y, sizeof(int));
				hornInfoFile.read((char *)&portal.portal_x, sizeof(int));
				hornInfoFile.read((char *)&portal.portal_y, sizeof(int));
				portalList.push_back(portal);
			}
			m_horn.push_back(portalList);
			portalList.clear();
		}
		hornInfoFile.close();
		DEBUG_ADD("Load HornInfo OK");
	}
}

MZone::~MZone()
{
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void
MZone::Init(TYPE_SECTORPOSITION width, TYPE_SECTORPOSITION height)
{
	
	if (width==0 || height==0) 
		return;

	
	Release();

	m_Width	 = width;
	m_Height = height;

	m_ppSector = new MSector* [m_Height];

	int i;
	for (i=0; i<m_Height; i++)
	{
		m_ppSector[i] = new MSector [m_Width];
	}

	//for (i=0; i<m_Height; i++)
	//{
	//	for (int j=0; j<m_Width; j++)
	//	{
	//		m_ppSector[i][j].SetFilterSpriteID( 0 );
	//	}
	//}
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
MZone::Release()
{
	//---------------------------------
	
	//---------------------------------
	//m_pPlayer = NULL;

	DEBUG_ADD( "Release ImageObject" );
	
	//---------------------------------
	
	//---------------------------------
	IMAGEOBJECT_MAP::iterator iImageObject = m_mapImageObject.begin();

	MImageObject *pImageObject = NULL;
	while (iImageObject != m_mapImageObject.end())
	{
		pImageObject = (*iImageObject).second;
		if (pImageObject!=NULL)
		{
			delete pImageObject;					
		}
		iImageObject++;
	}
	m_mapImageObject.clear();


	DEBUG_ADD( "Start Release Object" );
	
	//---------------------------------
	
	//---------------------------------
	ReleaseObject();

	//---------------------------------
	
	//---------------------------------
	/*
	OBSTACLE_LIST::iterator iObstacle = m_listObstacle.begin();

	MObstacle *pObstacle = NULL;
	while (iObstacle != m_listObstacle.end())
	{
		pObstacle = *iObstacle;
		if (pObstacle != NULL)
			delete pObstacle;
		iObstacle++;
	}
	m_listObstacle.clear();
	*/

	DEBUG_ADD_FORMAT( "MZone::Release() - %d x %d", m_Width, m_Height);

	//---------------------------------
	
	//---------------------------------	
	if (m_ppSector!=NULL)
	{
		int i;
		for (i=0; i<m_Height; i++)
		{
			delete [] m_ppSector[i];			
		}

		delete [] m_ppSector;		
		
		m_ppSector=NULL;
		m_Width = 0;
		m_Height =0;
	}


	m_bZoneSoundLoaded = false;
	DEBUG_ADD("zone rel ok");
}

//----------------------------------------------------------------------
// Release Object
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MZone::ReleaseObject()
{
	m_HelicopterManager.Release();

	//---------------------------------
	
	//---------------------------------
	DEBUG_ADD_FORMAT("MZone::ReleaseCreature. size=%d", m_mapCreature.size());
	
	CREATURE_MAP::iterator iCreature;
	iCreature = m_mapCreature.begin();

	MCreature *pCreature = NULL;
	while (iCreature != m_mapCreature.end())
	{
		pCreature = (*iCreature).second;
		
		if (pCreature!=NULL && pCreature->GetClassType()!=MCreature::CLASS_PLAYER)
		{
			DEBUG_ADD_FORMAT("RemoveCreature: [%s] id=%d, (%d, %d)", pCreature->GetName(), pCreature->GetID(), pCreature->GetX(), pCreature->GetY());
			
			/*
			if (g_pParty!=NULL && pCreature->IsPlayerParty())
			{
				PARTY_INFO* pInfo = g_pParty->GetMemberInfo( GetName() );

				if (pInfo!=NULL)
				{
					pInfo->bInSight = false;					
					pInfo->zoneID = 60002;
				}
			}
			*/
			
			
			if (m_ppSector!=NULL)
			{
				int id = pCreature->GetID();
				int x = pCreature->GetX();
				int y = pCreature->GetY();
				int serverX = pCreature->GetServerX();
				int serverY = pCreature->GetServerY();

				UnSetServerBlock(pCreature->GetMoveType(), serverX, serverY);

				//if (x>=0 && y>=0 && x<m_Width && y<m_Height) 
				{
					//------------------------------------------------
					
					//------------------------------------------------
					if (!m_ppSector[y][x].RemoveCreature(id))
					{
						if (!m_ppSector[serverY][serverX].RemoveCreature(id))
						{	
							DEBUG_ADD_FORMAT("Can't RemoveCreatureWhenRelease! ID=%d client(%d,%d), server(%d,%d)", pCreature->GetID(), x,y,serverX,serverY);
						}
					}

					/*
					bool removed = m_ppSector[y][x].RemoveGroundCreature( id );
					if (!removed)
					{
						removed = m_ppSector[serverY][serverX].RemoveGroundCreature( id );

						if (!removed)
						{
							removed = m_ppSector[y][x].RemoveFlyingCreature( id );

							if (!removed)
							{
								removed = m_ppSector[serverY][serverX].RemoveFlyingCreature( id );

								if (!removed)
								{
									removed = m_ppSector[y][x].RemoveUndergroundCreature( id );

									if (!removed)
									{
										removed = m_ppSector[serverY][serverX].RemoveUndergroundCreature( id );

										DEBUG_ADD_FORMAT("[Error] Can't Remove Creature. ID=%d. xy=(%d, %d), sxy=(%d, %d)", id, x, y, serverX, serverY);
									}
								}
							}
						}
					}
					*/
				}
			}
		
			DEBUG_ADD("RemoveCreature: OK");
			
			delete pCreature;

			DEBUG_ADD("DeleteCreature: OK");
		}
		iCreature++;
	}
	m_mapCreature.clear();

	//---------------------------------
	
	//---------------------------------
	DEBUG_ADD_FORMAT("MZone::ReleaseFakeCreature. size=%d", m_mapFakeCreature.size());
	
	CREATURE_MAP::iterator iFakeCreature= m_mapFakeCreature.begin();

	while (iFakeCreature != m_mapFakeCreature.end())
	{
		pCreature = (*iFakeCreature).second;
		if (pCreature!=NULL)
		{
			delete pCreature;					
		}
		iFakeCreature++;
	}
	m_mapFakeCreature.clear();


	//---------------------------------
	
	//---------------------------------
	DEBUG_ADD_FORMAT("MZone::ReleaseItem. size=%d", m_mapItem.size());
	
	ITEM_MAP::iterator iItem = m_mapItem.begin();

	MItem *pItem = NULL;
	while (iItem != m_mapItem.end())
	{
		pItem = (*iItem).second;
		if (pItem!=NULL)
		{
			if (m_ppSector!=NULL)
			{
				
				MSector& sector = m_ppSector[pItem->GetY()][pItem->GetX()];
				sector.RemoveItem();

				
				if (pItem->GetItemClass()==ITEM_CLASS_CORPSE)
				{
					sector.UnSetBlockServerGround();

					
					if (IsRelicTable(pItem))
					{
						sector.UnSetBlockGround();
						sector.UnSetBlockFlying();
					}
				}
			}

			delete pItem;					
		}
		iItem++;
	}
	m_mapItem.clear();

	//---------------------------------
	
	//---------------------------------
	DEBUG_ADD_FORMAT("MZone::ReleaseEffect. size=%d", m_mapEffect.size());
	
	EFFECT_MAP::iterator iEffect = m_mapEffect.begin();

	MEffect *pEffect = NULL;
	while (iEffect != m_mapEffect.end())
	{
		pEffect = iEffect->second;
		if (pEffect != NULL)
		{
			if (m_ppSector!=NULL)
			{
				
				RemoveSectorEffect( pEffect->GetX(), pEffect->GetY(), pEffect->GetID() );				
			}

			delete pEffect;			
		}
		iEffect++;
	}
	m_mapEffect.clear();

	//---------------------------------
	
	//---------------------------------
	DEBUG_ADD_FORMAT("MZone::ReleaseGroundEffect. size=%d", m_mapGroundEffect.size());
	
	iEffect = m_mapGroundEffect.begin();

	while (iEffect != m_mapGroundEffect.end())
	{
		pEffect = iEffect->second;
		if (pEffect != NULL)
		{	
			delete pEffect;			
		}
		iEffect++;
	}
	m_mapGroundEffect.clear();


	//---------------------------------
	
	//---------------------------------
	DEBUG_ADD_FORMAT("MZone::ReleaseSound. size=%d", m_listSoundNode.size());
	
	SOUND_NODE_LIST::iterator iSound = m_listSoundNode.begin();

	while (iSound != m_listSoundNode.end())
	{
		SOUND_NODE*	pNode = *iSound;

		
		delete pNode;
		
		iSound++;			
	}

	m_listSoundNode.clear();

	// 2004, 11, 29, sobeit add start
	//-----------------------------------------------
	
	//-----------------------------------------------
	DEBUG_ADD_FORMAT("MZone::ReleaseWaitEffect. size=%d", m_listWaitEffect.size());
	WAIT_EFFECT_LIST::iterator iWaitEffect = m_listWaitEffect.begin();

	while (iWaitEffect != m_listWaitEffect.end())
	{
		MEffect*	pNode = *iWaitEffect;

		delete pNode;

		iWaitEffect ++;
	}
	m_listWaitEffect.clear();
	// 2004, 11, 29, sobeit add end
	
	DEBUG_ADD("MZone::Release OK");
}

//----------------------------------------------------------------------
// Save To File SectorSound
//----------------------------------------------------------------------
// Width*Height
// SectorSoundInfo
// //MZoneSoundTable
//----------------------------------------------------------------------
bool		
MZone::SaveToFileSectorSound(std::ofstream& file)
{
	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	file.write((const char*)&m_Width, SIZE_SECTORPOSITION);
	file.write((const char*)&m_Height, SIZE_SECTORPOSITION);

	//-----------------------------------------------------------
	// SectorSoundInfo
	//-----------------------------------------------------------
	for (int x=0; x<m_Width; x++)
	{
		for (int y=0; y<m_Height; y++)
		{
			const SECTORSOUND_LIST& listSound = m_ppSector[y][x].GetSectorSoundList();

			SECTORSOUND_LIST::const_iterator iSound = listSound.begin();

			int num = listSound.size();

			
			file.write((const char*)&num, 4);

			
			while (iSound != listSound.end())
			{
				const SECTORSOUND_INFO& info = *iSound;

				info.SaveToFile( file );

				iSound++;
			}			
		}
	}

	 

	return true;
}

//----------------------------------------------------------------------
// Load From File SectorSound
//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool		
MZone::LoadFromFileSectorSound(std::ifstream& file)
{
	m_bZoneSoundLoaded = false;

	 

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	//TYPE_SECTORPOSITION width, height;

	//file.read((char*)&width, SIZE_SECTORPOSITION);
	//file.read((char*)&height, SIZE_SECTORPOSITION);

	//if (m_Width!=width || m_Height!=height)
	{
		
		
	//	return false;
	}

	//-----------------------------------------------------------
	// SectorSoundInfo
	//-----------------------------------------------------------
	//
	
	//
	
	
	
	
	//   a~z       : SECTORSOUND_INFO
	//
	
	//
	// ex) -----AAABBBAAC--DEEEEE---
	//
	
	
	//
	
	
	
	
	
	
	//
	
	
	
	
	//
	//-----------------------------------------------------------
	BYTE				num;
	SECTORSOUND_INFO	info;

	for (int y=0; y<m_Height; y++)
	{
		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		file.read((char*)&num, 1);

		int x = 0;
		int numCount = num;
		
		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		for (int i=0; i<numCount; i++)
		{
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			file.read((char*)&num, 1);	
			x += num;	

			//-----------------------------------------------------
			
			
			//-----------------------------------------------------
			file.read((char*)&num, 1);	
			int numSoundType = num;

			//-----------------------------------------------------
			
			//-----------------------------------------------------
			for (int j=0; j<numSoundType; j++)
			{
				//-----------------------------------------------------
				
				//-----------------------------------------------------
				file.read((char*)&num, 1);		// assert( num > 0 );
				int numSector = num;

				//-----------------------------------------------------
				
				//-----------------------------------------------------
				file.read((char*)&num, 1);		// assert( num > 0 );
				int numSound = num;

				//-----------------------------------------------------
				
				//-----------------------------------------------------
				for (int n=0; n<numSound; n++)
				{
					//-----------------------------------------------------
					
					//-----------------------------------------------------
					info.LoadFromFile( file );
					
					int xPlusNumSector = x + numSector;
					for (int k=x; k<xPlusNumSector; k++)
					{
						m_ppSector[y][k].AddSectorSound( info );
					}
				}

				x += numSector;
			}
		}
	}

	/*
	for (int x=0; x<m_Width; x++)
	{
		for (int y=0; y<m_Height; y++)
		{
			file.read((char*)&num, 1);

			#ifdef _DEBUG
				if (file.eof())
				{
					int a = 0;
				}
			#endif

			intNum = num;

			for (int n=0; n<intNum; n++)
			{
				info.LoadFromFile( file );

				#ifdef _DEBUG
					if (file.eof())
					{
						int a = 0;
					}
				#endif

				m_ppSector[y][x].AddSectorSound( info );			
			}
		}
	}
	*/
	

	//-----------------------------------------------------------
	// MZoneSoundTable
	//-----------------------------------------------------------
	
	
	//-----------------------------------------------------------
	 

	m_bZoneSoundLoaded = true;
	return true;

}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------

//   Zone ID




//----------------------------------------------------------------------
bool		
MZone::LoadFromFile(std::ifstream& file)
{
	//-------------------------------------------------
	// Header
	//-------------------------------------------------
	m_Info.LoadFromFile( file );
	

	//-------------------------------------------------
	//
	
	//
	//-------------------------------------------------
	if (m_Info.ZoneVersion==MAP_VERSION_2000_05_10)
	{
		//-------------------------------------------------
		// ZoneID 
		
		//-------------------------------------------------	
		file.read((char*)&m_fpTile, 4);				// Tile FP
		file.read((char*)&m_fpImageObject, 4);		// ImageObject FP

		//-------------------------------------------------
		
		//-------------------------------------------------
		Release();

		//-------------------------------------------------
		
		
		//-------------------------------------------------
		file.read((char*)&m_Width, 2);
		file.read((char*)&m_Height, 2);

		
		if (m_Width==0 || m_Height==0)
			return false;

		//-------------------------------------------------
		
		//-------------------------------------------------
		
		Init(m_Width, m_Height);

		///*
		int i,j;

		for (i=0; i<m_Height; i++)
		{
			for (j=0; j<m_Width; j++)
			{
				m_ppSector[i][j].LoadFromFile(file);			
			}
		}

		//*/
		 

		//-------------------------------------------------
		
		//-------------------------------------------------
		
		/*
		for (i=0; i<m_Height; i++)
		{
			for (j=0; j<m_Width; j++)
			{
				if (m_ppSector[i][j].GetLight()!=0)	
				{
					SetLight(j,i, 0);
				}
			}
		}
		*/
		

		int size;

		//-------------------------------------------------
		
		//-------------------------------------------------
		
		 

		//-------------------------------------------------
		
		//-------------------------------------------------	
		file.read((char *)&size, 4);

#ifdef __EMSCRIPTEN__
		extern bool g_demoSkipImageObjects;
		if (g_demoSkipImageObjects)
		{
			printf("Web demo: skipping ImageObject load (count=%d)\n", size);
			return true;
		}
#endif

		//-------------------------------------------------
		
		//-------------------------------------------------
		MImageObject				*pImageObject;
		IMAGEOBJECT_POSITION_LIST	ImageObjectPositionList;
		BYTE						ObjectType;
		for (i=0; i<size; i++)	
		{
			#ifdef __OUTPUT_IMAGEOBJECT__
				char str[1024];
			#endif

			
			file.read((char*)&ObjectType, 1);

			switch (ObjectType)
			{
				case MObject::TYPE_IMAGEOBJECT :
					pImageObject = new MImageObject;
					
					#ifdef __OUTPUT_IMAGEOBJECT__	
						strcpy(str, "ImageObject : ");
					#endif
				break;

				case MObject::TYPE_SHADOWOBJECT :
					pImageObject = new MShadowObject;

					#ifdef __OUTPUT_IMAGEOBJECT__
						strcpy(str, "ShadowObject : ");
					#endif
				break;

				case MObject::TYPE_ANIMATIONOBJECT :
					pImageObject = new MAnimationObject;
					
					#ifdef __OUTPUT_IMAGEOBJECT__
						strcpy(str, "AnimationObject : ");
					#endif
				break;

				case MObject::TYPE_SHADOWANIMATIONOBJECT :
					pImageObject = new MShadowAnimationObject;
					
					#ifdef __OUTPUT_IMAGEOBJECT__
						strcpy(str, "ShadowAnimationObject : ");
					#endif
				break;

				case MObject::TYPE_INTERACTIONOBJECT :
					pImageObject = new MInteractionObject;
					
					#ifdef __OUTPUT_IMAGEOBJECT__
						strcpy(str, "InteractionObject : ");
					#endif
				break;
			}

			pImageObject->LoadFromFile(file);	
			
			#ifdef __OUTPUT_IMAGEOBJECT__
				sprintf(str, "%s[%d] vp=%d. ", str, pImageObject->GetImageObjectID(), pImageObject->GetViewpoint());
			#endif

			
			AddImageObject(pImageObject);

			//-------------------------------------------------
			
			
			
			//-------------------------------------------------		
			ImageObjectPositionList.LoadFromFile(file);		
			
			//-------------------------------------------------------
			
			
			//-------------------------------------------------------
			IMAGEOBJECT_POSITION_LIST::POSITION_LIST::const_iterator 
				iImageObjectPosition = ImageObjectPositionList.GetIterator();


			 

			
			for (int j=0; j<ImageObjectPositionList.GetSize(); j++)
			{
				#ifdef __OUTPUT_IMAGEOBJECT__
					sprintf(str, "%s(%d,%d) ", str, (int)(*iImageObjectPosition).X, (int)(*iImageObjectPosition).Y);
				#endif

				SetImageObjectSector((*iImageObjectPosition).X, (*iImageObjectPosition).Y, pImageObject->GetID());

				 

				iImageObjectPosition++;			
			}			

			#ifdef __OUTPUT_IMAGEOBJECT__
				if (g_pDebugMessage!=NULL)
					g_pDebugMessage->AddToFile( str );
			#endif
			 
		}					
	}
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	else
	{
		return false;
	}



	return true;
}

//----------------------------------------------------------------------
// Set Player 
//


//----------------------------------------------------------------------
void		
MZone::SetPlayer()//MPlayer* pPlayer)
{
	// Player Setting
	//m_pPlayer = pPlayer; 

	// Sector Setting
	//m_ppSector[m_pPlayer->GetY()][m_pPlayer->GetX()].AddGroundCreature(pPlayer);
	//m_ppSector[g_pPlayer->GetY()][g_pPlayer->GetX()].AddGroundCreature(&g_pPlayer->;

	
	//SetSight(g_pPlayer->GetX(), g_pPlayer->GetY(), g_pPlayer->GetLightSight());

	AddCreature( g_pPlayer );
	
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	SendPositionInfoToParty();
	SendStatusInfoToParty();
}

//----------------------------------------------------------------------
// Set Player 
//


//----------------------------------------------------------------------
void		
MZone::RemovePlayer()//MPlayer* pPlayer)
{
	// Player Setting
	//m_pPlayer = pPlayer; 

	// Sector Setting
	//m_ppSector[m_pPlayer->GetY()][m_pPlayer->GetX()].AddGroundCreature(pPlayer);
	//m_ppSector[g_pPlayer->GetY()][g_pPlayer->GetX()].RemoveGroundCreature(&g_pPlayer->;

	
	//SetSight(g_pPlayer->GetX(), g_pPlayer->GetY(), g_pPlayer->GetLightSight());

	RemoveCreature( g_pPlayer->GetID() );	
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool		
MZone::CanMove(BYTE creatureType, TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
{
	
	if (x<0 || x>=m_Width || y<0 || y>=m_Height)
		return false;

	const MSector& sector = m_ppSector[y][x];

	//------------------------------------------------------
	
	//------------------------------------------------------
	if (sector.HasSanctuary()
		&& creatureType != MCreature::CREATURE_FAKE_NO_BLOCK
		&& creatureType != MCreature::CREATURE_FAKE_UNDERGROUND
		&& creatureType != MCreature::CREATURE_FAKE_GROUND
		&& creatureType != MCreature::CREATURE_FAKE_FLYING
		)
	{
		return false;
	}


	
	//if (m_ppSector[y][x].IsExistObject())
	{
		
		switch (creatureType)
		{
			// UNDERGROUND CREATURE
			case MCreature::CREATURE_UNDERGROUND :
			case MCreature::CREATURE_FAKE_UNDERGROUND :
				return sector.CanStandUndergroundCreature();// || sector.IsPortal();

			// CREATURE
			case MCreature::CREATURE_GROUND :
			case MCreature::CREATURE_FAKE_GROUND :
				return sector.CanStandGroundCreature();// || sector.IsPortal();

			// FLYING CREATURE
			case MCreature::CREATURE_FLYING :
			case MCreature::CREATURE_FAKE_FLYING :
				return sector.CanStandFlyingCreature();// || sector.IsPortal();

			
			case MCreature::CREATURE_FAKE_NO_BLOCK :
				return true;
		}		
	}

	return true;
}

//----------------------------------------------------------------------
// Set ServerBlock
//----------------------------------------------------------------------
void		
MZone::SetServerBlock(BYTE creatureType, TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
{
	
	if (x<0 || x>=m_Width || y<0 || y>=m_Height)
		return;

	
	//if (m_ppSector[y][x].IsExistObject())
	{
		
		switch (creatureType)
		{
			// UNDERGROUND CREATURE
			case MCreature::CREATURE_UNDERGROUND :
				m_ppSector[y][x].SetBlockServerUnderground();
			break;

			// CREATURE
			case MCreature::CREATURE_GROUND :
				m_ppSector[y][x].SetBlockServerGround();
			break;

			// FLYING CREATURE
			case MCreature::CREATURE_FLYING :
				m_ppSector[y][x].SetBlockServerFlying();
			break;
		}		
	}
}

//----------------------------------------------------------------------
// UnSet ServerBlock
//----------------------------------------------------------------------
void		
MZone::UnSetServerBlock(BYTE creatureType, TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
{
	
	if (x<0 || x>=m_Width || y<0 || y>=m_Height)
		return;

	
	//if (m_ppSector[y][x].IsExistObject())
	{
		
		
		// 2002.3.29
		
		/*
		switch (creatureType)
		{
			// UNDERGROUND CREATURE
			case MCreature::CREATURE_UNDERGROUND :
				m_ppSector[y][x].UnSetBlockServerUnderground();
			break;

			// CREATURE
			case MCreature::CREATURE_GROUND :
				m_ppSector[y][x].UnSetBlockServerGround();
			break;

			// FLYING CREATURE
			case MCreature::CREATURE_FLYING :
				m_ppSector[y][x].UnSetBlockServerFlying();
			break;
		}
		*/
		MSector& sector = m_ppSector[y][x];
		sector.UnSetBlockServerUnderground();
		sector.UnSetBlockServerGround();
		sector.UnSetBlockServerFlying();
	}
}


//----------------------------------------------------------------------
// Move Ground Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
MZone::MoveGroundCreature(MCreature* pCreature, TYPE_SECTORPOSITION xo, TYPE_SECTORPOSITION yo, TYPE_SECTORPOSITION xn, TYPE_SECTORPOSITION yn)
{
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (xn<0 || yn<0 || xn>=m_Width || yn>=m_Height) return false;

	
	if (pCreature->GetClassType()==MCreature::CLASS_PLAYER
		
		|| pCreature->IsFakeCreature())
	{
		return true;
	}

	
	//if (xo==xn && yo==yn)
	//	return false;

	
	//MCreature* pExistCreature = NULL;

	int id = pCreature->GetID();

	//------------------------------------------------
	
	//------------------------------------------------
	if (!m_ppSector[yo][xo].RemoveCreature(id))
	{
		int xs = pCreature->GetServerX();
		int ys = pCreature->GetServerY();

		if (!m_ppSector[ys][xs].RemoveCreature(id))
		{	
			DEBUG_ADD_FORMAT("Can't Remove! [Ground] ID=%d (%d,%d) --> (%d,%d)", pCreature->GetID(), xo,yo,xn,yn);			
		}
	}

	//------------------------------------------------
	
	//------------------------------------------------
	if (!m_ppSector[yn][xn].AddGroundCreature( pCreature ))
	{
		DEBUG_ADD_FORMAT("Can't Add GroundCreature! ID=%d (%d,%d)", pCreature->GetID(), xn,yn);			
	}

	
	 

	return true;
}

//----------------------------------------------------------------------
// Move Flying Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
MZone::MoveFlyingCreature(MCreature* pCreature, TYPE_SECTORPOSITION xo, TYPE_SECTORPOSITION yo, TYPE_SECTORPOSITION xn, TYPE_SECTORPOSITION yn)
{
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (xn<0 || yn<0 || xn>=m_Width || yn>=m_Height) return false;

	
	if (pCreature->GetClassType()==MCreature::CLASS_PLAYER
		
		|| pCreature->IsFakeCreature())
	{
		return true;
	}

	
	//if (xo==xn && yo==yn)
	//	return false;

	//MCreature* pExistCreature = NULL;

	int id = pCreature->GetID();

	//------------------------------------------------
	
	//------------------------------------------------
	if (!m_ppSector[yo][xo].RemoveCreature(id))
	{
		int xs = pCreature->GetServerX();
		int ys = pCreature->GetServerY();

		if (!m_ppSector[ys][xs].RemoveCreature(id))
		{	
			DEBUG_ADD_FORMAT("Can't Remove! [Flying] ID=%d (%d,%d) --> (%d,%d)", pCreature->GetID(), xo,yo,xn,yn);			
		}
	}

	//------------------------------------------------
	
	//------------------------------------------------
	if (!m_ppSector[yn][xn].AddFlyingCreature( pCreature ))
	{
		DEBUG_ADD_FORMAT("Can't Add FlyingCreature! ID=%d (%d,%d)", pCreature->GetID(), xn,yn);			
	}

	 

	return true;
}

//----------------------------------------------------------------------
// Move Ungerground Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
MZone::MoveUndergroundCreature(MCreature* pCreature, TYPE_SECTORPOSITION xo, TYPE_SECTORPOSITION yo, TYPE_SECTORPOSITION xn, TYPE_SECTORPOSITION yn)
{
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (xn<0 || yn<0 || xn>=m_Width || yn>=m_Height) return false;

	
	if (pCreature->GetClassType()==MCreature::CLASS_PLAYER
		
		|| pCreature->IsFakeCreature())
	{
		return true;
	}

	
	//if (xo==xn && yo==yn)
	//	return false;

	
	//MCreature* pExistCreature = NULL;

	int id = pCreature->GetID();

	//------------------------------------------------
	
	//------------------------------------------------
	if (!m_ppSector[yo][xo].RemoveCreature(id))
	{
		int xs = pCreature->GetServerX();
		int ys = pCreature->GetServerY();

		if (!m_ppSector[ys][xs].RemoveCreature(id))
		{	
			DEBUG_ADD_FORMAT("Can't Remove! [Underground] ID=%d (%d,%d) --> (%d,%d)", pCreature->GetID(), xo,yo,xn,yn);			
		}
	}

	//------------------------------------------------
	
	//------------------------------------------------
	if (!m_ppSector[yn][xn].AddUndergroundCreature( pCreature ))
	{
		DEBUG_ADD_FORMAT("Can't Add UndergroundCreature! ID=%d (%d,%d)", pCreature->GetID(), xn,yn);			
	}

	 

	return true;
}

//----------------------------------------------------------------------
// Get CreatureID
//----------------------------------------------------------------------


//----------------------------------------------------------------------
TYPE_OBJECTID		
MZone::GetCreatureID(const char* pName, int flag) const
{
	CREATURE_MAP::const_iterator	iCreature = m_mapCreature.begin();

	MCreature* pCreature;

	//------------------------------------------------------
	
	//------------------------------------------------------
	while (iCreature != m_mapCreature.end())
	{
		pCreature = iCreature->second;

		
		if (pCreature->GetID()!=g_pPlayer->GetID()
			&& strcmp(pCreature->GetName(), pName)==0
			&& (flag == 0 || flag == 1 && !pCreature->IsNPC() )
			)
		{
			return pCreature->GetID();
		}

		iCreature++;
	}	

	return OBJECTID_NULL;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

TYPE_OBJECTID
MZone::GetCreatureID(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
{
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (x<0 || y<0 
		|| x>=m_Width || y>=m_Height) return OBJECTID_NULL;

	
	MCreature*	pCreature = m_ppSector[y][x].GetGroundCreature();

	
	if (pCreature != NULL)
	{			
		return pCreature->GetID();
	}

	return OBJECTID_NULL;

}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
MCreature*
MZone::GetCreatureBySector(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
{
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (x<0 || y<0 
		|| x>=m_Width || y>=m_Height) return NULL;

	
	MCreature*	pCreature = m_ppSector[y][x].GetGroundCreature();

	
	if (pCreature != NULL)
	{			
		return pCreature;
	}

	return NULL;

}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
TYPE_OBJECTID
MZone::GetFlyingCreatureID(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
{
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (x<0 || y<0 
		|| x>=m_Width || y>=m_Height) return OBJECTID_NULL;

	
	MCreature*	pCreature = (MCreature*)m_ppSector[y][x].GetFlyingCreature();

	
	if (pCreature != NULL)
	{			
		return pCreature->GetID();
	}

	return OBJECTID_NULL;

}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
TYPE_OBJECTID
MZone::GetUndergroundCreatureID(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
{
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (x<0 || y<0 
		|| x>=m_Width || y>=m_Height) return OBJECTID_NULL;

	
	MCreature*	pCreature = (MCreature*)m_ppSector[y][x].GetUndergroundCreature();

	
	if (pCreature != NULL)
	{			
		return pCreature->GetID();
	}

	return OBJECTID_NULL;

}


//----------------------------------------------------------------------
// Update
//----------------------------------------------------------------------
void
MZone::Update()
{
	//------------------------------------------
	
	//------------------------------------------
	UpdateAllCreature();

	//------------------------------------------
	
	//------------------------------------------
	UpdateFakeCreature();

	//------------------------------------------
	// UpdateItem
	//------------------------------------------
	UpdateItem();

	#ifdef OUTPUT_DEBUG_UPDATE_ZONE
		DEBUG_ADD("MZone::before Effect");
	#endif

	//------------------------------------------
	
	//------------------------------------------
	UpdateWaitEffects();
	UpdateEffects();
	UpdateGroundEffects();
	//------------------------------------------
	// Helicopter
	//------------------------------------------
	m_HelicopterManager.Update();

	//------------------------------------------
	
	//------------------------------------------
	UpdateSound();

	#ifdef OUTPUT_DEBUG_UPDATE_ZONE
		DEBUG_ADD("MZone::Update OK");
	#endif
}

//----------------------------------------------------------------------
// Move All Creature
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
MZone::UpdateAllCreature()
{
	CREATURE_MAP::iterator	iCreature = m_mapCreature.begin();

	MCreature* pCreature;

	//	DEBUG_ADD_FORMAT("[UpdateAllCreature] %d Creature(s)", m_mapCreature.size());

	static DWORD nextResetTime = g_CurrentTime + g_pUserOption->persnalShopupdatetime;
	bool bResetPersonalShopValue = false;
	
	if( g_CurrentTime > nextResetTime )
	{
		nextResetTime = g_CurrentTime+ g_pUserOption->persnalShopupdatetime;
		bResetPersonalShopValue = true;
	}	
	
	//------------------------------------------------------
	
	//------------------------------------------------------
	while (iCreature != m_mapCreature.end())
	{
		pCreature = ((*iCreature).second);

		#ifdef OUTPUT_DEBUG_UPDATE_ZONE
				DEBUG_ADD_FORMAT("[UpdateCreature] id=%d, cType=%d, x=%d, y=%d, action=%d", 
									(int)pCreature->GetID(), (int)pCreature->GetCreatureType(),
									(int)pCreature->GetX(), (int)pCreature->GetY(), (int)pCreature->GetAction());
		#endif

		if( pCreature->CurPernalShop() == 1 && bResetPersonalShopValue )
			pCreature->SetPersonalShopMsgTime( rand() & 0x7F );
		
		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		if (pCreature->GetClassType()==MCreature::CLASS_PLAYER)
		{
#ifdef __METROTECH_TEST__
//			for(int i = 0; i < abs(g_iSpeed)-1; i++)
//			{
//				g_pPlayer->Action();
////				g_pPlayer->UpdateAttachEffect();
//			}
#endif
			if(g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
				||g_pPlayer->HasEffectStatus( EFFECTSTATUS_DRAGON_EYES )
				)
				g_pPlayer->Action();
			
			g_pPlayer->Action();

			#ifdef OUTPUT_DEBUG_UPDATE_ZONE
				DEBUG_ADD_FORMAT("PlAok");
			#endif

			g_pPlayer->UpdateAttachEffect();

			#ifdef OUTPUT_DEBUG_UPDATE_ZONE
				DEBUG_ADD_FORMAT("PlEok");
			#endif			
		}
		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		else
		{
			int invisibleCount = pCreature->GetInvisibleCount();
			if(pCreature->HasEffectStatus( EFFECTSTATUS_DRAGON_EYES ))
				pCreature->Action();
			pCreature->Action();
			pCreature->UpdateAttachEffect();

			//------------------------------------------------------
			
			//------------------------------------------------------
			
			//------------------------------------------------------
			 

			//------------------------------------------------------
			
			//------------------------------------------------------
			if (pCreature->GetInvisibleCount()==64
				&& invisibleCount!=64)
			{
				//-------------------------------------------------------
				
				//
				
				
				//-------------------------------------------------------
				if (//pCreature->HasEffectStatus(EFFECTSTATUS_SNIPPING_MODE)|| 
					pCreature->HasEffectStatus(EFFECTSTATUS_INVISIBILITY)
						&& !g_pPlayer->IsVampire()
						&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_DETECT_INVISIBILITY)
						&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_OBSERVING_EYE)
 						&& !g_pPlayer->HasEffectStatus(EFFECTSTATUS_GNOME_WHISPER)
//						&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
						)
				{
					//-------------------------------------------------------
					
					//-------------------------------------------------------
					int id			= pCreature->GetID();
					int x			= pCreature->GetX();
					int y			= pCreature->GetY();
					int serverX		= pCreature->GetServerX();
					int serverY		= pCreature->GetServerY();

					//----------------------------------------
					
					//----------------------------------------
					if (pCreature->IsSlayer())
					{
						m_HelicopterManager.RemoveHelicopter( pCreature->GetID() );
					}				
					
					
					bool removed = true;
					
					//------------------------------------------------
					
					//------------------------------------------------
					if (!m_ppSector[y][x].RemoveCreature(id))
					{
						if (!m_ppSector[serverY][serverX].RemoveCreature(id))
						{	
							DEBUG_ADD_FORMAT("Can't RemoveCreature! ID=%d client(%d,%d), server(%d,%d)", pCreature->GetID(), x,y,serverX,serverY);

							removed = false;
						}
					}

					UnSetServerBlock( pCreature->GetMoveType(), pCreature->GetServerX(), pCreature->GetServerY() );

					
					delete pCreature;	

					
					CREATURE_MAP::iterator	iCreatureTemp = iCreature;
					iCreature++;

					m_mapCreature.erase( iCreatureTemp );			


					continue;
				}
			}

		}

		iCreature++;
	}

#ifdef __METROTECH_TEST__
	extern int	g_iSpeed;
	if(g_iSpeed < 0)
	{
		CREATURE_MAP::iterator itr = m_mapCreature.begin();
		CREATURE_MAP::iterator endItr = m_mapCreature.end();
		
		while(itr != endItr)
		{
			MCreature *pCreature = itr->second;
			if(pCreature->IsPlayer() && pCreature->GetID() != g_pPlayer->GetID() && !pCreature->IsPlayerParty())
				break;
			
			itr++;
		}
		
		if(itr == endItr)
		{
			g_iSpeed = -4;
		}
		else
		{
			g_iSpeed = -1;
		}
	}
#endif
	
	#ifdef OUTPUT_DEBUG_UPDATE_ZONE
		DEBUG_ADD("UpdateAllCreature OK");
	#endif
}

//----------------------------------------------------------------------
// Create Corpse  From Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
 

//----------------------------------------------------------------------
// Create Corpse  From Creature
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MZone::AddCorpseFromCreature(TYPE_OBJECTID id)
{
	DEBUG_ADD_FORMAT("Add Corpse From Creature id=%d", id);

	if (id==g_pPlayer->GetID())
	{
		DEBUG_ADD("No Player Corpse");
		return false;
	}

	CREATURE_MAP::iterator iCreature = m_mapCreature.find( id );

	if (iCreature==m_mapCreature.end())
	{
		DEBUG_ADD("No such creature");
		return false;
	}

	MCreature* pCreature = iCreature->second;

	UnSetServerBlock( pCreature->GetMoveType(), pCreature->GetServerX(), pCreature->GetServerY() );

	
	m_mapCreature.erase( iCreature );	
	
	int sX = pCreature->GetX();
	int sY = pCreature->GetY();

	//----------------------------------------
	
	//----------------------------------------
	if(pCreature->GetCreatureType() != 723) 
		pCreature->ClearChatString();

	//----------------------------------------
	
	//----------------------------------------
	
	//----------------------------------------
	bool removed = false;
	if (id != g_pPlayer->GetID()
		&& !pCreature->IsFakeCreature())
	{
		//------------------------------------------------
		
		//------------------------------------------------
		removed = true;
		if (!m_ppSector[sY][sX].RemoveCreature(id))
		{
			int serverX = pCreature->GetServerX();
			int serverY = pCreature->GetServerY();

			if (!m_ppSector[serverY][serverX].RemoveCreature(id))
			{	
				removed = false;
				DEBUG_ADD_FORMAT("Can't RemoveCreatureToCorpse! ID=%d client(%d,%d), server(%d,%d)", pCreature->GetID(), sX,sY,serverX,serverY);
			}			
		}
		

		/*
		removed = m_ppSector[sY][sX].RemoveGroundCreature( id );
		if (!removed)
		{
			removed = m_ppSector[serverY][serverX].RemoveGroundCreature( id );

			if (!removed)
			{
				removed = m_ppSector[sY][sX].RemoveFlyingCreature( id );

				if (!removed)
				{
					removed = m_ppSector[serverY][serverX].RemoveFlyingCreature( id );

					if (!removed)
					{
						removed = m_ppSector[sY][sX].RemoveUndergroundCreature( id );

						if (!removed)
						{
							removed = m_ppSector[serverY][serverX].RemoveUndergroundCreature( id );

							DEBUG_ADD_FORMAT("[Error] Can't Remove Creature to corpse. ID=%d. xy=(%d, %d), sxy=(%d, %d)", id, sX, sY, serverX, serverY);
						}
					}
				}
			}
		}
		*/
	}

	
	#ifdef OUTPUT_DEBUG
		if (!removed)
		{			
			DEBUG_ADD_FORMAT("[Create Corpse] Can't Remove Creature from Sector: %d, (%d,%d)", id, sX, sY);
		}
	#endif
	
	//----------------------------------------
	//
	
	//
	//----------------------------------------
	
	int sSX = pCreature->GetServerX();
	int sSY = pCreature->GetServerY();

	MCorpse* pCorpse = (MCorpse*)MItem::NewItem( ITEM_CLASS_CORPSE );
	pCorpse->SetID( id );
	pCorpse->SetItemType( 0 );
	pCorpse->SetCreature( pCreature );
	pCorpse->SetPosition( sSX, sSY );

	
	pCorpse->SetNumber( pCreature->GetItemCount() );


	//----------------------------------------	
	
	//----------------------------------------
	if (m_ppSector[sSY][sSX].IsExistItem())
	{
		MItem* pOldItem = m_ppSector[sSY][sSX].GetItem();
		
		#ifdef OUTPUT_DEBUG
			if (pOldItem!=NULL)
			{
				DEBUG_ADD_FORMAT("Already Exist Item. Remove Old Item: %d, (%d,%d)", pOldItem->GetID(), sSX, sSY);
			}
		#endif

		bool bRemove = RemoveItem( pOldItem->GetID() );

		#ifdef OUTPUT_DEBUG
			if (!bRemove)
			{
					DEBUG_ADD_FORMAT("Can't Remove Old Item");
			}
		#endif
	}

	//----------------------------------------
	
	//----------------------------------------
	#ifdef OUTPUT_DEBUG
		if (!AddItem( pCorpse ))
		{
			
			DEBUG_ADD_FORMAT("[Error] Can't Add Corpse to Sector id=%d (%d,%d)", pCorpse->GetID(), sSX, sSY);
		
			delete pCorpse;

			return false;
		}

		return true;
	#else
		return AddItem( pCorpse );
	#endif

}


//----------------------------------------------------------------------
// Add Portal
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void						
MZone::AddPortal(int type, int zoneID, const RECT& rect)
{
	int top = rect.top;
	int left = rect.left;
	int bottom = rect.bottom;
	int right = rect.right;

	if (top < 0) top = 0;
	if (left < 0) left = 0;
	if (bottom >= m_Height) bottom = m_Height-1;
	if (right >= m_Width) right = m_Width-1;

	if (top > bottom) { int temp=top; top=bottom; bottom=temp; }
	if (left > right) { int temp=left; left=right; right=left; }

	for (int i=top; i<=bottom; i++)
	{
		for (int j=left; j<=right; j++)
		{
			m_ppSector[i][j].AddPortal( type, zoneID );
		}
	}
}

//----------------------------------------------------------------------
// Keep Object In PlayerSight
//----------------------------------------------------------------------




//----------------------------------------------------------------------
void
MZone::KeepObjectInSight(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y, BYTE sight)
{	
//	int sight15 = sight + (sight>>1);

	//------------------------------------------------------
	
	//------------------------------------------------------
//	int minX = min( sight, VISION_SECTOR_WIDTH_HALF );
//	int minY = min( sight, VISION_SECTOR_HEIGHT_HALF );
//	int minY2 = min( sight, VISION_SECTOR_HEIGHT_HALF );//-2 );

	const int keepViewportWidth = 72;
	const int keepViewportHeight = 84;
	int minX = max(keepViewportWidth, (int)g_SECTOR_WIDTH + 20);
	int minY = max(keepViewportHeight, (int)g_SECTOR_HEIGHT + 24);
	int minY2 = max(keepViewportHeight, (int)g_SECTOR_HEIGHT + 24);

	int sX1 = x-minX;
	int sY1 = y-minY;
	int sX2 = x+minX;
	int sY2 = y+minY2;

	//------------------------------------------------------
	
	//------------------------------------------------------
	if (sX1 < 0) 
	{					
		sX1 = 0;	
	}

	if (sX2 >= m_Width)
	{
		sX2 = m_Width-1;
	}

	if (sY1 < 0)
	{
		sY1 = 0;	
	}

	if (sY2 >= m_Height)
	{
		sY2 = m_Height-1;
	}
	
	//------------------------------------------------------
	//
	
	//
	//------------------------------------------------------
	CREATURE_MAP::iterator	iCreature = m_mapCreature.begin();

	MCreature* pCreature;

	while (iCreature != m_mapCreature.end())
	{
		pCreature = iCreature->second;

		//-----------------------------------------------------
		
		//-----------------------------------------------------
		if (pCreature->GetClassType()==MCreature::CLASS_PLAYER || pCreature->GetID() < 10000)	
		{	
			iCreature++;
		}		
		else
		{
			//-----------------------------------------------------
			
			
			//-----------------------------------------------------			
			int cX = pCreature->GetServerX();
			int cY = pCreature->GetServerY();
//			int dist = abs(x - cX) + abs(y - cY);

			DEBUG_ADD_FORMAT("[KeepObjectInSight] %d : %d : %d , %d : %d : %d",
				sX1, cX, sX2, sY1, cY, sY2 );
			//-----------------------------------------------------			
			
			//-----------------------------------------------------			
			if (cX>=sX1 && cX<=sX2 &&
				cY>=sY1 && cY<=sY2 
//				&& dist <= sight15
				)
			{
				iCreature++;			
			}
			//-----------------------------------------------------			
			
			//-----------------------------------------------------			
			else
			{
				
				CREATURE_MAP::iterator iTemp = iCreature;

				
				iCreature++;

				DEBUG_ADD_FORMAT("[Remove Creature by Sight] id=%d xy=(%d, %d)", pCreature->GetID(), cX, cY);						
				
				MCreature* pCreature = (*iTemp).second;
				
				//----------------------------------------
				
				//----------------------------------------
				
				//----------------------------------------
				int id = pCreature->GetID();
				int x = pCreature->GetX();
				int y = pCreature->GetY();
				
				//------------------------------------------------
				
				//------------------------------------------------
				if (!m_ppSector[y][x].RemoveCreature(id))
				{
					if (!m_ppSector[cY][cX].RemoveCreature(id))
					{	
						DEBUG_ADD_FORMAT("Can't RemoveCreature! ID=%d client(%d,%d), server(%d,%d)", pCreature->GetID(), x,y,cX,cY);
					}
				}

				/*
				bool removed = m_ppSector[y][x].RemoveGroundCreature( id );
				if (!removed)
				{
					removed = m_ppSector[serverY][serverX].RemoveGroundCreature( id );

					if (!removed)
					{
						removed = m_ppSector[y][x].RemoveFlyingCreature( id );

						if (!removed)
						{
							removed = m_ppSector[serverY][serverX].RemoveFlyingCreature( id );

							if (!removed)
							{
								removed = m_ppSector[y][x].RemoveUndergroundCreature( id );

								if (!removed)
								{
									removed = m_ppSector[serverY][serverX].RemoveUndergroundCreature( id );

									DEBUG_ADD_FORMAT("[Error] Can't Remove Creature by sight. ID=%d. xy=(%d, %d), sxy=(%d, %d)", id, x, y, serverX, serverY);
								}
							}
						}
					}
				}
				*/

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				/*
				if (pCreature->IsPlayerParty()
					&& g_pParty!=NULL)
				{
					PARTY_INFO* pInfo = g_pParty->GetMemberInfo( pCreature->GetName() );
						
					if (pInfo!=NULL)
					{
						pInfo->bInSight = false;
						pInfo->zoneID = 60002;
					}
				}
				*/
				
				
				UnSetServerBlock( pCreature->GetMoveType(), pCreature->GetServerX(), pCreature->GetServerY() );

				delete pCreature;				
				
				m_mapCreature.erase(iTemp);			
			}			
		}		
	}

	//------------------------------------------------------
	//
	
	//
	//------------------------------------------------------
	ITEM_MAP::iterator	iItem = m_mapItem.begin();

	MItem* pItem;

	while (iItem != m_mapItem.end())
	{
		pItem = iItem->second;
	
		//-----------------------------------------------------
		
		
		//-----------------------------------------------------			
		int iX = pItem->GetX();
		int iY = pItem->GetY();
//		int dist = abs(x - iX) + abs(y - iY);

		//-----------------------------------------------------			
		
		//-----------------------------------------------------			
		if (iX>=sX1 && iX<=sX2 &&
			iY>=sY1 && iY<=sY2 
//			&& dist <= sight15
			)
		{
			iItem++;			
		}
		//-----------------------------------------------------			
		
		//-----------------------------------------------------			
		else
		{
			
			ITEM_MAP::iterator iTemp = iItem;

			
			iItem++;

			DEBUG_ADD_FORMAT("[Remove Item by Sight] id=%d xy=(%d, %d)", pItem->GetID(), iX, iY);					
				
			MItem* pItem = (*iTemp).second;

			
			MSector& sector = m_ppSector[iY][iX];				
			sector.RemoveItem( pItem->GetID() );

			
			if (IsRelicTable(pItem))
			{
				sector.UnSetBlockGround();
				sector.UnSetBlockFlying();
			}
				
			
			//delete pItem;				
			SAFE_DELETE ( pItem );
				
			m_mapItem.erase(iTemp);	
		}					
	}
	
	//------------------------------------------------------
	
	//------------------------------------------------------
	EFFECT_MAP::iterator	iEffect = m_mapEffect.begin();

	MEffect* pEffect;

	while (iEffect != m_mapEffect.end())
	{
		pEffect = iEffect->second;
	
		//-----------------------------------------------------
		
		
		//-----------------------------------------------------			
		int iX = pEffect->GetX();
		int iY = pEffect->GetY();
//		int dist = abs(x - iX) + abs(y - iY);

		//-----------------------------------------------------			
		
		//-----------------------------------------------------			
		if (iX>=sX1 && iX<=sX2 &&
			iY>=sY1 && iY<=sY2 
//			&& dist <= sight15
			)
		{
			iEffect++;			
		}
		//-----------------------------------------------------			
		
		//-----------------------------------------------------			
		else if (pEffect->GetEffectType()!=MEffect::EFFECT_CHASE &&
				(pEffect->GetFrameID() < EFFECTSPRITETYPE_MAP_BLACK_LARGE_SMOKE ||
				pEffect->GetFrameID() > EFFECTSPRITETYPE_MAP_BLACK_SMALL_SMOKE_3)&&
				pEffect->GetFrameID() != EFFECTSPRITETYPE_GDR_LAIR_POTAL)
		{
			
			EFFECT_MAP::iterator iTemp = iEffect;

			
			iEffect++;

			DEBUG_ADD_FORMAT("[Remove Effect by Sight] id=%d xy=(%d, %d)", pEffect->GetID(), iX, iY);					
				
			MEffect* pEffect = iTemp->second;

			
			RemoveSectorEffect( iX, iY, pEffect->GetID() );
				
			
			delete pEffect;				
				
			m_mapEffect.erase(iTemp);	
		}	
		else
		{
			iEffect++;
		}
	}
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------

//----------------------------------------------------------------------

bool		
MZone::AddCreature(MCreature* pCreature)
{
	DEBUG_ADD_FORMAT("MZone::AddCreature id = %d", pCreature->GetID());

	if (pCreature->GetX()==SECTORPOSITION_NULL
		|| pCreature->GetY()==SECTORPOSITION_NULL)
	{
		DEBUG_ADD_FORMAT("[Error] Position Not Set");

		return false;
	}

	CREATURE_MAP::iterator	theIterator;

	theIterator = m_mapCreature.find(pCreature->GetID());
	
	
	if (theIterator == m_mapCreature.end())
	{		
		
		if (g_pParty!=NULL 
			&& pCreature->GetName()!=NULL
			&& g_pParty->HasMember( pCreature->GetName() )
			&& !pCreature->IsNPC() )
		{
			pCreature->SetPlayerParty();
				
			//-------------------------------------------------------
			
			//-------------------------------------------------------
			PARTY_INFO* pInfo = g_pParty->GetMemberInfo( pCreature->GetName() );
				
			if (pInfo!=NULL)
			{
				pInfo->bInSight = true;
				pInfo->HP = pCreature->GetHP();
				pInfo->MaxHP = pCreature->GetMAX_HP();
				pInfo->guildID = pCreature->GetGuildNumber();
			}			
		}

		//----------------------------------------
		
		//----------------------------------------
		if (g_pPlayer->HasEffectStatus(EFFECTSTATUS_HALLUCINATION))
		{
			pCreature->SetHalluCreature( g_pTopView->GetRandomMonsterTypeInZone() );
		}

		//----------------------------------------
		
		//----------------------------------------
		if (pCreature->IsUndergroundCreature())
		{
			
		}
		else
		{
			if ((*g_pCreatureTable)[pCreature->GetCreatureType()].IsFlyingCreature())
			{
				pCreature->SetFlyingCreature();
			}			
			else
			{
				pCreature->SetGroundCreature();
			}
		}

		bool bAdd;
		//----------------------------------------
		// Sector Setting
		//----------------------------------------
		
		//----------------------------------------
		if (pCreature->GetClassType()==MCreature::CLASS_PLAYER)
		{
			//m_mapCreature.insert(CREATURE_MAP::value_type(pCreature->GetID(), pCreature));
			bAdd = true;
		}
		//----------------------------------------
		
		//----------------------------------------
		else
		{			
			int x = pCreature->GetX();
			int y = pCreature->GetY();

			switch (pCreature->GetMoveType())
			{
				case MCreature::CREATURE_UNDERGROUND :
					bAdd = m_ppSector[y][x].AddUndergroundCreature(pCreature);

					//if (!bAdd)
					{
					//	m_ppSector[pCreature->GetY()][pCreature->GetX()].RemoveUndergroundCreature();
					//	bAdd = m_ppSector[pCreature->GetY()][pCreature->GetX()].AddUndergroundCreature(pCreature);
					}
				break;

				case MCreature::CREATURE_GROUND :
					bAdd = m_ppSector[y][x].AddGroundCreature(pCreature);		

					//if (!bAdd)
					{
					//	m_ppSector[pCreature->GetY()][pCreature->GetX()].RemoveGroundCreature();
					//	bAdd = m_ppSector[pCreature->GetY()][pCreature->GetX()].AddGroundCreature(pCreature);		
					}
				break;

				case MCreature::CREATURE_FLYING :
					bAdd = m_ppSector[y][x].AddFlyingCreature(pCreature);		

					//if (!bAdd)
					{
					//	m_ppSector[pCreature->GetY()][pCreature->GetX()].RemoveFlyingCreature();
					//	bAdd = m_ppSector[pCreature->GetY()][pCreature->GetX()].AddFlyingCreature(pCreature);		
					}
				break;
			}
		}

		if (bAdd)
		{
			m_mapCreature.insert(CREATURE_MAP::value_type(pCreature->GetID(), pCreature));

			pCreature->SetZone( this );

			return true;
		}		

		DEBUG_ADD_FORMAT("[Error] AddCreature Failed!");

		return false;
	}

	
	DEBUG_ADD_FORMAT("Add Failed - Already Exist in Zone");

	return false;
}

	

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MZone::RemoveCreature(TYPE_OBJECTID id)
{
	DEBUG_ADD_FORMAT("Remove Creature. ID=%d", id);

	CREATURE_MAP::iterator	theIterator;

	
	theIterator = m_mapCreature.find(id);
    
	
	if (theIterator == m_mapCreature.end())
		return false;


	MCreature* pCreature = (*theIterator).second;

	
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	/*
	if (pCreature->IsPlayerParty()
		&& g_pParty!=NULL)
	{
		PARTY_INFO* pInfo = g_pParty->GetMemberInfo( pCreature->GetName() );
			
		if (pInfo!=NULL)
		{
			pInfo->bInSight = false;
			pInfo->zoneID = 60002;
		}
	}
	*/
	
	//----------------------------------------
	
	//----------------------------------------
	
	//----------------------------------------
	if (pCreature==NULL || pCreature->GetClassType()==MCreature::CLASS_PLAYER)
	{
		m_mapCreature.erase(theIterator);
		
		return true;
	}
	
	//----------------------------------------
	
	//----------------------------------------
	if (pCreature->IsSlayer())
	{
		m_HelicopterManager.RemoveHelicopter( id );
	}

	//----------------------------------------
	
	//----------------------------------------	
	int x			= pCreature->GetX();
	int y			= pCreature->GetY();
	int serverX		= pCreature->GetServerX();
	int serverY		= pCreature->GetServerY();

	
	/*
	switch (pCreature->GetMoveType())
	{
		case MCreature::CREATURE_GROUND :					
			removed = m_ppSector[y][x].RemoveGroundCreature( creatureID );
			if (!removed)
			{
				removed = m_ppSector[serverY][serverX].RemoveGroundCreature( creatureID );
			}
		break;

		case MCreature::CREATURE_FLYING :
			removed = m_ppSector[y][x].RemoveFlyingCreature( creatureID );
			if (!removed)
			{
				removed = m_ppSector[serverY][serverX].RemoveFlyingCreature( creatureID );
			}
		break;

		case MCreature::CREATURE_UNDERGROUND :
			removed = m_ppSector[y][x].RemoveUndergroundCreature( creatureID );
			if (!removed)
			{
				removed = m_ppSector[serverY][serverX].RemoveUndergroundCreature( creatureID );
			}
		break;
	}
	*/

	
	bool removed = true;
	
	//------------------------------------------------
	
	//------------------------------------------------
	if (!m_ppSector[y][x].RemoveCreature(id))
	{
		if (!m_ppSector[serverY][serverX].RemoveCreature(id))
		{	
			DEBUG_ADD_FORMAT("Can't RemoveCreature! ID=%d client(%d,%d), server(%d,%d)", pCreature->GetID(), x,y,serverX,serverY);

			removed = false;
		}
	}

	/*
	removed = m_ppSector[y][x].RemoveGroundCreature( id );
	if (!removed)
	{
		removed = m_ppSector[serverY][serverX].RemoveGroundCreature( id );

		if (!removed)
		{
			removed = m_ppSector[y][x].RemoveFlyingCreature( id );

			if (!removed)
			{
				removed = m_ppSector[serverY][serverX].RemoveFlyingCreature( id );

				if (!removed)
				{
					removed = m_ppSector[y][x].RemoveUndergroundCreature( id );

					if (!removed)
					{
						removed = m_ppSector[serverY][serverX].RemoveUndergroundCreature( id );

						DEBUG_ADD_FORMAT("[Error] Can't Remove Creature. ID=%d. xy=(%d, %d), sxy=(%d, %d)", id, x, y, serverX, serverY);
					}
				}
			}
		}
	}
	*/

	
	UnSetServerBlock( pCreature->GetMoveType(), pCreature->GetServerX(), pCreature->GetServerY() );


	
	if (removed)
	{
		//UnSetServerBlock( pCreature->GetMoveType(), pCreature->GetServerX(), pCreature->GetServerY() );

		delete pCreature;

		m_mapCreature.erase(theIterator);
	}			

	return removed;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
MCreature*	
MZone::GetCreature(TYPE_OBJECTID id)
{
	CREATURE_MAP::iterator	theIterator;

	
	theIterator = m_mapCreature.find(id);

	
	if (theIterator == m_mapCreature.end()) 
	{
		ITEM_MAP::iterator iItem = m_mapItem.find(id);

		if (iItem==m_mapItem.end())
		{
			return NULL;
		}

		MItem* pItem = iItem->second;

		if (pItem!=NULL
			&& pItem->GetItemClass()==ITEM_CLASS_CORPSE)
		{
			MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();

			if (pCreature!=NULL
				&& pCreature->GetActionCount() < pCreature->GetActionCountMax())
			{
				return pCreature;
			}
		}
		
		return NULL;
	}

	
	return (*theIterator).second;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
MCreature*	
MZone::GetCreatureOnly(TYPE_OBJECTID id)
{
	CREATURE_MAP::iterator	theIterator;

	
	theIterator = m_mapCreature.find(id);

	
	if (theIterator == m_mapCreature.end()) 
	{
		return NULL;
	}

	
	return theIterator->second;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool		
MZone::AddImageObject(MImageObject* pImageObject)
{
	if(pImageObject != NULL)
	{
		IMAGEOBJECT_MAP::iterator	theIterator;

		theIterator = m_mapImageObject.find(pImageObject->GetID());
		
		
		if (theIterator == m_mapImageObject.end())
		{
			m_mapImageObject.insert(IMAGEOBJECT_MAP::value_type(pImageObject->GetID(), pImageObject));

			return true;
		}

		
	}
	return false;
}

	

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MZone::RemoveImageObject(TYPE_OBJECTID	id)
{
	IMAGEOBJECT_MAP::iterator	theIterator;

	
	theIterator = m_mapImageObject.find(id);
    
	
	if (theIterator == m_mapImageObject.end())
		return false;


	
	MImageObject* pImageObject = (*theIterator).second;	


	//???????????????????????????????????????????????????????
	//
	
	//
	//???????????????????????????????????????????????????????

	m_mapImageObject.erase(theIterator);

	
	delete pImageObject;	

	return true;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
MImageObject*	
MZone::GetImageObject(TYPE_OBJECTID id)
{
	IMAGEOBJECT_MAP::iterator	theIterator;

	
	theIterator = m_mapImageObject.find(id);

	
	if (theIterator == m_mapImageObject.end()) 
		return NULL;

	
	return (*theIterator).second;
}

//----------------------------------------------------------------------
// SetImageObjectSector(sX,sY)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
MZone::SetImageObjectSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_OBJECTID id)
{
	IMAGEOBJECT_MAP::iterator	theIterator;

	// Guard against corrupted/out-of-range coordinates (WASM traps on OOB).
	if (sX >= m_Width || sY >= m_Height)
	{
#ifdef __EMSCRIPTEN__
		static int s_badPosCount = 0;
		if (s_badPosCount < 5)
		{
			printf("WARN: ImageObject position out of bounds: (%u,%u) size=%ux%u\n",
				(unsigned)sX, (unsigned)sY, (unsigned)m_Width, (unsigned)m_Height);
			s_badPosCount++;
		}
#endif
		return;
	}

	
	theIterator = m_mapImageObject.find(id);

	
	if (theIterator == m_mapImageObject.end()) 
		return;
	
	
	m_ppSector[sY][sX].AddImageObject((*theIterator).second);
}

//----------------------------------------------------------------------
// UnSetImageObjectSector(sX,sY)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
MZone::UnSetImageObjectSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_OBJECTID id)
{
	if (sX >= m_Width || sY >= m_Height)
	{
		return;
	}

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	m_ppSector[sY][sX].RemoveImageObject(id);
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------
 

//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------


//----------------------------------------------------------------------
//bool		
//MZone::RemoveObstacle(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
//{
	 
	//return false;
//}


//----------------------------------------------------------------------
// Change Frame Obstacle
//----------------------------------------------------------------------

//----------------------------------------------------------------------
/*
void
MZone::ChangeFrameObstacle()
{
	OBSTACLE_LIST::iterator iObstacle = m_listObstacle.begin();

	MObstacle* pObstacle;
	while (iObstacle != m_listObstacle.end())
	{
		pObstacle = *iObstacle;

		pObstacle->NextFrame();

		iObstacle++;
	}
	
}
*/

//----------------------------------------------------------------------

//----------------------------------------------------------------------
TYPE_OBJECTID
MZone::GetItemID(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
{
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (//x<0 || y<0 || 
		x>=m_Width || y>=m_Height) return OBJECTID_NULL;

	
	MItem*	pItem = m_ppSector[y][x].GetItem();

	
	if (pItem != NULL)
	{			
		return pItem->GetID();
	}

	return OBJECTID_NULL;

	
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool		
MZone::AddItem(MItem* pItem, BOOL bDropping)
{
	//--------------------------------------------------------	
	
	//--------------------------------------------------------	
	if (pItem->GetItemClass()!=ITEM_CLASS_MOTORCYCLE)	
	{
		if (bDropping 
			&& pItem->GetDropFrameID()!=FRAMEID_NULL)				
		{
			pItem->SetDropping();
		}
		else
		{
			//--------------------------------------------------------	
			
			//--------------------------------------------------------	
			//PlaySound( pItem->GetTileSoundID(),
			//					false,
			//					g_pPlayer->GetX(), g_pPlayer->GetY());
		}
	}

	//--------------------------------------------------------	
	
	//--------------------------------------------------------	
	ITEM_MAP::iterator	theIterator;

	theIterator = m_mapItem.find(pItem->GetID());
	
	
	if (theIterator == m_mapItem.end())
	{
		
		MSector& sector = m_ppSector[pItem->GetY()][pItem->GetX()];

		if (sector.AddItem(pItem))
		{		
			m_mapItem.insert(ITEM_MAP::value_type(pItem->GetID(), pItem));

			
			if (IsRelicTable(pItem))
			{
				sector.SetBlockGround();
				sector.SetBlockFlying();
			}

			bool bSpecialItem = false;

			
			if(pItem->GetItemClass() == ITEM_CLASS_BLOOD_BIBLE)
			{
				bSpecialItem = true;
				ExecuteActionInfoFromMainNode(
					ARMEGA_TILE+pItem->GetItemType(),		
					
					pItem->GetX(), pItem->GetY(), 0,
					DIRECTION_DOWN,
					
					OBJECTID_NULL,
					pItem->GetX(), pItem->GetY(), 0,
					
					0xFFFF,	
					
					NULL
					);
			}

			
			if(pItem->GetItemClass() == ITEM_CLASS_CASTLE_SYMBOL)
			{
				int TempEffetType[6] = {	AMBER_OF_GUARD_TILE,
											AMBER_OF_AVENGER_TILE,
											AMBER_OF_IMMORTAL_TILE,
											AMBER_OF_CURSE_TILE,
											AMBER_OF_LIFE_TILE,
											AMBER_OF_LIGHT_TILE,
										};
				int TempItemType = pItem->GetItemType();

				if(TempItemType >5 ) TempItemType = 0;
				bSpecialItem = true;

				ExecuteActionInfoFromMainNode(
					TempEffetType[TempItemType],		
					
					pItem->GetX(), pItem->GetY(), 0,
					DIRECTION_DOWN,
					
					OBJECTID_NULL,
					pItem->GetX(), pItem->GetY(), 0,
					
					0xFFFF,	
					
					NULL
					);
			}

			if(bSpecialItem)
			{
				ExecuteActionInfoFromMainNode(
					DROP_BLOOD_BIBLE,		
					
					pItem->GetX(), pItem->GetY(), 0,
					DIRECTION_DOWN,
					
					OBJECTID_NULL,
					pItem->GetX(), pItem->GetY(), 0,
					
					0xFFFF,	
					
					NULL
					);
			}
			
			
			if(pItem->GetItemClass() == ITEM_CLASS_WAR_ITEM)
			{
				int TempItemType = pItem->GetItemType();
				if(0 == TempItemType) 
				{
					ExecuteActionInfoFromMainNode(
						SKILL_CLIENT_DRAGON_EYES,		
						
						pItem->GetX(), pItem->GetY(), 0,
						DIRECTION_DOWN,
						
						OBJECTID_NULL,
						pItem->GetX(), pItem->GetY(), 0,
						
						0xFFFF,	
						
						NULL
						);
				}
			}
			

			return true;
		}

		DEBUG_ADD_FORMAT("[Error] Already Exist Item id=%d, xy(%d,%d), cl=%d, ty=%d",
								pItem->GetID(), 
								pItem->GetX(), pItem->GetY(), 
								(int)pItem->GetItemClass(), 
								(int)pItem->GetItemType());
		

		return false;
	}

	
	return false;
}
	

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MZone::RemoveItem(TYPE_OBJECTID id)
{
	DEBUG_ADD_FORMAT("Remove Item. ID=%d", id);
	
	ITEM_MAP::iterator	theIterator;

	ITEM_MAP::iterator tempItr = m_mapItem.begin();
	TYPE_OBJECTID idd;
	while( tempItr != m_mapItem.end() )
	{
		 idd = (*tempItr).first;
		tempItr++;
	}

	
	theIterator = m_mapItem.find(id);
    
	
	if (theIterator == m_mapItem.end())
		return false;


	

	
	MItem* pItem = NULL;
	

	int sector_y = (*theIterator).second->GetY(), sector_x = (*theIterator).second->GetX();
	MSector& sector = m_ppSector[sector_y][sector_x];
	bool removed = sector.RemoveItem(pItem);

	if (removed && pItem!=NULL)
	{
		
		if (IsRelicTable(pItem))
		{
			sector.UnSetBlockGround();
			sector.UnSetBlockFlying();
		}
		bool bSpecialItem = false;
		
		
		if(pItem->GetItemClass() == ITEM_CLASS_BLOOD_BIBLE)
		{
			bSpecialItem = true;
						
			if (!g_pZone->RemoveTileEffect( sector_x,sector_y,   (TYPE_EFFECTSPRITETYPE)(EFFECTSPRITETYPE_ARMEGA_TILE + pItem->GetItemType()) ) )
			{
				DEBUG_ADD("[Error] Can't Remove Effect From Tile (ARMEGA_TILE HardCoding)");
			}
		}
		
		
		if(pItem->GetItemClass() == ITEM_CLASS_WAR_ITEM)
		{
			if(0 == pItem->GetItemType())
			{
				if (!g_pZone->RemoveTileEffect( sector_x,sector_y,   (TYPE_EFFECTSPRITETYPE)(EFFECTSPRITETYPE_DRAGON_EYE)) )
				{
					DEBUG_ADD("[Error] Can't Remove Effect From Tile (ITEM_CLASS_WAR_ITEM)");
				}
			}
		}
		// 2004, 10, 25, sobeit add end
		
		if(pItem->GetItemClass() == ITEM_CLASS_CASTLE_SYMBOL)
		{
			bSpecialItem = true;

			int TempEffetType[6] = {	EFFECTSPRITETYPE_AMBER_OF_GUARD_TILE,
										EFFECTSPRITETYPE_AMBER_OF_AVENGER_TILE,
										EFFECTSPRITETYPE_AMBER_OF_IMMORTAL_TILE,
										EFFECTSPRITETYPE_AMBER_OF_CURSE_TILE,
										EFFECTSPRITETYPE_AMBER_OF_LIFE,
										EFFECTSPRITETYPE_AMBER_OF_LIGHT,
										};
				int TempItemType = pItem->GetItemType();
				if(TempItemType >5 ) TempItemType = 0;
			if (!g_pZone->RemoveTileEffect( sector_x,sector_y,  (TYPE_EFFECTSPRITETYPE)(TempEffetType[TempItemType]) ) )
			{
				DEBUG_ADD("[Error] Can't Remove Effect From Tile (AMBER_OF_GUARD_TILE HardCoding)");
			}
		}
		
		if(bSpecialItem)
		{
			if (!g_pZone->RemoveTileEffect( sector_x,sector_y,   (TYPE_EFFECTSPRITETYPE)EFFECTSPRITETYPE_DROP_BLOOD_BIBLE) )
			{
				DEBUG_ADD("[Error] Can't Remove Effect From Tile (DropBloodBible HardCoding)");
			}
		}

		m_mapItem.erase(theIterator);

		//------------------------------------------------------
		
		//------------------------------------------------------
		MItem* pCheckItem = g_pPlayer->GetItemCheckBuffer();

		if (pCheckItem!=NULL)
		{
			DEBUG_ADD_FORMAT("Remove Item. ID=%d", id);

			if (pItem->GetID()==pCheckItem->GetID())
			{
				//---------------------------------------------
				
				//---------------------------------------------
				g_pPlayer->ClearItemCheckBuffer();
			}
		}
		else
		{
			DEBUG_ADD("Check Item is NULL");
		}

		
		//delete pItem;	
		SAFE_DELETE ( pItem );
	}

	return removed;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MZone::PickupItem(TYPE_OBJECTID id)
{
	ITEM_MAP::iterator	theIterator;

	
	theIterator = m_mapItem.find(id);
    
	
	if (theIterator == m_mapItem.end())
		return false;


	

	
	MItem* pItem = NULL;
	int sector_y = (*theIterator).second->GetY(), sector_x = (*theIterator).second->GetX();
	bool removed = m_ppSector[sector_y][sector_x].RemoveItem(pItem);

	bool bSpecialItem = false;

	if(pItem != NULL && removed)
	{
		
		if(pItem->GetItemClass() == ITEM_CLASS_BLOOD_BIBLE)
		{
			bSpecialItem = true;
						
			if (!g_pZone->RemoveTileEffect( sector_x,sector_y,   (TYPE_EFFECTSPRITETYPE)(EFFECTSPRITETYPE_ARMEGA_TILE + pItem->GetItemType() )) )
			{
				DEBUG_ADD("[Error] Can't Remove Effect From Tile (ARMEGA_TILE HardCoding)");
			}
		}
		
		
		if(pItem->GetItemClass() == ITEM_CLASS_WAR_ITEM)
		{
			if(0 == pItem->GetItemType())
			{
				if (!g_pZone->RemoveTileEffect( sector_x,sector_y,   (TYPE_EFFECTSPRITETYPE)(EFFECTSPRITETYPE_DRAGON_EYE)) )
				{
					DEBUG_ADD("[Error] Can't Remove Effect From Tile (ITEM_CLASS_WAR_ITEM)");
				}
			}
		}
		// 2004, 10, 25, sobeit add end
		
		
		if(pItem->GetItemClass() == ITEM_CLASS_CASTLE_SYMBOL)
		{
			bSpecialItem = true;

			int TempEffetType[6] = {	EFFECTSPRITETYPE_AMBER_OF_GUARD_TILE,
									EFFECTSPRITETYPE_AMBER_OF_AVENGER_TILE,
									EFFECTSPRITETYPE_AMBER_OF_IMMORTAL_TILE,
									EFFECTSPRITETYPE_AMBER_OF_CURSE_TILE,
									EFFECTSPRITETYPE_AMBER_OF_LIFE,
									EFFECTSPRITETYPE_AMBER_OF_LIGHT,
									};
			int TempItemType = pItem->GetItemType();
			if(TempItemType >5 ) TempItemType = 0;
			if (!g_pZone->RemoveTileEffect( sector_x,sector_y,  (TYPE_EFFECTSPRITETYPE)(TempEffetType[TempItemType] )) )
			{
				DEBUG_ADD("[Error] Can't Remove Effect From Tile (AMBER_OF_GUARD_TILE HardCoding)");
			}
		}
		
		if(bSpecialItem)
		{
			if (!g_pZone->RemoveTileEffect( sector_x,sector_y,   (TYPE_EFFECTSPRITETYPE)EFFECTSPRITETYPE_DROP_BLOOD_BIBLE) )
			{
				DEBUG_ADD("[Error] Can't Remove Effect From Tile (DropBloodBible HardCoding)");
			}
		}
	}


	if (removed)
	{
		m_mapItem.erase(theIterator);
	}

	return removed;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*	
MZone::GetItem(TYPE_OBJECTID id)
{
	ITEM_MAP::iterator	theIterator;

	
	theIterator = m_mapItem.find(id);

	
	if (theIterator == m_mapItem.end()) 
		return NULL;

	
	return (*theIterator).second;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool
MZone::AddEffect(MEffect* pNewEffect, DWORD dwWaitCount)
{
	DEBUG_ADD("AddEffect");

	// Validate pNewEffect pointer before use
	// This helps catch use-after-free errors earlier
	if (pNewEffect == NULL) {
		DEBUG_ADD_ERR("AddEffect: NULL pNewEffect pointer!");
		return false;
	}

	// Check for obviously invalid pointers (wild pointers)
	// This catches cases where pointer points to unmapped or redzone memory
	uintptr_t ptr_value = (uintptr_t)pNewEffect;
	if (ptr_value < 0x1000) {  // NULL or near-NULL pointer
		DEBUG_ADD_ERR("AddEffect: Invalid pNewEffect pointer (near NULL)!");
		return false;
	}

	// Try to safely validate the object
	// We use __builtin_expect to hint that validation usually succeeds
	if (__builtin_expect(pNewEffect != NULL, 1)) {
		// Log the pointer for debugging
		DEBUG_ADD_FORMAT("AddEffect: pNewEffect=%p, dwWaitCount=%u", pNewEffect, dwWaitCount);

		// NOTE: We don't try to dereference here because the object might be corrupted
		// ASAN will catch actual use-after-free errors
	}

	if(dwWaitCount)
	{
		m_listWaitEffect.push_back( pNewEffect );
		return true;
	}

	// Use try-catch for any dereference that might fail
	int x, y;
	TYPE_FRAMEID frameID;

	// Safely get position and frame ID with ASAN-friendly checks
	// These dereferences might fail if pNewEffect is corrupted
	// Use ASAN's __asan_region_is_poisoned to check if pointer is valid
	// This helps detect use-after-free before dereferencing
	#ifdef __SANITIZE_ADDRESS__
	void* ptr_test = (void*)pNewEffect;
	if (__asan_region_is_poisoned(ptr_test, sizeof(MEffect))) {
		DEBUG_ADD_ERR("AddEffect: pNewEffect points to poisoned memory!");
		DEBUG_ADD_FORMAT("AddEffect: pNewEffect=%p is in freed region", pNewEffect);
		// Don't delete the effect since it's already freed
		return false;
	}
	#endif

	try {
		x = pNewEffect->GetX();
		y = pNewEffect->GetY();

		// Get FrameID - this is where the crash happens (line 3938)
		frameID = pNewEffect->GetFrameID();

		DEBUG_ADD_FORMAT("AddEffect: Effect at (%d,%d), FrameID=%d", x, y, frameID);
	} catch (...) {
		DEBUG_ADD_ERR("AddEffect: Exception when accessing pNewEffect members!");
		// Don't delete the effect since we can't trust the pointer
		return false;
	}

	// Use cached frameID to avoid multiple GetFrameID() calls
	// This prevents multiple potential dereferences of a corrupted pointer

	// SAFETY CHECK: Verify g_pEffectSpriteTypeTable is valid before use
	// This protects against use-after-free where the table might have been corrupted
	if (g_pEffectSpriteTypeTable == NULL) {
		DEBUG_ADD_ERR("AddEffect: g_pEffectSpriteTypeTable is NULL!");
		delete pNewEffect;
		return false;
	}

	// Validate the table pointer hasn't been corrupted (ASAN builds only)
	#ifdef __SANITIZE_ADDRESS__
	validate_effect_sprite_table_pointer("MZone::AddEffect (entry)");
	#endif

	// ROOT CAUSE FIX: Check if m_pTypeInfo is NULL (happens after Release() but before delete)
	// This can occur if there's a use-after-free of the table object
	if (g_pEffectSpriteTypeTable->GetInternalPointer() == NULL) {
		DEBUG_ADD_ERR("AddEffect: g_pEffectSpriteTypeTable->m_pTypeInfo is NULL! Table object may have been released.");
		delete pNewEffect;
		return false;
	}

	// Check if pointer looks obviously invalid (points to near-NULL address)
	uintptr_t table_ptr = (uintptr_t)g_pEffectSpriteTypeTable;
	if (table_ptr < 0x1000) {
		DEBUG_ADD_FORMAT("AddEffect: g_pEffectSpriteTypeTable has invalid pointer=%p", g_pEffectSpriteTypeTable);
		delete pNewEffect;
		return false;
	}

	// ASAN check: verify g_pEffectSpriteTypeTable pointer is not poisoned
	#ifdef __SANITIZE_ADDRESS__
	if (__asan_address_is_poisoned((void*)g_pEffectSpriteTypeTable, sizeof(void*))) {
		DEBUG_ADD_ERR("AddEffect: g_pEffectSpriteTypeTable pointer is poisoned!");
		DEBUG_ADD_FORMAT("AddEffect: g_pEffectSpriteTypeTable=%p points to freed memory", g_pEffectSpriteTypeTable);
		delete pNewEffect;
		return false;
	}
	#endif

	DEBUG_ADD_FORMAT("AddEffect: g_pEffectSpriteTypeTable=%p, checking frameID=%d", g_pEffectSpriteTypeTable, frameID);

	// Validate that the global table pointer hasn't been corrupted
	#ifdef __SANITIZE_ADDRESS__
	validate_effect_sprite_table_pointer("MZone::AddEffect");
	#endif

	// CRITICAL: Try to safely detect if g_pEffectSpriteTypeTable points to corrupted memory
	// Use a volatile read to avoid compiler optimization and catch SIGSEGV
	volatile bool table_valid = true;
	TYPE_FRAMEID test_frame_id = 0;

	// Use signal handler to safely test if we can read from the table
	// This is a best-effort check - might still crash if memory is truly corrupted
	#if defined(__GNUC__) || defined(__clang__)
		if (__builtin_expect(!table_valid, 0)) {
			// This should never execute, but prevents optimization
		}
	#endif

	// Check if the pointer address looks like heap memory (freed regions)
	// If it points to the range where SDL surfaces were allocated, it's corrupted
	uintptr_t table_addr = (uintptr_t)g_pEffectSpriteTypeTable;
	if (table_addr >= 0x632000000000ULL && table_addr <= 0x6320000FFFFFFULL) {
		// This looks like the SDL surface region that gets freed!
		DEBUG_ADD_ERR("AddEffect: g_pEffectSpriteTypeTable points to SDL surface region!");
		DEBUG_ADD_FORMAT("AddEffect: table_addr=%p is in freed SDL memory range", g_pEffectSpriteTypeTable);
		delete pNewEffect;
		return false;
	}

	// Attempt to safely read the first element to catch corruption early
	// This might crash, but ASAN should give us a better stack trace
	if (table_valid) {
		// Force a volatile read to prevent optimization
		test_frame_id = (*g_pEffectSpriteTypeTable)[0].FrameID;
		(void)test_frame_id; // Suppress unused warning
	}

	// Safe access with boundary checking
	int tableSize = g_pEffectSpriteTypeTable->GetSize();
	bool bDarkNess = false;
	bool bAcidSwamp = false;
	bool bProminence = false;

	if (EFFECTSPRITETYPE_DARKNESS_1_1 < tableSize &&
	    EFFECTSPRITETYPE_DARKNESS_3_5 < tableSize &&
	    EFFECTSPRITETYPE_GRAY_DARKNESS_1_1 < tableSize &&
	    EFFECTSPRITETYPE_GRAY_DARKNESS_3_5 < tableSize) {
		bDarkNess = frameID >= (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_DARKNESS_1_1].FrameID &&
			frameID <= (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_DARKNESS_3_5].FrameID ||
			frameID >= (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_GRAY_DARKNESS_1_1].FrameID &&
			frameID <= (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_GRAY_DARKNESS_3_5].FrameID;
	}

	if (EFFECTSPRITETYPE_ACID_SWAP_1 < tableSize &&
	    EFFECTSPRITETYPE_ACID_SWAP_3 < tableSize) {
		bAcidSwamp = (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_ACID_SWAP_1].FrameID <= frameID &&
			(*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_ACID_SWAP_3].FrameID >= frameID;
	}

	if (EFFECTSPRITETYPE_PROMINENCE_ING < tableSize &&
	    EFFECTSPRITETYPE_PROMINENCE_START < tableSize &&
	    EFFECTSPRITETYPE_PROMINENCE2_ING < tableSize &&
	    EFFECTSPRITETYPE_PROMINENCE3_START < tableSize) {
		bProminence = (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_PROMINENCE_ING].FrameID <= frameID &&
			(*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_PROMINENCE_START].FrameID >= frameID ||
			(*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_PROMINENCE2_ING].FrameID <= frameID &&
			(*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_PROMINENCE3_START].FrameID >= frameID;
	}


	if(bDarkNess)
	{
		const MSector& sector= m_ppSector[y][x];
		if(sector.HasDarknessForbidden() == true )
		{
			delete pNewEffect;
			return false;
		}
	}
	
	if( bDarkNess )
	{
		if( !( x < 0 || x >= GetWidth() || y < 0 || y >=GetHeight() ) )
		{
			const MSector& sector= m_ppSector[y][x];
			const MItem* pItem = sector.GetItem();
			
			
			if( pItem != NULL && pItem->GetItemClass() == ITEM_CLASS_CORPSE &&
				((MCorpse*)pItem)->GetCreature() != NULL &&
				((MCorpse*)pItem)->GetCreature()->GetCreatureType() == 670 )
			{
				delete pNewEffect;
				return false;
			}
		}		
	}

	
	if( bDarkNess || bAcidSwamp || bProminence )
	{
		EFFECT_MAP::iterator iGroundEffect = m_mapGroundEffect.begin();
		
		while (iGroundEffect != m_mapGroundEffect.end())
		{
			MEffect* pEffect = iGroundEffect->second;
			
			MEffectTarget* pEffectTarget = pEffect->GetEffectTarget();
			TYPE_FRAMEID	frameID_s = (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_MERCY_GROUND_1].FrameID;
			TYPE_FRAMEID	frameID_e = (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_MERCY_GROUND_9].FrameID;
			TYPE_FRAMEID	regenTowerTile = (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_REGEN_TOWER_GROUND].FrameID;
			TYPE_FRAMEID	summonClay = (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_SUMMON_CLAY_LOOP].FrameID;
			
			
			if( pEffect->GetFrameID() >= frameID_s && pEffect->GetFrameID() <= frameID_e )
			{
				int fix_y =   ( pEffect->GetFrameID() - frameID_s) /3 -1;
				int fix_x = ( ( pEffect->GetFrameID() - frameID_s) %3) -1;

				if( pEffect->GetX() + fix_x == x &&	pEffect->GetY() + fix_y == y )
				{
					delete pNewEffect;
					return false;
				}
			} 
			else if( pEffect->GetFrameID() == regenTowerTile && x == pEffect->GetX() && y == pEffect->GetY() )
			{
				delete pNewEffect;
				return false;
			}

// 			else if( pEffect->GetFrameID() == summonClay && x == pEffect->GetX() && y == pEffect->GetY() )
// 			{
// 				delete pNewEffect;
// 				return false;
// 			}
			
			iGroundEffect++;
		}
	}
		

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (x<0 || x>=m_Width
		|| y<0 || y>=m_Height)
	{
		DEBUG_ADD("OuterZone");

		
		if (pNewEffect->GetEffectType()==MEffect::EFFECT_CHASE)
		{
			TYPE_OBJECTID id = pNewEffect->GetID();

			
			if (m_mapEffect.find( id )==m_mapEffect.end())
			{
				
				m_mapEffect[id] = pNewEffect;				

				DEBUG_ADD("ChaseEffect");

				return true;
			}
		}

		DEBUG_ADD("deleteEff");

		delete pNewEffect;
		
		return false;
	}

	MSector &sector = m_ppSector[y][x];

	//---------------------------------------------------------
	
	//
	
	//---------------------------------------------------------
	BOOL AddOK = TRUE;

	if (pNewEffect->GetEffectType()==MEffect::EFFECT_SECTOR && !pNewEffect->IsMulti())
	{
		DEBUG_ADD("getSector");

		
		if (sector.IsExistEffect())
		{
			EFFECT_LIST::const_iterator iEffect = sector.GetEffectIterator();
			
			int num = sector.GetEffectSize();

			DEBUG_ADD_FORMAT("secEffNum=%d", num);

			for (int i=0; i<num; i++)
			{
				MEffect* pEffect = *iEffect;

				//---------------------------------------------------------
				
				
				//---------------------------------------------------------
				
				if (pEffect->GetEffectType()==MEffect::EFFECT_SECTOR)
				{
					int fid1 = pNewEffect->GetFrameID();
					int fid2 = pEffect->GetFrameID();

					int est1 = g_pTopView->GetEffectSpriteType( (BLT_TYPE)pNewEffect->GetBltType(), fid1 );
					int est2 = g_pTopView->GetEffectSpriteType( (BLT_TYPE)pEffect->GetBltType(), fid2 );
					

//					if(est1 == EFFECTSPRITETYPE_SPIT_STREAM ||
//						est1 == EFFECTSPRITETYPE_GREAT_RUFFIAN_2_AXE_THROW ||
//						est1 == EFFECTSPRITETYPE_GREAT_RUFFIAN_2_AXE_THROW_SHADOW ||
//						est1 == EFFECTSPRITETYPE_GREAT_RUFFIAN_1_AXE_GROUND ||
//						est1 == EFFECTSPRITETYPE_GREAT_RUFFIAN_1_AXE_WAVE
//						)
//					{

//					}
//					else
//					// 2004, 10, 19, sobeit add end
					if (fid1 == fid2)
					{
						
						if (pNewEffect->GetEndFrame() > pEffect->GetEndFrame() &&
							est1 != EFFECTSPRITETYPE_DRAGON_FIRE_CRACKER) 
						{
							int count = pNewEffect->GetEndFrame() - g_CurrentFrame;
							
							pEffect->SetCount( count );
						}

						
						AddOK = FALSE;
						
						if( est1 ==	EFFECTSPRITETYPE_DRAGON_FIRE_CRACKER )
							AddOK = TRUE;

						break;
					}
					
					else
					{					
						if (
							( est1>=EFFECTSPRITETYPE_DARKNESS_1_1
							&& est1<=EFFECTSPRITETYPE_DARKNESS_3_5
							&& est2>=EFFECTSPRITETYPE_DARKNESS_1_1
							&& est2<=EFFECTSPRITETYPE_DARKNESS_3_5

							&& (est1>=EFFECTSPRITETYPE_DARKNESS_2_1
								&& est1<=EFFECTSPRITETYPE_DARKNESS_2_5
								&& est2>=EFFECTSPRITETYPE_DARKNESS_2_1
								&& est2<=EFFECTSPRITETYPE_DARKNESS_2_5
								|| est1>=EFFECTSPRITETYPE_DARKNESS_1_1
								&& est1<=EFFECTSPRITETYPE_DARKNESS_1_5
								&& est2>=EFFECTSPRITETYPE_DARKNESS_1_1
								&& est2<=EFFECTSPRITETYPE_DARKNESS_1_5
								|| est1>=EFFECTSPRITETYPE_DARKNESS_3_1
								&& est1<=EFFECTSPRITETYPE_DARKNESS_3_5
								&& est2>=EFFECTSPRITETYPE_DARKNESS_3_1
								&& est2<=EFFECTSPRITETYPE_DARKNESS_3_5) 
								)||

							(	est1>=EFFECTSPRITETYPE_GRAY_DARKNESS_1_1
							&& est1<=EFFECTSPRITETYPE_GRAY_DARKNESS_3_5
							&& est2>=EFFECTSPRITETYPE_GRAY_DARKNESS_1_1
							&& est2<=EFFECTSPRITETYPE_GRAY_DARKNESS_3_5

							&& (est1>=EFFECTSPRITETYPE_GRAY_DARKNESS_2_1
								&& est1<=EFFECTSPRITETYPE_GRAY_DARKNESS_2_5
								&& est2>=EFFECTSPRITETYPE_GRAY_DARKNESS_2_1
								&& est2<=EFFECTSPRITETYPE_GRAY_DARKNESS_2_5
								|| est1>=EFFECTSPRITETYPE_GRAY_DARKNESS_1_1
								&& est1<=EFFECTSPRITETYPE_GRAY_DARKNESS_1_5
								&& est2>=EFFECTSPRITETYPE_GRAY_DARKNESS_1_1
								&& est2<=EFFECTSPRITETYPE_GRAY_DARKNESS_1_5
								|| est1>=EFFECTSPRITETYPE_GRAY_DARKNESS_3_1
								&& est1<=EFFECTSPRITETYPE_GRAY_DARKNESS_3_5
								&& est2>=EFFECTSPRITETYPE_GRAY_DARKNESS_3_1
								&& est2<=EFFECTSPRITETYPE_GRAY_DARKNESS_3_5)
								)								
							)
						{
							
							if (pNewEffect->GetEndFrame() > pEffect->GetEndFrame()) 
							{
								int count = pNewEffect->GetEndFrame() - g_CurrentFrame;
								
								pEffect->SetCount( count );
							}

							
							AddOK = FALSE;

							
							if (est1>=EFFECTSPRITETYPE_DARKNESS_2_1 &&
								est1<=EFFECTSPRITETYPE_DARKNESS_2_5 ||
								est1>=EFFECTSPRITETYPE_GRAY_DARKNESS_2_1 &&
								est1<=EFFECTSPRITETYPE_GRAY_DARKNESS_2_5)
							{
								sector.SetDarkness();
							}

						
							CheckCreatureInDarkness( sector, x, y );

							break;
						}	
							
						else if (est1>=EFFECTSPRITETYPE_SANCTUARY_1
								&& est1<=EFFECTSPRITETYPE_SANCTUARY_3)
						{
							
							for (int i=-1; i<=1; i++)
							{							
								for (int j=-1; j<=1; j++)
								{	
									int x0 = x + i;
									int y0 = y + j;

									if (x0<0 || x0>=m_Width
										|| y0<0 || y0>=m_Height)
									{
										continue;
									}

									MSector& sector2 = m_ppSector[y0][x0];
									sector2.SetSanctuary();
								}
							}
						}

					}
				}

				iEffect++;
			}
		}
	}
			
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (AddOK)
	{
		DEBUG_ADD("addOK");

		TYPE_OBJECTID id = pNewEffect->GetID();

		
		if (m_mapEffect.find( id )==m_mapEffect.end())
		{
			
			m_mapEffect[id] = pNewEffect;

			
			MSector& sector = m_ppSector[y][x];

			DEBUG_ADD("secAddEff");
			sector.AddEffect( pNewEffect );

			DEBUG_ADD("chkDark");
			CheckCreatureInDarkness( sector, x, y );
			DEBUG_ADD("chkDarkOK");

			return true;
		}
	}

	
//	if (pEffect->GetBltType()==BLT_EFFECT)
//	{
//		SetLight(pEffect->GetX(), pEffect->GetY(), pEffect->GetLight());		
//	}

	DEBUG_ADD("delEff");
	delete pNewEffect;

	DEBUG_ADD("reF");
	return false;
}

//----------------------------------------------------------------------
// Get Effect
//----------------------------------------------------------------------
MEffect*	
MZone::GetEffect(TYPE_OBJECTID id) const
{
	EFFECT_MAP::const_iterator iEffect = m_mapEffect.find( id );

	if (iEffect==m_mapEffect.end())
	{
		return NULL;
	}

	return iEffect->second;
}

//----------------------------------------------------------------------
// Remove Effect ( id )
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool		
MZone::RemoveEffect(TYPE_OBJECTID id)
{
	EFFECT_MAP::iterator iEffect = m_mapEffect.find( id );

	if (iEffect==m_mapEffect.end())
	{
		return false;
	}

	MEffect* pEffect = iEffect->second;

	int x = pEffect->GetX();
	int y = pEffect->GetY();

	
	if (x>=0 && x<m_Width
		&& y>=0 && y<m_Height)
	{
		MSector& sector = m_ppSector[y][x];
		
		
		RemoveSectorEffect( x, y, pEffect->GetID() );

		CheckCreatureInDarkness( sector, x, y );
	}

	delete pEffect;

	m_mapEffect.erase( iEffect );

	return true;
}

//----------------------------------------------------------------------
// Remove Tile Effect
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
MZone::RemoveTileEffect(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, int effectStatus, int serverID)
{
	if (effectStatus >= g_pEffectStatusTable->GetSize())
	{
		DEBUG_ADD_FORMAT("[Error]EffectStatus is NULL or Over: %d", effectStatus);

		return false;
	}

	TYPE_EFFECTSPRITETYPE type = (*g_pEffectStatusTable)[effectStatus].EffectSpriteType;

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	switch (effectStatus)
	{
		case EFFECTSTATUS_TRYING_POSITION :
		case EFFECTSTATUS_TRAP_INSTALLED :
		{
			bool bDelete = false;
			
			EFFECT_MAP::iterator iGroundEffect = m_mapGroundEffect.begin();
			
			while (iGroundEffect != m_mapGroundEffect.end())
			{
				MEffect* pEffect = iGroundEffect->second;
				
				MEffectTarget* pEffectTarget = pEffect->GetEffectTarget();
				TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[type].FrameID;

				if( pEffect->GetFrameID() == frameID &&
					pEffect->GetX() == sX &&
					pEffect->GetY() == sY )
				{
					delete pEffect;
					m_mapGroundEffect.erase( iGroundEffect );
					break;
				}
				
				iGroundEffect++;
			}
			
			DEBUG_ADD("EffectSpriteType of EffectStatus is NULL");
			
			return bDelete;
		}
		break;

		//----------------------------------------------------------------
		
		//----------------------------------------------------------------		
		case EFFECTSTATUS_VAMPIRE_PORTAL :
		{
			bool bDelete = false;

			EFFECT_MAP::iterator iGroundEffect = m_mapGroundEffect.begin();

			while (iGroundEffect != m_mapGroundEffect.end())
			{
				MEffect* pEffect = iGroundEffect->second;

				MEffectTarget* pEffectTarget = pEffect->GetEffectTarget();

				if (pEffectTarget!=NULL)
				{
					//------------------------------------------------------------------
					
					//------------------------------------------------------------------
					if (pEffectTarget->GetEffectTargetType()==MEffectTarget::EFFECT_TARGET_PORTAL)
					{
						MPortalEffectTarget* pPortalEffectTarget = (MPortalEffectTarget*)pEffectTarget;

						int portalServerID = pPortalEffectTarget->GetServerID();
						
						if (serverID==portalServerID)
						{
							delete pEffect;

							m_mapGroundEffect.erase( iGroundEffect );

							break;
						}
					}
				}

				iGroundEffect++;
			}

			DEBUG_ADD("EffectSpriteType of EffectStatus is NULL");

			return bDelete;
		}
		break;	// - -;
		
		case EFFECTSTATUS_GROUND_ELEMENTAL_AURA:
			if( g_pZone != NULL )
			{
				const_cast<MSector &>(g_pZone->GetSector(sX, sY)).UnSetGroundElemental();
			}
			break;
		
		case EFFECTSTATUS_DARKNESS_FORBIDDEN:
			if( g_pZone != NULL )
			{
				const_cast<MSector &>(g_pZone->GetSector(sX, sY)).UnSetDarknessForbidden();
			}
			break;
			
		case EFFECTSTATUS_FURY_OF_GNOME:
			if( g_pZone != NULL )
			{
				const_cast<MSector &>(g_pZone->GetSector(sX, sY)).UnSetFuryOfGnome();
			}
			break;	
		case EFFECTSTATUS_DRAGON_TORNADO:
		case EFFECTSTATUS_DRAGON_TORNADO_CHILD:
			{
				return RemoveFakeCreature(serverID);
			}
			
		case EFFECTSTATUS_DELETE_TILE:
			if( g_pZone != NULL )
			{
				const_cast<MSector &>(g_pZone->GetSector(sX, sY)).UnSetDisableTileImage();
				if(g_pTopView != NULL)
					g_pTopView->SetFirstDraw();
			}
			break;	
			return true;
	}


	return RemoveTileEffect(sX, sY, type, serverID);

}



//----------------------------------------------------------------------
// Remove Tile Effect
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
MZone::RemoveTileEffect(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_EFFECTSPRITETYPE type, int serverID)
{
	if (type==EFFECTSPRITETYPE_NULL)
	{
		DEBUG_ADD("[Error]EffectSpriteType of EffectStatus is NULL");

		return false;
	}

	TYPE_FRAMEID	frameID = (*g_pEffectSpriteTypeTable)[type].FrameID;

	if (frameID==FRAMEID_NULL)
	{
		DEBUG_ADD("[Error]FrameID of EffectSpriteType is NULL");

		return false;
	}


	MSector& sector = m_ppSector[sY][sX];

	//--------------------------------------------------
	
	//--------------------------------------------------
	if (sector.IsExistEffect())
	{
		int num = sector.GetEffectSize();

		bool isRemoveDarknessEffect = (frameID>=EFFECTSPRITETYPE_DARKNESS_1_1
								&& frameID<=EFFECTSPRITETYPE_DARKNESS_3_5) ||
								(frameID>=EFFECTSPRITETYPE_GRAY_DARKNESS_1_1 
								&& frameID <= EFFECTSPRITETYPE_GRAY_DARKNESS_3_5 
//								||	frameID >= EFFECTSPRITETYPE_MAP_BLACK_LARGE_SMOKE &&
//								frameID <= EFFECTSPRITETYPE_MAP_BLACK_SMALL_SMOKE_3
								);

		EFFECT_LIST::const_iterator iEffect = sector.GetEffectIterator();

		for (int i=0; i<num; i++)
		{
			MEffect* pEffect = *iEffect;

			//--------------------------------------------------
			
			//--------------------------------------------------
			if (pEffect->GetEffectType() == MEffect::EFFECT_SECTOR)
			{
				bool isExistDarknessEffect = ((pEffect->GetFrameID()>=EFFECTSPRITETYPE_DARKNESS_1_1
											&& pEffect->GetFrameID()<=EFFECTSPRITETYPE_DARKNESS_3_5) ||
											pEffect->GetFrameID() >= EFFECTSPRITETYPE_GRAY_DARKNESS_1_1 &&
											pEffect->GetFrameID() <= EFFECTSPRITETYPE_GRAY_DARKNESS_3_5 
//											||	pEffect->GetFrameID() <= EFFECTSPRITETYPE_MAP_BLACK_LARGE_SMOKE &&
//											pEffect->GetFrameID() >= EFFECTSPRITETYPE_MAP_BLACK_SMALL_SMOKE_3;
											);
				//--------------------------------------------------
				
				//--------------------------------------------------
				if (pEffect->GetFrameID() == frameID

					
					|| isRemoveDarknessEffect
					&& isExistDarknessEffect					
					)
				{
					
					RemoveSectorEffect( sX, sY, pEffect->GetID() );

					
					//--------------------------------------------------
					
					//--------------------------------------------------
					// delete
					EFFECT_MAP::iterator iZoneEffect = m_mapEffect.begin();

					BOOL found = FALSE;
					while (iZoneEffect != m_mapEffect.end())
					{
						MEffect* pZoneEffect = iZoneEffect->second;

						
						if (pZoneEffect->GetID()==pEffect->GetID())
						{
							m_mapEffect.erase( iZoneEffect );

							found = TRUE;

							break;
						}

						iZoneEffect++;
					}

					if (!found)
					{
						EFFECT_MAP::iterator iGroundEffect = m_mapGroundEffect.begin();

						BOOL found = FALSE;
						while (iGroundEffect != m_mapGroundEffect.end())
						{
							MEffect* pGroundEffect = iGroundEffect->second;

							
							if (pGroundEffect->GetID()==pEffect->GetID())
							{
								m_mapGroundEffect.erase( iGroundEffect );

								found = TRUE;

								break;
							}

							iGroundEffect++;
						}
					}

					
					if (isRemoveDarknessEffect)
					{
						CheckCreatureInDarkness( sector, sX, sY );
					}

					delete pEffect;
					
					return true;
				}
			}

			iEffect++;
		}
	}

	DEBUG_ADD("EffectSpriteType of EffectStatus is NULL");
	
	return false;
}


//----------------------------------------------------------------------
// Change Frame Effect
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MZone::UpdateEffects()
{
	EFFECT_MAP::iterator iEffect = m_mapEffect.begin();

	EFFECT_MAP::iterator iTemp;
	
	MEffect* pEffect;
	int count = m_mapEffect.size();


	int		sX, sY;		
	int		nX, nY;		
	int		light;
	int		id;
	
//	bool bChangeSight;

	for (int i=0; i<count; i++)	
	{
		pEffect = iEffect->second;

		if(pEffect->IsDelayFrame() == true)
		{
			iEffect++;
			continue;
		}

		
		id		= pEffect->GetID();
		sX		= pEffect->GetX();
		sY		= pEffect->GetY();
		light	= pEffect->GetLight();

		
//		bChangeSight = pEffect->GetBltType()==BLT_EFFECT;

		BOOL sXYinZone = (sX>=0 && sX<m_Width && sY>=0 && sY<m_Height);
		
		//---------------------------------------
		//
		
		//
		//---------------------------------------
		if (pEffect->Update())
		{
			nX = pEffect->GetX();
			nY = pEffect->GetY();

			BOOL nXYinZone = (nX>=0 && nX<m_Width && nY>=0 && nY<m_Height);

			//---------------------------------------
			
			
			//---------------------------------------
			if (pEffect->GetEffectType()!=MEffect::EFFECT_CHASE
//				&& (pEffect->GetFrameID() <= EFFECTSPRITETYPE_MAP_BLACK_LARGE_SMOKE ||
//					pEffect->GetFrameID() >= EFFECTSPRITETYPE_MAP_BLACK_SMALL_SMOKE_3)
				&& !nXYinZone)
			{
				
//				if (bChangeSight)
//				{
//					UnSetLight(sX, sY, light);
//				}

				if (sXYinZone)
				{
					
					RemoveSectorEffect( sX, sY, id );
				}
										
				#ifdef OUTPUT_DEBUG_UPDATE_EFFECT
					DEBUG_ADD("delete pEffect0");
					
					delete pEffect;			
					DEBUG_ADD("delete OK");
				#else
					delete pEffect;
				#endif
				
				iTemp = iEffect;
				iEffect++;

				
				m_mapEffect.erase(iTemp);	

				continue;
			}

			//-----------------------------------------------------------
			
			//-----------------------------------------------------------
			
			//-----------------------------------------------------------
			if (pEffect->GetEffectType() == MEffect::EFFECT_SECTOR)
			{
				
//				if (bChangeSight && light!=pEffect->GetLight())
//				{
//					UnSetLight(sX, sY, light);
//					SetLight(sX, sY, pEffect->GetLight());
//				}				
			}
			//-----------------------------------------------------------
			
			//-----------------------------------------------------------
			else
			{
				
				
				//case MEffect::EFFECT_LINEAR :
				//case MEffect::EFFECT_PARABOLA :
				//case MEffect::EFFECT_GUIDANCE :
				//case MEffect::EFFECT_ATTACH :

				//------------------------------------------------------
				
				//------------------------------------------------------
				if (sX!=nX || sY!=nY)
				{
					//------------------------------------------------------
					
					//------------------------------------------------------
					//
					
					//
					
					if (0)
						
						
						//
						//pEffect->GetEffectType()!=MEffect::EFFECT_CHASE
						//&& nXYinZone && m_ppSector[nY][nX].IsBlockFlying())						
					{
						
//						if (bChangeSight)
//						{
//							UnSetLight(sX, sY, light);
//						}

						
						if (sXYinZone)
						{
							
							RemoveSectorEffect(sX, sY, id);
						}
													
						
						#ifdef OUTPUT_DEBUG_UPDATE_EFFECT
							DEBUG_ADD("delete pEffect1");
							
							delete pEffect;			
							DEBUG_ADD("delete OK");
						#else
							delete pEffect;
						#endif
						
						iTemp = iEffect;
						iEffect++;

						
						m_mapEffect.erase(iTemp);	

						continue;
					}

					
//					if (bChangeSight)
//					{
//						UnSetLight(sX, sY, light);
//						SetLight(pEffect->GetX(), pEffect->GetY(), pEffect->GetLight());
//					}

					
					if (sXYinZone)
					{
						
						RemoveSectorEffect(sX, sY, id);
					}

					
					if (nXYinZone)
					{
						m_ppSector[nY][nX].AddEffect( pEffect );
					}
				}		
				//------------------------------------------------------
				
				//------------------------------------------------------
				else if (light != pEffect->GetLight())
				{
					
//					if (bChangeSight)
//					{
//						UnSetLight(sX, sY, light);
//						SetLight(sX, sY, pEffect->GetLight());
//					}
				}
			}

			//-----------------------------------------------
			//
			
			
			//
			
			//
			//-----------------------------------------------
			if (g_CurrentFrame >= pEffect->GetEndLinkFrame()
				&& pEffect->GetLinkSize() != 0)
			{
				
				
				
				#ifdef OUTPUT_DEBUG_UPDATE_EFFECT
					DEBUG_ADD("GenerateNext1");
					g_pEffectGeneratorTable->GenerateNext( pEffect );
					DEBUG_ADD("GenerateNext1 OK");
				#else
					g_pEffectGeneratorTable->GenerateNext( pEffect );
				#endif

				
			}
			
			
			iEffect++;
		}
		//---------------------------------------
		//
		
		//
		//---------------------------------------
		else
		{
			//-----------------------------------------------
			//
			
			//
			//-----------------------------------------------
			
			//if (bChangeSight)
			//{
				//UnSetLight(sX, sY, light);
			//}			
			//-----------------------------------------------
			//
			
			//
			//-----------------------------------------------
			if (pEffect->GetLinkSize() != 0)
			{
				#ifdef OUTPUT_DEBUG_UPDATE_EFFECT
					DEBUG_ADD("GenerateNext2");
					g_pEffectGeneratorTable->GenerateNext( pEffect );
					DEBUG_ADD("GenerateNext2 OK");
				#else
					g_pEffectGeneratorTable->GenerateNext( pEffect );
				#endif
			}		
			
			
			if (sXYinZone)
			{
				MSector& sector = m_ppSector[sY][sX];
				
				
				RemoveSectorEffect( sX, sY, id );

				CheckCreatureInDarkness( sector, sX, sY );				
			}
							
			#ifdef OUTPUT_DEBUG_UPDATE_EFFECT
				DEBUG_ADD("delete pEffect2");
				
				delete pEffect;			
				DEBUG_ADD("delete OK");
			#else
				delete pEffect;
			#endif

			iTemp = iEffect;
			iEffect++;

			
			m_mapEffect.erase(iTemp);	
		}		
	}	
}



//----------------------------------------------------------------------

//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool		
MZone::AddGroundEffect(MEffect* pEffect)
{	
	TYPE_OBJECTID id = pEffect->GetID();
	BOOL	AddOK = TRUE;
	
	if( m_mapGroundEffect.empty() )
		AddOK = TRUE;
	else
	{
		EFFECT_MAP::iterator Itr = m_mapGroundEffect.begin();
		EFFECT_MAP::iterator endItr = m_mapGroundEffect.end();
		
		int NewEffectX = pEffect->GetX();
		int NewEffectY = pEffect->GetY();
		
		while( Itr != endItr )
		{
			MEffect *pOldEffect = Itr->second;			
			
			if(NewEffectX == pOldEffect->GetX() && NewEffectY == pOldEffect->GetY())
			{
				int est1 = g_pTopView->GetEffectSpriteType( (BLT_TYPE) pEffect->GetBltType(), pEffect->GetFrameID() );
				int est2 = g_pTopView->GetEffectSpriteType( (BLT_TYPE) pOldEffect->GetBltType(), pOldEffect->GetFrameID() );

				if( est1 == EFFECTSPRITETYPE_SUMMON_GROUND_ELEMENTAL_END && est2 == EFFECTSPRITETYPE_SUMMON_GROUND_ELEMENTAL_ING)
				{
					pOldEffect->SetCount( 0 );
				}

				if( est1 == EFFECTSPRITETYPE_MAGIC_ELUSION_ING && est2 == EFFECTSPRITETYPE_MAGIC_ELUSION_ING ||
					est1 >= EFFECTSPRITETYPE_MERCY_GROUND_1 && est1 <= EFFECTSPRITETYPE_MERCY_GROUND_9 &&
					est1 == est2)
				{
					if( pEffect->GetEndFrame() > pOldEffect->GetEndFrame() )
					{
						int count = pEffect->GetEndFrame() - g_CurrentFrame;
						pOldEffect->SetCount( count );
					}
					AddOK = FALSE;
				}
				else if ( est2 == EFFECTSPRITETYPE_REGEN_TOWER_GROUND && 
					(est1 == EFFECTSPRITETYPE_ICE_FIELD_2 ||
					est1 == EFFECTSPRITETYPE_ICE_FIELD_1 ||
					est1 == EFFECTSPRITETYPE_ICE_FIELD_3
					)
					)
				{
					AddOK = FALSE;
				}
// 				else if ( est1 == EFFECTSPRITETYPE_SUMMON_CLAY_LOOP || 
// 						  est1 == EFFECTSPRITETYPE_SUMMON_CLAY_START ||
// 						  est1 == EFFECTSPRITETYPE_SUMMON_CLAY_END)
// 				{
// 					AddOK = FALSE;
// 				}
			}
			if(AddOK == FALSE)
				break;			
			Itr++;
		}
	}
	if(AddOK == TRUE)
	{
		m_mapGroundEffect[id] = pEffect;
	}		
		 

	return true;
//	return false;
}

//----------------------------------------------------------------------
// Get GroundEffect
//----------------------------------------------------------------------
MEffect*	
MZone::GetGroundEffect(TYPE_OBJECTID id) const
{
	EFFECT_MAP::const_iterator iEffect = m_mapGroundEffect.find( id );

	if (iEffect==m_mapGroundEffect.end())
	{
		return NULL;
	}

	return iEffect->second;
}

//----------------------------------------------------------------------
// Remove GroundEffect ( id )
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool		
MZone::RemoveGroundEffect(TYPE_OBJECTID id)
{
	EFFECT_MAP::iterator iEffect = m_mapGroundEffect.find( id );

	if (iEffect==m_mapGroundEffect.end())
	{
		return false;
	}

	MEffect* pEffect = iEffect->second;

	 

	delete pEffect;

	m_mapGroundEffect.erase( iEffect );

	return true;
}


//----------------------------------------------------------------------
// Update GroundEffects
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MZone::UpdateGroundEffects()
{
	EFFECT_MAP::iterator iEffect = m_mapGroundEffect.begin();

	EFFECT_MAP::iterator iTemp;
	
	MEffect* pEffect;
	int count = m_mapGroundEffect.size();


	int		light;
	int		id;
	
	for (int i=0; i<count; i++)	
	{
		pEffect = iEffect->second;

		
		id		= pEffect->GetID();
		light	= pEffect->GetLight();

		//---------------------------------------
		//
		
		//
		//---------------------------------------
		if (pEffect->Update())
		{
			//-----------------------------------------------
			//
			
			
			//
			
			//
			//-----------------------------------------------
			if (g_CurrentFrame >= pEffect->GetEndLinkFrame()
				&& pEffect->GetLinkSize() != 0)
			{
				
				
				
				g_pEffectGeneratorTable->GenerateNext( pEffect );

				
			}

			
			iEffect++;
		}
		//---------------------------------------
		//
		
		//
		//---------------------------------------
		else
		{
			//-----------------------------------------------
			//
			
			//
			//-----------------------------------------------
			if (pEffect->GetLinkSize() != 0)
			{
				g_pEffectGeneratorTable->GenerateNext( pEffect );
			}				
			
			//---------------------------------------------------------
			
			//---------------------------------------------------------
			 
												
			
			delete pEffect;			

			iTemp = iEffect;
			iEffect++;

			
			m_mapGroundEffect.erase(iTemp);	
		}		
	}	
}

//----------------------------------------------------------------------
// Get Near SpriteSet
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void						
MZone::GetNearSpriteSet(CSpriteSetManager& TileSSM, CSpriteSetManager& ImageObjectSSM, TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY) const
{
	//----------------------------------------------------------------------
	//
	
	//
	//----------------------------------------------------------------------	
	POINT firstSector;
	firstSector.x = sX + g_SECTOR_SKIP_PLAYER_LEFT*3;
	firstSector.y = sY + g_SECTOR_SKIP_PLAYER_UP*3;
	int sX1 = firstSector.x + SECTOR_SKIP_LEFT*3;
	int sY1 = firstSector.y + SECTOR_SKIP_UP*3;
	int sX2 = firstSector.x + g_SECTOR_WIDTH*3;
	int sY2 = firstSector.y + g_SECTOR_HEIGHT*3;
	
	//------------------------------------------------------
	
	//------------------------------------------------------
	if (sX1 < 0) 
	{					
		//tilePoint.x += -sX1 * TILE_X;
		sX1 = 0;	
	}

	if (sX2 >= m_Width)
	{
		sX2 = m_Width-1;
	}

	if (sY1 < 0)
	{
		//tilePoint.y += -sY1 * TILE_Y;
		sY1 = 0;	
	}

	if (sY2 >= m_Height)
	{
		sY2 = m_Height-1;
	}
		
	DEBUG_ADD_FORMAT("GetNearSpriteSet: (%d, %d) ~ (%d, %d)", sX1, sY1, sX2, sY2);

	//------------------------------------------------------
	
	//------------------------------------------------------
	int y,x,i;

	int maxTile = g_pTopView->m_TileSPK.GetSize();

	for (y=sY1; y<=sY2; y++)
	{				
		for (x=sX1; x<=sX2; x++)
		{				
			const MSector& sector = m_ppSector[y][x];

			//--------------------------------------------
			
			//--------------------------------------------
			int spriteID = sector.GetSpriteID();

			if (spriteID < maxTile)
			{
				TileSSM.Add( spriteID );
			}

			//--------------------------------------------
			
			//--------------------------------------------
			if (sector.IsExistImageObject())
			{
				OBJECT_MAP::const_iterator iImageObject = sector.GetImageObjectIterator();

				//--------------------------------------------				
				
				
				//--------------------------------------------
				for (i=0; i<sector.GetImageObjectSize(); i++)
				{
					MImageObject* const pImageObject = (MImageObject* const)((*iImageObject).second);
					
					if (pImageObject->GetSpriteID() != SPRITEID_NULL)
					{
						ImageObjectSSM.Add( pImageObject->GetSpriteID() );
					}
					
					
					if (pImageObject->IsAnimation())
					{
						DEBUG_ADD_FORMAT("AnimationObject: (%d, %d)",x, y);

						int fid = ((MAnimationObject*)pImageObject)->GetFrameID();

						if(fid < g_pTopView->m_ImageObjectFPK.GetSize())
						{
							FRAME_ARRAY* pFrameArray = &(g_pTopView->m_ImageObjectFPK[ fid ]);
							
							
							for (int j=0; j<pFrameArray->GetSize(); j++)
							{
								if ((*pFrameArray)[j].GetSpriteID() != SPRITEID_NULL)
								{
									ImageObjectSSM.Add( (*pFrameArray)[j].GetSpriteID() );
								}
							}
						}
					}
					
					
					iImageObject++;
				}
			}
		}
	}
}


//----------------------------------------------------------------------
// Add Sound
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MZone::AddSound(SOUND_NODE* pNode)
{
	m_listSoundNode.push_back( pNode );
}

//----------------------------------------------------------------------
// Update Sound
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MZone::UpdateSound()
{
	SOUND_NODE_LIST::iterator iSound = m_listSoundNode.begin();

	while (iSound != m_listSoundNode.end())
	{
		SOUND_NODE*	pNode = *iSound;

		//-------------------------------------------------
		
		//-------------------------------------------------
		if (pNode->GetPlayTime() < g_CurrentTime)
		{
			
			PlaySound( pNode->GetSoundID(), false, pNode->GetX(), pNode->GetY() );

			
			delete pNode;

			
			SOUND_NODE_LIST::iterator iTemp = iSound;
			iSound++;
			m_listSoundNode.erase( iTemp );
		}
		//-------------------------------------------------
		
		//-------------------------------------------------
		else
		{
			iSound++;
		}	
	}

	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	//----------------------------------------------------------------
	
	
	//----------------------------------------------------------------
	int zoneID = (g_bZonePlayerInLarge? g_nZoneLarge : g_nZoneSmall);
			
	if (zoneID==2106 || zoneID==2004 || zoneID==2014 || zoneID==2024)
	{
		if (!g_bPlayPropeller)// && g_CurrentTime > g_ZoneRandomSoundTime)
		{
			int x, y;
			x = g_pPlayer->GetX();// + ((rand()%2)? 1 : -1) * (rand()%7 + 4);
			y = g_pPlayer->GetY();// + ((rand()%2)? 1 : -1) * (rand()%6 + 2);

			PlaySound( SOUND_WORLD_PROPELLER, true, x, y );
			g_bPlayPropeller = TRUE;

			//g_ZoneRandomSoundTime = 0x0FFFFFFF;		// -_-;;
		}
	}
	else
	{
		if (g_bPlayPropeller)
		{
			StopSound( SOUND_WORLD_PROPELLER );

			g_bPlayPropeller = FALSE;
		}

		if (g_CurrentTime > g_ZoneRandomSoundTime)
		{
			ZONETABLE_INFO* pZoneInfo = g_pZoneTable->Get( zoneID );

			if (pZoneInfo!=NULL)
			{
				int soundID = pZoneInfo->GetRandomSoundID();			

				int x, y;

				x = g_pPlayer->GetX() + ((rand()%2)? 1 : -1) * (rand()%15 + 13);
				y = g_pPlayer->GetY() + ((rand()%2)? 1 : -1) * (rand()%12 + 10);

				PlaySound( soundID, false, x, y );
			}
			
			
			g_ZoneRandomSoundTime = g_CurrentTime + ((rand()%10)+6)*1000;			
		}
	}
}

//----------------------------------------------------------------------
// Update Item
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MZone::UpdateItem()
{
	ITEM_MAP::iterator iItem = m_mapItem.begin();

	while (iItem!= m_mapItem.end())
	{
		MItem*	pItem = (*iItem).second;

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		if (pItem->GetItemClass()==ITEM_CLASS_CORPSE)
		{
			MCorpse* pCorpse = (MCorpse*)pItem;

			MCreature* pCreature = pCorpse->GetCreature();

			if (pCreature!=NULL)
			{
				if (pCreature->GetNextAction()!=ACTION_STAND
					|| pCreature->GetActionCount()<pCreature->GetActionCountMax()
					|| pCreature->GetCreatureType() == 672 )
				{
					pCreature->Action();
				}
				pCreature->UpdateAttachEffect();
			}
		}

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		else if (pItem->IsDropping())
		{
			pItem->NextDropFrame();

			//--------------------------------------------------------
			
			//--------------------------------------------------------
			if (!pItem->IsDropping())
			{
				PlaySound( pItem->GetTileSoundID(),
							false,
							g_pPlayer->GetX(), g_pPlayer->GetY());
			}
		}
		
		iItem++;		
	}

}


//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool		
MZone::AddFakeCreature(MCreature* pCreature)
{
	DEBUG_ADD_FORMAT("MZone::AddFakeCreature id = %d", pCreature->GetID());

	CREATURE_MAP::iterator	theIterator;

	theIterator = m_mapFakeCreature.find(pCreature->GetID());
	
	
	if (theIterator == m_mapFakeCreature.end())
	{
		m_mapFakeCreature.insert(CREATURE_MAP::value_type(pCreature->GetID(), pCreature));
		
		return true;
	}		

	
	DEBUG_ADD_FORMAT("Add Failed - Already Exist in Zone");

	return false;
}

	

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MZone::RemoveFakeCreature(TYPE_OBJECTID id)
{
	CREATURE_MAP::iterator	theIterator;

	
	theIterator = m_mapFakeCreature.find(id);
    
	
	if (theIterator == m_mapFakeCreature.end())
		return false;


	MFakeCreature* pFakeCreature = (MFakeCreature *)theIterator->second;

	if(pFakeCreature != NULL)
	{
		
		if(pFakeCreature->GetOwnerID() != OBJECTID_NULL)
		{
			MCreature *pCreature = GetCreature(pFakeCreature->GetOwnerID());

			if(pCreature != NULL)
			{
				if(pCreature->GetPetID() == pFakeCreature->GetID())
					pCreature->SetPetID(OBJECTID_NULL);
				else if(pCreature->GetElementalID() == pFakeCreature->GetID())
					pCreature->SetElementalID(OBJECTID_NULL);
			}
		}

		delete pFakeCreature;
	}

	m_mapFakeCreature.erase(theIterator);

	return true;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
MCreature*	
MZone::GetFakeCreature(TYPE_OBJECTID id)
{
	CREATURE_MAP::iterator	theIterator;

	
	theIterator = m_mapFakeCreature.find(id);

	
	if (theIterator == m_mapFakeCreature.end()) 
	{
		return NULL;
	}

	
	return (*theIterator).second;
}

//----------------------------------------------------------------------
// Update FakeCreature
//----------------------------------------------------------------------
void		
MZone::UpdateFakeCreature()
{
	CREATURE_MAP::iterator	iCreature = m_mapFakeCreature.begin();

	MCreature* pCreature;
	
	//	DEBUG_ADD_FORMAT("[UpdateAllCreature] %d Creature(s)", m_mapCreature.size());
	
	//------------------------------------------------------
	
	//------------------------------------------------------
	while (iCreature != m_mapFakeCreature.end())
	{
		pCreature = iCreature->second;

		//------------------------------------------------------
		
		//------------------------------------------------------
		if (pCreature->GetClassType()==MCreature::CLASS_FAKE)
		{
			MFakeCreature* pFakeCreature = (MFakeCreature*)pCreature;

			pFakeCreature->Action();
			pFakeCreature->UpdateAttachEffect();

			//------------------------------------------------------
			
			//------------------------------------------------------
			if (pFakeCreature->IsFakeEnd())
			{	
				CREATURE_MAP::iterator	iCreatureTemp = iCreature;
				iCreature++;

				m_mapFakeCreature.erase( iCreatureTemp );

				delete pFakeCreature;

				continue;
			}
		}

		iCreature++;
	}
}

//----------------------------------------------------------------------
// Add Helicopter
//----------------------------------------------------------------------
bool
MZone::AddHelicopter(TYPE_OBJECTID creatureID, int x0, int y0)
{
	MHelicopter* pHelicopter = new MHelicopter;

	if (!pHelicopter->SetChaseCreature( creatureID, x0, y0 )
		|| !m_HelicopterManager.AddHelicopter( pHelicopter ))
	{
		DEBUG_ADD("Can't Add Helicopter");

		delete pHelicopter;

		return false;
	}

	return true;
}

//----------------------------------------------------------------------
// Get Helicopter
//----------------------------------------------------------------------
MHelicopter*	
MZone::GetHelicopter(TYPE_OBJECTID creatureID) const
{
	return m_HelicopterManager.GetHelicopter(creatureID);
}

//----------------------------------------------------------------------
// Remove Helicopter
//----------------------------------------------------------------------
void		
MZone::RemoveHelicopter(TYPE_OBJECTID creatureID)
{
	m_HelicopterManager.RemoveHelicopter( creatureID );
}

//----------------------------------------------------------------------
// Change To HalluCreature
//----------------------------------------------------------------------
void		
MZone::ChangeToHalluCreature()
{
	CREATURE_MAP::iterator	iCreature = m_mapCreature.begin();

	MCreature* pCreature;

	DEBUG_ADD("Zone::ChangeToHalluCreature");
	
	//------------------------------------------------------
	
	//------------------------------------------------------
	while (iCreature != m_mapCreature.end())
	{
		pCreature = iCreature->second;

		
		if (pCreature!=g_pPlayer)
		{
			pCreature->SetHalluCreature( g_pTopView->GetRandomMonsterTypeInZone() );
		}

		iCreature ++;
	}

	DEBUG_ADD("Zone::ChangeToHalluCreature OK");

}

//----------------------------------------------------------------------
// Remove HalluCreature
//----------------------------------------------------------------------
void		
MZone::RemoveHalluCreature()
{
	CREATURE_MAP::iterator	iCreature = m_mapCreature.begin();

	MCreature* pCreature;

	DEBUG_ADD("Zone::RemoveHalluCreature");
	
	//------------------------------------------------------
	
	//------------------------------------------------------
	while (iCreature != m_mapCreature.end())
	{
		pCreature = iCreature->second;

		
		if (pCreature!=g_pPlayer)
		{
			pCreature->UnSetHalluCreature();
		}

		iCreature ++;
	}

	DEBUG_ADD("Zone::RemoveHalluCreature OK");
/*
	ITEM_MAP::iterator	iItem = m_mapItem.begin();

	MItem *pItem;

	while (iItem != m_mapItem.end())
	{
		pItem = iItem->second;

		if (pItem != NULL && pItem->GetItemClass()==ITEM_CLASS_CORPSE)
		{
			MCorpse* pCorpse = (MCorpse*)pItem;
			
			MCreature* pCreature = pCorpse->GetCreature();
			
			if (pCreature!=NULL && pCreature != g_pPlayer)
			{
				pCreature->UnSetHalluCreature();
			}
		}

		iItem++;
	}

	DEBUG_ADD("Zone::RemoveHalluCorpse OK");
*/
}

//----------------------------------------------------------------------
// Set Safe Sector
//----------------------------------------------------------------------
void						
MZone::SetSafeSector( const RECT& rect, BYTE fSafe )
{
	int top = rect.top;
	int left = rect.left;
	int bottom = rect.bottom;
	int right = rect.right;

	if (top < 0) top = 0;
	if (left < 0) left = 0;
	if (bottom >= m_Height) bottom = m_Height-1;
	if (right >= m_Width) right = m_Width-1;

	if (top > bottom) { int temp=top; top=bottom; bottom=temp; }
	if (left > right) { int temp=left; left=right; right=left; }

	for (int i=top; i<=bottom; i++)
	{
		for (int j=left; j<=right; j++)
		{
			m_ppSector[i][j].SetSafe( fSafe );
		}
	}
}

MItem *
MZone::GetCorpseKilledByMe(int limitItemCount)
{
	ITEM_MAP::iterator itr = m_mapItem.begin();
	ITEM_MAP::iterator endItr = m_mapItem.end();

	while(itr != endItr)
	{
		MItem* pItem = itr->second;

		if (pItem!=NULL
			&& pItem->GetItemClass()==ITEM_CLASS_CORPSE && pItem->GetNumber() > limitItemCount)
		{
			MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();

			if (pCreature!=NULL
				&& pCreature->GetDrainCreatureID() == g_pPlayer->GetID())
			{
				return pItem;
			}
		}

		itr++;
	}

	return NULL;
}

BYTE			
MZone::GetPKType()
{
	if(g_pZoneTable)
	{
		ZONETABLE_INFO* pZoneInfo = g_pZoneTable->Get( GetID() );
		if(NULL != pZoneInfo)
			return pZoneInfo->PKType;
	}

	return 0;
}


// 2004, 8, 30, sobeit add start - sweep vice test
//----------------------------------------------------------------------
// ChangeSwapViceType
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
MZone::ChangeSwapViceType(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_EFFECTSPRITETYPE type, WORD wDelay)
{
	EFFECT_MAP::iterator iEffect = m_mapGroundEffect.begin();

	MEffect *pEffect = NULL;
	while (iEffect != m_mapGroundEffect.end())
	{
		pEffect = iEffect->second;
		if (pEffect != NULL)
		{
			if( pEffect->GetFrameID() == EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_1 ||
					pEffect->GetFrameID() == EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_2 ||
					pEffect->GetFrameID() == EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_3 )
				{
					pEffect->SetFrameID(type, pEffect->GetMaxFrame());
					pEffect->SetDelayFrame(wDelay);
					return;
			}
		}
		iEffect++;
	}

//	
//
//	MSector& sector = m_ppSector[sY][sX];
//
//	//--------------------------------------------------

//	//--------------------------------------------------
//	if (sector.IsExistEffect())
//	{
//		int num = sector.GetEffectSize();
//
//		EFFECT_LIST::const_iterator iEffect = sector.GetEffectIterator();
//
//		for (int i=0; i<num; i++)
//		{
//			MEffect* pEffect = *iEffect;
//
//			//--------------------------------------------------

//			//--------------------------------------------------
//			if (pEffect->GetEffectType() == MEffect::EFFECT_SECTOR)
//			{
//				if( pEffect->GetFrameID() == EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_1 ||
//					pEffect->GetFrameID() == EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_2 ||
//					pEffect->GetFrameID() == EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_3 )
//				{
//					pEffect->SetFrameID(type, pEffect->GetMaxFrame());
//					pEffect->SetDelayFrame(wDelay);
//					return;
//				}
//			}
//
//			iEffect++;
//		}
//	}
	return ;
}

void		
MZone::RemoveSwapViceType()
{
	EFFECT_MAP::iterator iEffect = m_mapGroundEffect.begin();

	MEffect *pEffect = NULL;
	while (iEffect != m_mapGroundEffect.end())
	{
		pEffect = iEffect->second;
		if (pEffect != NULL)
		{
			if( pEffect->GetFrameID() == EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_1 ||
					pEffect->GetFrameID() == EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_2 ||
					pEffect->GetFrameID() == EFFECTSPRITETYPE_SWEEP_VICE_PRECASTING_3 )
				{
					

					iEffect = m_mapGroundEffect.erase( iEffect );
					
					delete pEffect;

					continue;
			}
		}
		iEffect++;
	}

}
// 2004, 8, 30, sobeit add end - sweep vice test

// 2004, 8, 31, sobeit add start
void
MZone::UpdateWaitEffects()
{
	WAIT_EFFECT_LIST::iterator iWaitEffect = m_listWaitEffect.begin();

	while (iWaitEffect != m_listWaitEffect.end())
	{
		MEffect*	pNode = *iWaitEffect;
		if( !pNode->IsWaitFrame())
		{
			AddEffect( pNode );
			iWaitEffect = m_listWaitEffect.erase(iWaitEffect);
		}
		else
			iWaitEffect ++;
	}
}
// 2004, 8, 31, sobeit add end

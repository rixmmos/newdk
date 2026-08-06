//----------------------------------------------------------------------
// MZone.h
//----------------------------------------------------------------------
//



// ImageObject : ??
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//


//   Zone ID




//
//
//----------------------------------------------------------------------
// On-line Update
//----------------------------------------------------------------------
//




//



//
//----------------------------------------------------------------------

#ifndef	__MZONE_H__
#define	__MZONE_H__

#pragma warning(disable:4786)

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include <list>
#include <map>
#include <vector>
#include "MTypeDef.h"
#include "MSector.h"
class MObstacle;
class MItem;
class MCreature;
class MEffect;
class MPlayer;
class MImageObject;
class CSpriteSetManager;
#include "MHelicopterManager.h"
#include "CPositionList.h"
#include "ZoneFileHeader.h"
#include "SoundNode.h"

#include <fstream>
using namespace std;

struct UI_PORTAL_FLAG
{
	int zone_id, x, y, portal_x, portal_y;
};
typedef std::list<UI_PORTAL_FLAG> UI_PORTAL_LIST;
typedef std::vector<UI_PORTAL_LIST> UI_PORTAL_VECTOR;

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class MZone {
	//--------------------------------------------------------------		
	// typedef
	//--------------------------------------------------------------	
	public :
		// building map
		typedef std::map<TYPE_OBJECTID, MImageObject*>				IMAGEOBJECT_MAP;

		
		typedef	CPositionList<TYPE_SECTORPOSITION>							IMAGEOBJECT_POSITION_LIST;

		
		typedef	std::map<TYPE_OBJECTID, IMAGEOBJECT_POSITION_LIST*>	IMAGEOBJECT_POSITION_LIST_MAP;


		// creature map
		typedef std::map<TYPE_OBJECTID, MCreature*>		CREATURE_MAP;

		// Item map	
		typedef std::map<TYPE_OBJECTID, MItem*>			ITEM_MAP;

		// Obstacle list
		//typedef std::list<MObstacle*>					OBSTACLE_LIST;

		// Effect list
		typedef std::map<TYPE_OBJECTID, MEffect*>		EFFECT_MAP;

		// Sound list
		typedef std::list<SOUND_NODE*>					SOUND_NODE_LIST;

		// 2004, 8, 31, sobeit add start - WaitEffectList
		// Wiait Effect list
		typedef std::list<MEffect*>						WAIT_EFFECT_LIST;
		// 2004, 8, 31, sobeit add end - WaitEffectList
	public :
		MZone(TYPE_SECTORPOSITION width=0, TYPE_SECTORPOSITION height=0);
		~MZone();

		//--------------------------------------------------------------		
		// init / release
		//--------------------------------------------------------------		
		void		Init(TYPE_SECTORPOSITION width, TYPE_SECTORPOSITION height);
		void		Release();
		
		//--------------------------------------------------------------
		// Update
		//--------------------------------------------------------------
		void		Update();

		//--------------------------------------------------------------
		// Release Object
		//--------------------------------------------------------------
		void		ReleaseObject();


		//--------------------------------------------------------------
		// file I/O
		//--------------------------------------------------------------		
		bool		LoadFromFile(std::ifstream& file);

		bool		SaveToFileSectorSound(std::ofstream& file);
		bool		LoadFromFileSectorSound(std::ifstream& file);

		//--------------------------------------------------------------		
		
		//--------------------------------------------------------------		
		const MString&	GetVersion() const			{ return m_Info.ZoneVersion; } 
		WORD			GetID() const				{ return m_Info.ZoneID; }			
		WORD			GetGroupID() const			{ return m_Info.ZoneGroupID; }	
		const MString&	GetName() const				{ return m_Info.ZoneName; }		
		BYTE			GetType() const				{ return m_Info.ZoneType; }		
		BYTE			GetLevel() const			{ return m_Info.ZoneLevel; }		
		const MString&	GetDescription() const		{ return m_Info.Description; }	



		//--------------------------------------------------------------
		//
		// Creature
		//
		//--------------------------------------------------------------
		void		SetPlayer();//MPlayer* pPlayer);
		void		RemovePlayer();
		
		
		int			GetCreatureNumber() const		{ return m_mapCreature.size(); }
		CREATURE_MAP::const_iterator GetCreatureBegin()		{ return m_mapCreature.begin(); }

		// Move
		void		UpdateAllCreature();

		
		void		KeepObjectInSight(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y, BYTE sight);

		
		bool		CanMove(BYTE creatureType, TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y);
		void		SetServerBlock(BYTE creatureType, TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y);
		void		UnSetServerBlock(BYTE creatureType, TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y);

		
		bool		MoveGroundCreature(MCreature* pCreature, TYPE_SECTORPOSITION xo, TYPE_SECTORPOSITION yo, TYPE_SECTORPOSITION xn, TYPE_SECTORPOSITION yn);
		bool		MoveFlyingCreature(MCreature* pCreature, TYPE_SECTORPOSITION xo, TYPE_SECTORPOSITION yo, TYPE_SECTORPOSITION xn, TYPE_SECTORPOSITION yn);
		bool		MoveUndergroundCreature(MCreature* pCreature, TYPE_SECTORPOSITION xo, TYPE_SECTORPOSITION yo, TYPE_SECTORPOSITION xn, TYPE_SECTORPOSITION yn);

		
		
		TYPE_OBJECTID		GetCreatureID(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY);
		TYPE_OBJECTID		GetFlyingCreatureID(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY);
		TYPE_OBJECTID		GetUndergroundCreatureID(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY);

		TYPE_OBJECTID		GetCreatureID(const char* pName, int flag = 0) const;

		// other creatures : Add, Get, Remove
		bool		AddCreature(MCreature* pCreature);	
		bool		RemoveCreature(TYPE_OBJECTID	id);			
		MCreature*	GetCreature(TYPE_OBJECTID id);				
		MCreature*	GetCreatureOnly(TYPE_OBJECTID id);

		// 2004, 8, 12 sobeit add start
		MCreature*  GetCreatureBySector(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y);
		// 2004, 8, 12 sobeit add end
		
		

		
		void		RemoveUndergroundCreature(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_OBJECTID id)	{ m_ppSector[sY][sX].RemoveUndergroundCreature(id); }
		void		RemoveGroundCreature(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_OBJECTID id)		{ m_ppSector[sY][sX].RemoveGroundCreature(id); }
		void		RemoveFlyingCreature(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_OBJECTID id)		{ m_ppSector[sY][sX].RemoveFlyingCreature(id); }

		
		void		ChangeToHalluCreature();
		void		RemoveHalluCreature();


		//--------------------------------------------------------------
		//
		//							Item
		//
		//--------------------------------------------------------------
		bool		AddItem(MItem* pItem, BOOL bDropping=false);				
		bool		RemoveItem(TYPE_OBJECTID	id);	
		bool		PickupItem(TYPE_OBJECTID	id);	
		MItem*		GetItem(TYPE_OBJECTID id);			
		MItem*		GetCorpseKilledByMe(int limitItemCount = 0);				
		
		
		TYPE_OBJECTID		GetItemID(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY);	

		
		//bool		AddCorpseFromCreature(MCreature* pCreature);
		bool		AddCorpseFromCreature(TYPE_OBJECTID id);

		
		int			GetItemNumber() const		{ return m_mapItem.size(); }
		ITEM_MAP::const_iterator GetItemBegin()		{ return m_mapItem.begin(); }

		// update item
		void		UpdateItem();


		//--------------------------------------------------------------
		//
		//							ImageObject
		//
		//--------------------------------------------------------------
		bool		AddImageObject(MImageObject* pImageObject);	
		bool		RemoveImageObject(TYPE_OBJECTID	id);			
		MImageObject*	GetImageObject(TYPE_OBJECTID id);				
		void		SetImageObjectSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_OBJECTID id);	// ImageObject Sector
		void		UnSetImageObjectSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_OBJECTID id);	// Unset ImageObject Sector

		
		const IMAGEOBJECT_MAP&	GetImageObjectMap() const { return m_mapImageObject; }	

		//--------------------------------------------------------------
		//
		//							Obstacle
		//
		//--------------------------------------------------------------
		
		
		

		//--------------------------------------------------------------
		//
		//							 Effect
		//
		//--------------------------------------------------------------
		bool		AddEffect(MEffect* pEffect, DWORD dwWaitCount = 0);	
		bool		RemoveEffect(TYPE_OBJECTID id);
		void		UpdateEffects();				
		// 2004, 8, 31, sobeit add start
		void		UpdateWaitEffects();
		// 2004, 8, 31, sobeit add end
		MEffect*	GetEffect(TYPE_OBJECTID id) const;
		int			GetEffectCount()				{ return m_mapEffect.size(); }
		EFFECT_MAP::const_iterator GetEffects()		{ return m_mapEffect.begin(); }
		bool		RemoveTileEffect(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, int effectStatus, int serverID=0);
		bool		RemoveTileEffect(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_EFFECTSPRITETYPE type, int serverID=0);
		void		ChangeSwapViceType(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, TYPE_EFFECTSPRITETYPE type, WORD wDelay = 0xffff);
		void		RemoveSwapViceType();
		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		bool		AddGroundEffect(MEffect* pEffect);	
		bool		RemoveGroundEffect(TYPE_OBJECTID id);
		MEffect*	GetGroundEffect(TYPE_OBJECTID id) const;
		void		UpdateGroundEffects();				
		int			GetGroundEffectCount()				{ return m_mapGroundEffect.size(); }
		EFFECT_MAP::const_iterator GetGroundEffects()	{ return m_mapGroundEffect.begin(); }
		EFFECT_MAP::const_iterator GetGroundEffectsEnd()	{ return m_mapGroundEffect.end(); }

		//--------------------------------------------------------------
		//
		//						 Item
		//
		//--------------------------------------------------------------
		void		AddSound(SOUND_NODE* pNode);	
		void		UpdateSound();					


		//--------------------------------------------------------------
		//
		//							Sector
		//
		//--------------------------------------------------------------		
		bool		IsPositionInZone(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)	{ return x<m_Width && y<m_Height; }

		void		SetSector(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y, const MSector& sector)	{ m_ppSector[y][x]=sector; }

		void		SetBlockAllSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)				{ m_ppSector[sY][sX].SetBlockAll(); }		
		void		UnSetBlockAllSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)			{ m_ppSector[sY][sX].UnSetBlockAll(); }	

		void		SetBlockUndergroundSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)		{ m_ppSector[sY][sX].SetBlockUnderground(); }		
		void		UnSetBlockUndergroundSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)	{ m_ppSector[sY][sX].UnSetBlockUnderground(); }	

		void		SetBlockGroundSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)			{ m_ppSector[sY][sX].SetBlockGround(); }	
		void		UnSetBlockGroundSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)		{ m_ppSector[sY][sX].UnSetBlockGround(); }	

		void		SetBlockFlyingSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)	{ m_ppSector[sY][sX].SetBlockFlying(); }					
		void		UnSetBlockFlyingSector(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)	{ m_ppSector[sY][sX].UnSetBlockFlying(); }			

		void		SetVisitedFlag(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)				{ m_ppSector[sY][sX].SetVisitedFlag(); }
		void		UnSetVisitedFlag(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)			{ m_ppSector[sY][sX].UnSetVisitedFlag(); }
		const bool	IsVisitedFlag(TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY) const		{ return m_ppSector[sY][sX].IsVisitedFlag(); }
		
		//--------------------------------------------------------------		
		//
		//		get
		//
		//--------------------------------------------------------------		
		TYPE_SECTORPOSITION			GetWidth() const				{ return m_Width; }
		TYPE_SECTORPOSITION			GetHeight()	const				{ return m_Height; }
		const MSector&	GetSector(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y) const	{ return m_ppSector[y][x]; }
		//const MPlayer&	GetPlayer()	const				{ return *m_pPlayer; }
	
		long						GetTileFilePosition() const			{ return m_fpTile; }
		long						GetImageObjectFilePosition() const	{ return m_fpImageObject; }

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		void						GetNearSpriteSet(CSpriteSetManager& TileSSM, CSpriteSetManager& ImageObjectSSM, TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY) const;


		//--------------------------------------------------------------		
		//
		// portal
		//
		//--------------------------------------------------------------		
		void						AddPortal(int type, int zoneID, const RECT& rect);

		//--------------------------------------------------------------		
		//
		// Safe
		//
		//--------------------------------------------------------------		
		void						SetSafeSector( const RECT& safeRect, BYTE fSafe );


		//--------------------------------------------------------------
		//
		// Fake Creature
		//
		//--------------------------------------------------------------
		
		int			GetFakeCreatureNumber() const		{ return m_mapFakeCreature.size(); }
		CREATURE_MAP::const_iterator GetFakeCreatureBegin()		{ return m_mapFakeCreature.begin(); }

		// Move
		void		UpdateFakeCreature();

		// other creatures : Add, Get, Remove
		bool		AddFakeCreature(MCreature* pCreature);	
		bool		RemoveFakeCreature(TYPE_OBJECTID id);			
		MCreature*	GetFakeCreature(TYPE_OBJECTID id);				
	
		//--------------------------------------------------------------
		// ZoneSound
		//--------------------------------------------------------------
		bool		IsZoneSoundLoaded() const		{ return m_bZoneSoundLoaded; }

		//--------------------------------------------------------------
		// HelicopterManager
		//--------------------------------------------------------------
		bool			AddHelicopter(TYPE_OBJECTID creatureID, int x0, int y0);
		MHelicopter*	GetHelicopter(TYPE_OBJECTID creatureID) const;
		void			RemoveHelicopter(TYPE_OBJECTID creatureID);
		UI_PORTAL_VECTOR &	GetHorn() { return m_horn; }
		
		void			SetCurrentZoneID( WORD id ) { m_Info.ZoneID = id; }			

		BYTE			GetPKType();

		
	protected :
		// Zone File Header
		FILEINFO_ZONE_HEADER	m_Info;

		TYPE_SECTORPOSITION					m_Width;		
		TYPE_SECTORPOSITION					m_Height;		
		MSector**				m_ppSector;		

		// Other Creature
		CREATURE_MAP			m_mapCreature;	// other creatures	

		// Other Creature
		ITEM_MAP				m_mapItem;		// Item map

		// ImageObjects
		IMAGEOBJECT_MAP			m_mapImageObject;	// ImageObject map

		// Obstacle
		//OBSTACLE_LIST			m_listObstacle;	// Obstacle List

		// Effects
		EFFECT_MAP				m_mapEffect;	// Effect map

		// Effects
		EFFECT_MAP				m_mapGroundEffect;	// Effect map

		
		long					m_fpTile;
		long					m_fpImageObject;

		SOUND_NODE_LIST			m_listSoundNode;

		
		CREATURE_MAP			m_mapFakeCreature;

		
		bool					m_bZoneSoundLoaded;

		MHelicopterManager		m_HelicopterManager;
		UI_PORTAL_VECTOR		m_horn;
		
		// 2004, 8, 31, sobeit add start
		WAIT_EFFECT_LIST		m_listWaitEffect;
		// 2004, 8, 31, sobeit add end
};

// Zone
extern MZone*						g_pZone;

#endif



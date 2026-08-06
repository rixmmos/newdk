//----------------------------------------------------------------------
// MZoneTable.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//

//
// - ZoneID


// 
//----------------------------------------------------------------------

#ifndef	__MZONETABLE_H__
#define	__MZONETABLE_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include "MTypeDef.h"
#include "MString.h"
#include <map>
#include <list>
#include <fstream>
using namespace std;

#define	FLAG_ZONESIZE_SMALL		0x01



#define PK_TYPE_NULL			0			
#define PK_TYPE_DISABLE			0x01		
#define PK_TYPE_ALL				0x02		
#define PK_TYPE_GUILD			0x03		
#define PK_TYPE_SIEGE			0x04		

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class ZONETABLE_INFO {
	public :
		typedef std::list<TYPE_SOUNDID>			SOUNDID_LIST;

	
	public :
		TYPE_ZONEID				ID;						// Zone ID
		MString					Name;					
		BYTE					Property;				
		MString					Filename;				
		MString					InfoFilename;			
		MString					TeenFilename;			
		TYPE_MUSICID			MusicIDSlayer;			
		TYPE_MUSICID			MusicIDVampire;			
		SOUNDID_LIST			SoundIDList;			
		WORD					CreatureColorSet;		
		bool					Safety;
		unsigned short			MinimapSpriteID;		
		bool					HolyLand;				
		bool					ChatMaskByRace;			
		bool					CannotAttackInSafe;		
		bool					CannotUseSpecialItem;	
		bool					CompetenceZone;
		BYTE					PKType;					
		
	public :
		ZONETABLE_INFO();

		TYPE_SOUNDID			GetRandomSoundID() const;

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void			SaveToFile(std::ofstream& file);		
		void			LoadFromFile(std::ifstream& file);		
};

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class CZoneTable {
	public :
		typedef	std::map<TYPE_ZONEID, ZONETABLE_INFO*>	ZONEINFO_MAP;

	public :
		CZoneTable();
		~CZoneTable();

		//-------------------------------------------------------
		// Release
		//-------------------------------------------------------
		void				Release();

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		bool				Add(ZONETABLE_INFO* pZoneInfo);
		ZONETABLE_INFO*		Get(TYPE_ZONEID id);

		const ZONEINFO_MAP&	GetZoneMap() const	{ return m_mapZoneInfo; }

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void				SaveToFile(std::ofstream& file);
		void				LoadFromFile(std::ifstream& file);
	
	protected :
		ZONEINFO_MAP		m_mapZoneInfo;		
};

extern CZoneTable*		g_pZoneTable;

#endif



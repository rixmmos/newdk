//----------------------------------------------------------------------
// ZoneFileHeader.h
//----------------------------------------------------------------------

#ifndef	__ZONEFILEHEADER_H__
#define	__ZONEFILEHEADER_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "basic/Platform.h"
#endif
#include "MString.h"
#include "MZoneDef.h"

#include <fstream>
using namespace std;

//----------------------------------------------------------------------
// FILEINFO_ZONE_HEADER
//----------------------------------------------------------------------
class FILEINFO_ZONE_HEADER {
public:
	enum ZONE_TYPE
	{
		NORMAL,
		SLAYER_GUILD,
		RESERVED_SLAYER_GUILD,
		NPC_VAMPIRE_LAIR,
		PC_VAMPIRE_LAIR,
		NPC_HOME,
		NPC_SHOP,
		RANDOMAP
	};

public :
	MString		ZoneVersion;	
	WORD		ZoneID;			
	WORD		ZoneGroupID;	
	MString		ZoneName;		


	BYTE		ZoneType;		
	BYTE		ZoneLevel;		
	MString		Description;	

public :
	FILEINFO_ZONE_HEADER()
	{
		ZoneVersion = MAP_VERSION_2000_05_10;
	}

	void		SaveToFile(std::ofstream& file);
	void		LoadFromFile(std::ifstream& file);

};

#endif


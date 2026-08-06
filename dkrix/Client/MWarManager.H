//-----------------------------------------------------------------------------------------------
// MWarManager
//  Date : 2003.2
//  
//  by sonee
// 


//-----------------------------------------------------------------------------------------------

#ifndef __MWARMANAGER_H__
#define __MWARMANAGER_H__

//#include "types.h"
#include "Packet/WarInfo.h"

#include <map>
#include <list>
#include <vector>

enum WAR_TYPE_ID 
{
	WAR_TYPE_HOLYLAND,					
	WAR_TYPE_GUILD,						
};

typedef			std::map<ZoneID_t, WarInfo*>			WarInfoMap;
typedef			WarInfoMap::iterator			WarInfoMapItr;


class MWarManager
{
private :


	WarInfoMap			m_WarInfo;								

public :	
	MWarManager();
	~MWarManager();	

	//-------------------------------------------------------------------
	// SetData
	//-------------------------------------------------------------------
	void			SetWar(WarInfo *info);						// WarList Add
	void			RemoveWar(ZoneID_t id);						// WarList Remove
	
	void			ClearWar();									// Clear
	void			ClearRaceWar();

	//-------------------------------------------------------------------
	// Get Data
	//-------------------------------------------------------------------
	WarInfo*						GetWarInfo(ZoneID_t id);
	const WarInfoMap&				getWarInfoList() { return m_WarInfo; }
	int								getSize() { return m_WarInfo.size(); }
	
	//-------------------------------------------------------------------
	// Check
	//-------------------------------------------------------------------
	bool			IsExist(ZoneID_t id);						

	
	void			Update();	
};

extern MWarManager	*g_pWarManager;

#endif

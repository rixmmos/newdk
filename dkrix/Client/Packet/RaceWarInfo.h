//--------------------------------------------------------------------------------
// 
// Filename    : RaceWarInfo.h 
// Written By  : 
// Description : 
// 
//--------------------------------------------------------------------------------

#ifndef __RACE_WAR_LIST_H__
#define __RACE_WAR_LIST_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"
#include "WarInfo.h"

//--------------------------------------------------------------------------------
//
// class WarInfo;
//

//
//--------------------------------------------------------------------------------

class RaceWarInfo : public WarInfo {

public :
	typedef ValueList<ZoneID_t>	ZoneIDList;

public :
	RaceWarInfo() {}
	~RaceWarInfo() {}
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

//	// execute packet's handler
//	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketSize_t getSize() const throw() 
	{ 
		return WarInfo::getSize() + m_CastleIDs.getPacketSize();
	}

	static PacketSize_t getMaxSize() throw() 
	{
		return WarInfo::getMaxSize() + ZoneIDList::getPacketMaxSize();
	}

#ifdef __DEBUG_OUTPUT__
	// get packet's debug std::string
	std::string toString() const throw();
#endif

public :
	WarType_t 			getWarType() const 			{ return WAR_RACE; }

	ZoneIDList& 		getCastleIDs()				{ return m_CastleIDs; }
	void 				addCastleID(ZoneID_t zid) 	{ m_CastleIDs.addValue( zid ); }

private :
	ZoneIDList			m_CastleIDs;		
};

#endif

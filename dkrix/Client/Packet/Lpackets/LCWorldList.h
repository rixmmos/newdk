//////////////////////////////////////////////////////////////////////
// 
// Filename    : LCWorldList.h 
// Written By  : Reiot
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __LC_WORLD_LIST_H__
#define __LC_WORLD_LIST_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"
#include "../WorldInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class LCWorldList;
//
//////////////////////////////////////////////////////////////////////

class LCWorldList : public Packet {

public:

	// constructor
	
	LCWorldList() throw();

	// destructor
	
	~LCWorldList() throw();
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_LC_WORLD_LIST; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw();
	
	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName() const throw() { return "LCWorldList"; }
		
		// get packet's debug std::string
		std::string toString() const throw();
	#endif
	
public:

	
	WorldID_t getCurrentWorldID() const throw() { return m_CurrentWorldID; }
	void setCurrentWorldID( WorldID_t WorldID ) throw() { m_CurrentWorldID = WorldID; }

    BYTE getListNum() const throw() { return m_WorldInfoList.size(); }

	// add / delete / clear S List
	void addListElement(WorldInfo* pWorldInfo) throw() { m_WorldInfoList.push_back(pWorldInfo); }

	// ClearList
	void clearList() throw() { m_WorldInfoList.clear(); }

	// pop front Element in Status List
	WorldInfo* popFrontListElement() throw()
	{
		WorldInfo* TempWorldInfo = m_WorldInfoList.front(); m_WorldInfoList.pop_front(); return TempWorldInfo;
	}

private : 

	
	WorldID_t m_CurrentWorldID;

	
	std::list<WorldInfo*> m_WorldInfoList;

};

//////////////////////////////////////////////////////////////////////
//
// class LCWorldListFactory;
//
// Factory for LCWorldList
//
//////////////////////////////////////////////////////////////////////

class LCWorldListFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket() throw() { return new LCWorldList(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName() const throw() { return "LCWorldList"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_LC_WORLD_LIST; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		
		
		return szWorldID  + szBYTE + WorldInfo::getMaxSize();
	}
	
};


//////////////////////////////////////////////////////////////////////
//
// class LCWorldListHandler;
//
//////////////////////////////////////////////////////////////////////

class LCWorldListHandler {

public:

	// execute packet's handler
	static void execute(LCWorldList* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif

//////////////////////////////////////////////////////////////////////
// 
// Filename    : LCServerList.h 
// Written By  : Reiot
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __LC_SERVER_LIST_H__
#define __LC_SERVER_LIST_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"
#include "../ServerGroupInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class LCServerList;
//
//////////////////////////////////////////////////////////////////////

class LCServerList : public Packet {

public:

	// constructor
	
	LCServerList() throw();

	// destructor
	
	~LCServerList() throw();
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_LC_SERVER_LIST; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw();
	
	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName() const throw() { return "LCServerList"; }
		
		// get packet's debug std::string
		std::string toString() const throw();
	#endif
	
public:

	
	ServerGroupID_t getCurrentServerGroupID() const throw() { return m_CurrentServerGroupID; }
	void setCurrentServerGroupID( ServerGroupID_t ServerGroupID ) throw() { m_CurrentServerGroupID = ServerGroupID; }

    BYTE getListNum() const throw() { return m_ServerGroupInfoList.size(); }

	// add / delete / clear S List
	void addListElement(ServerGroupInfo* pServerGroupInfo) throw() { m_ServerGroupInfoList.push_back(pServerGroupInfo); }

	// ClearList
	void clearList() throw() { m_ServerGroupInfoList.clear(); }

	// pop front Element in Status List
	ServerGroupInfo* popFrontListElement() throw()
	{
		ServerGroupInfo* TempServerGroupInfo = m_ServerGroupInfoList.front(); m_ServerGroupInfoList.pop_front(); return TempServerGroupInfo;
	}

private : 

	
	ServerGroupID_t m_CurrentServerGroupID;

	
	std::list<ServerGroupInfo*> m_ServerGroupInfoList;

};

//////////////////////////////////////////////////////////////////////
//
// class LCServerListFactory;
//
// Factory for LCServerList
//
//////////////////////////////////////////////////////////////////////

class LCServerListFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket() throw() { return new LCServerList(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName() const throw() { return "LCServerList"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_LC_SERVER_LIST; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		
		
		return szServerGroupID  + szBYTE + ServerGroupInfo::getMaxSize();
	}
	
};


//////////////////////////////////////////////////////////////////////
//
// class LCServerListHandler;
//
//////////////////////////////////////////////////////////////////////

class LCServerListHandler {

public:

	// execute packet's handler
	static void execute(LCServerList* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif

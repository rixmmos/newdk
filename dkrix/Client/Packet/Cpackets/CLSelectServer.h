//////////////////////////////////////////////////////////////////////
// 
// Filename    : CLSelectServer.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CL_SELECT_SERVER_H__
#define __CL_SELECT_SERVER_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CLSelectServer;
//
//////////////////////////////////////////////////////////////////////

class CLSelectServer : public Packet {

public:
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CL_SELECT_SERVER; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szServerGroupID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName() const throw() { return "CLSelectServer"; }

		// get packet's debug std::string
		std::string toString() const throw() { return "CLSelectServer"; }
	#endif

	// get / set ServerGroupID
	ServerGroupID_t getServerGroupID() const throw() { return m_ServerGroupID; }
	void setServerGroupID(ServerGroupID_t ServerGroupID) throw() { m_ServerGroupID = ServerGroupID; }
	

private :

	ServerGroupID_t m_ServerGroupID;

};


//////////////////////////////////////////////////////////////////////
//
// class CLSelectServerFactory;
//
// Factory for CLSelectServer
//
//////////////////////////////////////////////////////////////////////

class CLSelectServerFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket() throw() { return new CLSelectServer(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName() const throw() { return "CLSelectServer"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CL_SELECT_SERVER; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const throw() { return szServerGroupID; }

};


//////////////////////////////////////////////////////////////////////
//
// class CLSelectServerHandler;
//
//////////////////////////////////////////////////////////////////////

#ifndef __GAME_CLIENT__
	class CLSelectServerHandler {

	public:

		// execute packet's handler
		static void execute(CLSelectServer* pPacket, Player* player) throw(ProtocolException, Error);

	};
#endif

#endif

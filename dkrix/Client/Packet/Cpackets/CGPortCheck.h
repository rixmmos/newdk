//----------------------------------------------------------------------
// 
// Filename    : CGPortCheck.h 
// Written By  : Reiot
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __CG_PORT_CHECK_H__
#define __CG_PORT_CHECK_H__

// include files
#include "../DatagramPacket.h"
#include "../PacketFactory.h"


//----------------------------------------------------------------------
//
// class CGPortCheck;
//



//
// *CAUTION*
//






//
//----------------------------------------------------------------------

class CGPortCheck : public DatagramPacket {

public:
	
    
    void read(Datagram & iDatagram) throw(ProtocolException, Error);
		    
    
    void write(Datagram & oDatagram) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_PORT_CHECK; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() 
	{ 
		return szBYTE + m_PCName.size();		// PC name
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName() const throw() { return "CGPortCheck"; }
		
		// get packet's debug std::string
		std::string toString() const throw();
	#endif

public:

	// get/set pcName
	std::string getPCName() const throw() { return m_PCName; }
	void setPCName(const std::string& pcName) throw() { m_PCName = pcName; }

private :

	// PC name
	std::string m_PCName;
};


//////////////////////////////////////////////////////////////////////
//
// class CGPortCheckFactory;
//
// Factory for CGPortCheck
//
//////////////////////////////////////////////////////////////////////

class CGPortCheckFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket() throw() { return new CGPortCheck(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName() const throw() { return "CGPortCheck"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_PORT_CHECK; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return szBYTE + 20;		// PC name
	}

};


//////////////////////////////////////////////////////////////////////
//
// class CGPortCheckHandler;
//
////
//////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGPortCheckHandler {
	
public:

	// execute packet's handler
	static void execute(CGPortCheck* pPacket) throw(ProtocolException, Error);

};
#endif

#endif

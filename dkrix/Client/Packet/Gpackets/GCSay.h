//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCSay.h 
// Written By  : Reiot
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_SAY_H__
#define __GC_SAY_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCSay;
//



//
//////////////////////////////////////////////////////////////////////

class GCSay : public Packet {

public :
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_SAY; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szObjectID + szuint + szBYTE + m_Message.size(); }

#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const throw() { return "GCSay"; }
	
	// get packet's debug std::string
	std::string toString() const throw();
#endif

	// get/set sender's creature id
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID(const ObjectID_t & creatureID) throw() { m_ObjectID = creatureID; }

	//get/set text color
	uint getColor() const throw() { return m_Color; }
	void setColor( uint color ) throw() { m_Color = color; }

	// get/set chatting message
	std::string getMessage() const throw() { return m_Message; }
	void setMessage(const std::string & msg) throw() { m_Message = msg; }
	

private :
	
	// character's creature id
	ObjectID_t m_ObjectID;

	// text color
	uint m_Color;
	
	// chatting message
	std::string m_Message;
	
};


//////////////////////////////////////////////////////////////////////
//
// class GCSayFactory;
//
// Factory for GCSay
//
//////////////////////////////////////////////////////////////////////

class GCSayFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCSay(); }

	// get packet name
	std::string getPacketName() const throw() { return "GCSay"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_SAY; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + szuint + szBYTE + 128 ; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCSayHandler;
//
//////////////////////////////////////////////////////////////////////

class GCSayHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCSay* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif

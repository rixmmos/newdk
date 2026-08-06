//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCPartySay.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_PARTY_SAY_H__
#define __GC_PARTY_SAY_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCPartySay;
//


//
//////////////////////////////////////////////////////////////////////

class GCPartySay : public Packet
{
public :
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_PARTY_SAY; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize() const throw() { return szBYTE + m_Name.size() + szBYTE + m_Message.size() + szuint; }

#ifdef __DEBUG_OUTPUT__
	// get packet's name
	string getPacketName() const throw() { return "GCPartySay"; }
	
	// get packet's debug string
	string toString() const throw();
#endif

public :
	string	getName() const { return m_Name; }
	void	setName( const string& name ) { m_Name = name; }

	string	getMessage() const { return m_Message; }
	void	setMessage( const string& msg ) { m_Message = msg; }

	uint getColor() const throw() { return m_Color; }
	void setColor( uint color ) throw() { m_Color = color; }

private :
	string	m_Name;
	uint	m_Color;
	string	m_Message;
};


//////////////////////////////////////////////////////////////////////
//
// class GCPartySayFactory;
//
// Factory for GCPartySay
//
//////////////////////////////////////////////////////////////////////

class GCPartySayFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCPartySay(); }

	// get packet name
	string getPacketName() const throw() { return "GCPartySay"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_PARTY_SAY; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + 20 + szBYTE + 128 + szuint; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCPartySayHandler;
//
//////////////////////////////////////////////////////////////////////

class GCPartySayHandler {

public :

	// execute packet's handler
	static void execute(GCPartySay* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif

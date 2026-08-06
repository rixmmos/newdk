//////////////////////////////////////////////////////////////////////
// 
// Filename    : DatagramPacket.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __DATAGRAM_PACKET_H__
#define __DATAGRAM_PACKET_H__

// include files
#include "Packet.h"
#include "Datagram.h"
#include "SocketAPI.h"


//////////////////////////////////////////////////////////////////////
//
// class DatagramPacket;
//




//
//////////////////////////////////////////////////////////////////////

class DatagramPacket : public Packet {
	
public :
	
	// destructor
	virtual ~DatagramPacket () throw () {}

	
	
	virtual void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error ) 
	{ 
		throw ProtocolException("datagram packet from TCP socket"); 
	}

	
	virtual void read ( Datagram & iDatagram ) throw ( ProtocolException , Error ) = 0;
	
	
	
	virtual void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error )
	{ 
		throw Error("cannot write datagram-packet to TCP-socket-stream"); 
	}

	
	virtual void write ( Datagram & oDatagram ) const throw ( ProtocolException , Error ) = 0;
	
	// execute packet's handler
	virtual void execute ( Player * pPlayer ) throw ( ProtocolException , Error ) = 0;
	
	// get packet's DatagramPacketID	
	virtual PacketID_t getPacketID () const throw () = 0;

	// get packet's body size
	virtual PacketSize_t getPacketSize () const throw () = 0;

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		virtual std::string getPacketName () const throw () = 0;
			
		// get packet's debug std::string
		virtual std::string toString () const throw () = 0;
	#endif

	// get/set host
	const std::string& getHost () const throw () { return m_Host; }
	void setHost ( std::string host ) throw () { m_Host = host; }

	// get/set port
	uint getPort () const throw () { return m_Port; }
	void setPort ( uint port ) throw () { m_Port = port; }
	

protected :

	// sender's host
	std::string m_Host;

	// sender's port
	uint m_Port;
	
};

#endif

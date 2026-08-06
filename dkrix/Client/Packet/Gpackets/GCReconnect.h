//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCReconnect.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_RECONNECT_H__
#define __GC_RECONNECT_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCReconnect;
//



//
//////////////////////////////////////////////////////////////////////

class GCReconnect : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_RECONNECT; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () 
	{ 
		return szBYTE + m_Name.size() 		
			+ szPCType 						
			+ szBYTE + m_ServerIP.size() 	
			+ szDWORD; 						
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCReconnect"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

public :

	// get/set creature name
	std::string getName () const throw () { return m_Name; }
	void setName ( const std::string & name ) throw () { m_Name = name; }

	// get/set pc type
	PCType getPCType () const throw () { return m_PCType; }
	void setPCType ( PCType pcType ) throw () { m_PCType = pcType; }

	// get/set server ip
	std::string getServerIP () const throw () { return m_ServerIP; }
	void setServerIP ( const std::string & serverIP ) throw () { m_ServerIP = serverIP; }

	// get/set key
	DWORD getKey () const throw () { return m_Key; }
	void setKey ( DWORD key ) throw () { m_Key = key; }

private :
	
	// creature name
	std::string m_Name;

	// pc type
	PCType m_PCType;
	
	// New Server IP
	std::string m_ServerIP;

	// authentication key
	DWORD m_Key;

};


//////////////////////////////////////////////////////////////////////
//
// class GCReconnectFactory;
//
// Factory for GCReconnect
//
//////////////////////////////////////////////////////////////////////

class GCReconnectFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCReconnect(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCReconnect"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_RECONNECT; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw ()
	{
		return szBYTE + 20 		 		
			+ szPCType 					
			+ szBYTE + 15			 	
			+ szDWORD; 					
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GCReconnectHandler;
//
//////////////////////////////////////////////////////////////////////

class GCReconnectHandler {

public :

	// execute packet's handler
	static void execute ( GCReconnect * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif

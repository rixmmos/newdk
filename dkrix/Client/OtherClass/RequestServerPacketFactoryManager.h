//////////////////////////////////////////////////////////////////////
// 
// Filename    : RequestServerPacketFactoryManager.h 
// Written By  : reiot@ewestsoft.com
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __REQUEST_SERVER_PACKET_FACTORY_MANAGER_H__
#define __REQUEST_SERVER_PACKET_FACTORY_MANAGER_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class RequestServerPacketFactoryManager
//
//////////////////////////////////////////////////////////////////////

class RequestServerPacketFactoryManager {

public :
	
	// constructor
	RequestServerPacketFactoryManager () throw ();
	
	// destructor
	~RequestServerPacketFactoryManager () throw ();

	
	
	void init () throw ( Error );
	
	
	void addFactory ( PacketFactory * pFactory ) throw ( Error );
	
	
	Packet * createPacket ( PacketID_t packetID ) throw ( InvalidProtocolException , Error );

	#ifdef	__DEBUG_OUTPUT__
		
		std::string getPacketName ( PacketID_t packetID ) throw ( InvalidProtocolException , Error );

		// get debug string
		std::string toString () const throw ();
	#endif

	
	PacketSize_t getPacketMaxSize ( PacketID_t packetID ) throw ( InvalidProtocolException , Error );

	

private :
	
	
	PacketFactory ** m_Factories;
	
	
	ushort m_Size;

};

extern RequestServerPacketFactoryManager * g_pRequestServerPacketFactoryManager;

#endif

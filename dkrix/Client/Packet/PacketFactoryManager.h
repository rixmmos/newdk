//////////////////////////////////////////////////////////////////////
// 
// Filename    : PacketFactoryManager.h 
// Written By  : reiot@ewestsoft.com
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __PACKET_FACTORY_MANAGER_H__
#define __PACKET_FACTORY_MANAGER_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class PacketFactoryManager
//
//////////////////////////////////////////////////////////////////////

class PacketFactoryManager {

public :
	
	// constructor
	PacketFactoryManager () throw ();
	
	// destructor
	~PacketFactoryManager () throw ();

	
	
	void init () throw (Error);
	
	
	void addFactory (PacketFactory* pFactory) throw (Error);
	
	
	Packet* createPacket (PacketID_t packetID) throw (InvalidProtocolException, Error);

#ifdef __DEBUG_OUTPUT__
	
	std::string getPacketName (PacketID_t packetID) throw (InvalidProtocolException, Error);
		// get debug std::string
	std::string toString () const throw ();
#endif


	
	PacketSize_t getPacketMaxSize (PacketID_t packetID) throw (InvalidProtocolException, Error);



private :
	
	
	PacketFactory ** m_Factories;
	
	
	ushort m_Size;

};

extern PacketFactoryManager* g_pPacketFactoryManager;

#endif

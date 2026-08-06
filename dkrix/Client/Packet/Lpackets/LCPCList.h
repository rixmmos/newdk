//////////////////////////////////////////////////////////////////////
// 
// Filename    : LCPCList.h 
// Written By  : Reiot
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __LC_PC_LIST_H__
#define __LC_PC_LIST_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"
#include "../PCInfo.h"
#include "packet/PCSlayerInfo.h"
#include "packet/PCVampireInfo.h"
#include "packet/PCOustersInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class LCPCList;
//
//////////////////////////////////////////////////////////////////////

class LCPCList : public Packet {

public :

	// constructor
	
	LCPCList () throw ();

	// destructor
	
	~LCPCList () throw ();
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_LC_PC_LIST; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw ();
	
	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "LCPCList"; }
		
		// get packet's debug string
		std::string toString () const throw ();
	#endif
	
public :

	// get/set pc info
	PCInfo * getPCInfo ( Slot slot ) const throw ( NoSuchElementException ) 
	{ 
		if ( m_pPCInfos[ slot ] == NULL )
			throw NoSuchElementException("no such PC exist in that slot");

		return m_pPCInfos[slot];
	}

	void setPCInfo ( Slot slot , PCInfo * pPCInfo ) throw ( DuplicatedException ) 
	{ 
		if ( m_pPCInfos[ slot ] != NULL )
			throw DuplicatedException("PCInfo duplicated.");
		m_pPCInfos[slot] = pPCInfo;
	}

private : 

	
	PCInfo * m_pPCInfos[ SLOT_MAX ];

};


//////////////////////////////////////////////////////////////////////
//
// class LCPCListFactory;
//
// Factory for LCPCList
//
//////////////////////////////////////////////////////////////////////

class LCPCListFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new LCPCList(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "LCPCList"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_LC_PC_LIST; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () 
	{ 
		
		
		return SLOT_MAX * sizeof(char) + PCSlayerInfo::getMaxSize() * SLOT_MAX; 
	}
	
};


//////////////////////////////////////////////////////////////////////
//
// class LCPCListHandler;
//
//////////////////////////////////////////////////////////////////////

class LCPCListHandler {

public :

	// execute packet's handler
	static void execute ( LCPCList * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif

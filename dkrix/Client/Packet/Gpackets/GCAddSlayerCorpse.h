//----------------------------------------------------------------------
// 
// Filename    : GCAddSlayerCorpse.h 
// Written By  : Reiot
// 
//----------------------------------------------------------------------

#ifndef __GC_ADD_SLAYER_CORPSE_H__
#define __GC_ADD_SLAYER_CORPSE_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"
#include "../PCSlayerInfo3.h"

//----------------------------------------------------------------------
//
// class GCAddSlayerCorpse;
//

//
//----------------------------------------------------------------------

class GCAddSlayerCorpse : public Packet {

public :

	// constructor
	GCAddSlayerCorpse () throw () { m_TreasureCount = 0; }
	GCAddSlayerCorpse ( const PCSlayerInfo3 & slayerInfo ) throw () : m_SlayerInfo(slayerInfo) {}


public :

    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_ADD_SLAYER_CORPSE; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () { return m_SlayerInfo.getSize() + szBYTE; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCAddSlayerCorpse"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif


public :

	// get slayer info
	PCSlayerInfo3 & getSlayerInfo () throw () { return m_SlayerInfo; }
	const PCSlayerInfo3 & getSlayerInfo () const throw () { return m_SlayerInfo; }
	void setSlayerInfo ( const PCSlayerInfo3 & slayerInfo ) throw () { m_SlayerInfo = slayerInfo; }

	// get/set Treasure Count
	BYTE getTreasureCount() const throw() { return m_TreasureCount; }
	void setTreasureCount( BYTE Count ) throw() { m_TreasureCount = Count; }

private :
	
	
	PCSlayerInfo3 m_SlayerInfo;

	
	BYTE m_TreasureCount;

};


//--------------------------------------------------------------------------------
//
// class GCAddSlayerCorpseFactory;
//
// Factory for GCAddSlayerCorpse
//
//--------------------------------------------------------------------------------

class GCAddSlayerCorpseFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCAddSlayerCorpse(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCAddSlayerCorpse"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_ADD_SLAYER_CORPSE; }

	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw ()
	{ 
		return PCSlayerInfo3::getMaxSize() + szBYTE;
	}

};


//--------------------------------------------------------------------------------
//
// class GCAddSlayerCorpseHandler;
//
//--------------------------------------------------------------------------------

class GCAddSlayerCorpseHandler {

public :

	// execute packet's handler
	static void execute ( GCAddSlayerCorpse * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif

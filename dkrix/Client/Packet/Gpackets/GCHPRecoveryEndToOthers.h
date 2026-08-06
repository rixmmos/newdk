//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCHPRecoveryEndToOthers.h 
// Written By  : Reiot
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_HP_RECOVERY_END_TO_OTHERS_H__
#define __GC_HP_RECOVERY_END_TO_OTHERS_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"
#include "../EffectInfo.h"


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryEndToOthers;
//
////////////////////////////////////////////////////////////////////

class GCHPRecoveryEndToOthers : public Packet {

public :

	GCHPRecoveryEndToOthers();

	virtual ~GCHPRecoveryEndToOthers();
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_HP_RECOVERY_END_TO_OTHERS; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () 
	{ 
		return szObjectID + szHP; 
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCHPRecoveryEndToOthers"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

public :

	// get/set creature ID 
	ObjectID_t getObjectID () const throw () { return m_ObjectID; }
	void setObjectID ( ObjectID_t creatureID ) throw () { m_ObjectID = creatureID; }

	// get /set CurrentHP
	HP_t getCurrentHP() const throw() { return m_CurrentHP; }
	void setCurrentHP( HP_t CurrentHP ) throw() { m_CurrentHP = CurrentHP; }

private :
	
    
    ObjectID_t m_ObjectID;

	
	HP_t m_CurrentHP;

};


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryEndToOthersFactory;
//
// Factory for GCHPRecoveryEndToOthers
//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryEndToOthersFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCHPRecoveryEndToOthers(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCHPRecoveryEndToOthers"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_HP_RECOVERY_END_TO_OTHERS; }

	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () 
	{ 
		return szObjectID + szHP; 
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryEndToOthersHandler;
//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryEndToOthersHandler {

public :

	// execute packet's handler
	static void execute ( GCHPRecoveryEndToOthers * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif

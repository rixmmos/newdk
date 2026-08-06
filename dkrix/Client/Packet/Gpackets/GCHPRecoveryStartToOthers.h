//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCHPRecoveryStartToOthers.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_HP_RECOVERY_START_TO_OTHERS_H__
#define __GC_HP_RECOVERY_START_TO_OTHERS_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryStartToOthers;
//

//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryStartToOthers : public Packet {

public :
	
	// constructor
	GCHPRecoveryStartToOthers () throw ();
	
	// destructor
	~GCHPRecoveryStartToOthers () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_HP_RECOVERY_START_TO_OTHERS; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + szBYTE + szHP + szHP; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCHPRecoveryStartToOthers"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set ObjectID
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

	// get / set Delay
	BYTE getDelay() const throw() { return m_Delay; }
	void setDelay( BYTE Delay ) throw() { m_Delay = Delay; }

	// get / set Period
	HP_t getPeriod() const throw() { return m_Period; }
	void setPeriod( HP_t Period ) throw() { m_Period = Period; }

	// get / set Quantity
	HP_t getQuantity() const throw() { return m_Quantity; }
	void setQuantity( HP_t Quantity ) throw() { m_Quantity = Quantity; }

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

	
	BYTE m_Delay;

	
	HP_t m_Period;

	
	HP_t m_Quantity;


};


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryStartToOthersFactory;
//
// Factory for GCHPRecoveryStartToOthers
//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryStartToOthersFactory : public PacketFactory {

public :
	
	// constructor
	GCHPRecoveryStartToOthersFactory () throw () {}
	
	// destructor
	virtual ~GCHPRecoveryStartToOthersFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCHPRecoveryStartToOthers(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCHPRecoveryStartToOthers"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_HP_RECOVERY_START_TO_OTHERS; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szBYTE + szHP + szHP; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCHPRecoveryStartToOthersHandler;
//
//////////////////////////////////////////////////////////////////////

class GCHPRecoveryStartToOthersHandler {

public :

	// execute packet's handler
	static void execute ( GCHPRecoveryStartToOthers * pGCHPRecoveryStartToOthers , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif

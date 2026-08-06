//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCMPRecoveryStart.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_MP_RECOVERY_START_H__
#define __GC_MP_RECOVERY_START_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCMPRecoveryStart;
//

//
//////////////////////////////////////////////////////////////////////

class GCMPRecoveryStart : public Packet {

public :
	
	// constructor
	GCMPRecoveryStart () throw ();
	
	// destructor
	~GCMPRecoveryStart () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_MP_RECOVERY_START; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szBYTE + szMP + szMP; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCMPRecoveryStart"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set Delay
	BYTE getDelay() const throw() { return m_Delay; }
	void setDelay( BYTE Delay ) throw() { m_Delay = Delay; }

	// get / set Period
	MP_t getPeriod() const throw() { return m_Period; }
	void setPeriod( MP_t Period ) throw() { m_Period = Period; }

	// get / set Quantity
	MP_t getQuantity() const throw() { return m_Quantity; }
	void setQuantity( MP_t Quantity ) throw() { m_Quantity = Quantity; }

private :
	
	
	BYTE m_Delay;

	
	MP_t m_Period;

	
	MP_t m_Quantity;


};


//////////////////////////////////////////////////////////////////////
//
// class GCMPRecoveryStartFactory;
//
// Factory for GCMPRecoveryStart
//
//////////////////////////////////////////////////////////////////////

class GCMPRecoveryStartFactory : public PacketFactory {

public :
	
	// constructor
	GCMPRecoveryStartFactory () throw () {}
	
	// destructor
	virtual ~GCMPRecoveryStartFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCMPRecoveryStart(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCMPRecoveryStart"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_MP_RECOVERY_START; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szBYTE + szMP + szMP; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCMPRecoveryStartHandler;
//
//////////////////////////////////////////////////////////////////////

class GCMPRecoveryStartHandler {

public :

	// execute packet's handler
	static void execute ( GCMPRecoveryStart * pGCMPRecoveryStart , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif

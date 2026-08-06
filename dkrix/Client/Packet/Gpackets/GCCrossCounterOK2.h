//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCCrossCounterOK2.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_CROSS_COUNTER_OK_2_H__
#define __GC_CROSS_COUNTER_OK_2_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../ModifyInfo.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCCrossCounterOK2;
//

//
//////////////////////////////////////////////////////////////////////

class GCCrossCounterOK2 : public ModifyInfo {

public :
	
	// constructor
	GCCrossCounterOK2 () throw ();
	
	// destructor
	~GCCrossCounterOK2 () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_CROSS_COUNTER_OK_2; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szObjectID + ModifyInfo::getPacketSize() + szSkillType; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCCrossCounterOK2"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set CEffectID
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

	SkillType_t getSkillType() const throw() { return m_SkillType; }
	void setSkillType( SkillType_t SkillType ) throw() { m_SkillType = SkillType; }
	

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

	SkillType_t m_SkillType;

};


//////////////////////////////////////////////////////////////////////
//
// class GCCrossCounterOK2Factory;
//
// Factory for GCCrossCounterOK2
//
//////////////////////////////////////////////////////////////////////

class GCCrossCounterOK2Factory : public PacketFactory {

public :
	
	// constructor
	GCCrossCounterOK2Factory () throw () {}
	
	// destructor
	virtual ~GCCrossCounterOK2Factory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCCrossCounterOK2(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCCrossCounterOK2"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_CROSS_COUNTER_OK_2; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + ModifyInfo::getPacketMaxSize() + szSkillType; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCCrossCounterOK2Handler;
//
//////////////////////////////////////////////////////////////////////

class GCCrossCounterOK2Handler {

public :

	// execute packet's handler
	static void execute ( GCCrossCounterOK2 * pGCCrossCounterOK2 , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif

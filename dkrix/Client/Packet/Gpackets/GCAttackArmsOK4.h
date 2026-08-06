//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCAttackArmsOK4.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_ATTACK_ARMS_OK_4_H__
#define __GC_ATTACK_ARMS_OK_4_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCAttackArmsOK4;
//

//
//////////////////////////////////////////////////////////////////////

class GCAttackArmsOK4 : public Packet {

public :
	
	// constructor
	GCAttackArmsOK4 () throw ();
	
	// destructor
	~GCAttackArmsOK4 () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_ATTACK_ARMS_OK_4; }
	
	// get packet's body size
	
//	PacketSize_t getPacketSize () const throw () { return szObjectID + szObjectID + szbool; }
	PacketSize_t getPacketSize () const throw () { return szSkillType + szObjectID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCAttackArmsOK4"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get / set ObjectID
//	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
//	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

	// get / set ObjectID
	ObjectID_t getTargetObjectID() const throw() { return m_TargetObjectID; }
	void setTargetObjectID( ObjectID_t TargetObjectID ) throw() { m_TargetObjectID = TargetObjectID; }
//	Coord_t getTargetX() const throw() { return m_X;}
//	Coord_t getTargetY() const throw() { return m_Y;}
//	void setTargetXY( Coord_t X, Coord_t Y) throw() { m_X = X; m_Y = Y;}

	// get / set success
//	bool getSkillSuccess() const throw() { return m_bSuccess;}
//	void setSkillSuccess( bool bSuccess) throw() { m_bSuccess = bSuccess;}

	void setSkillType( SkillType_t		SkillType ) { m_SkillType = SkillType; }
	SkillType_t getSkillType() const throw() { return m_SkillType; }		

private :
	
	// ObjectID
//	ObjectID_t m_ObjectID;
//	Coord_t m_X, m_Y;

	// TargetObjectID
	ObjectID_t m_TargetObjectID;
	
	SkillType_t m_SkillType;

	// bool
//	bool m_bSuccess;

};


//////////////////////////////////////////////////////////////////////
//
// class GCAttackArmsOK4Factory;
//
// Factory for GCAttackArmsOK4
//
//////////////////////////////////////////////////////////////////////

class GCAttackArmsOK4Factory : public PacketFactory {

public :
	
	// constructor
	GCAttackArmsOK4Factory () throw () {}
	
	// destructor
	virtual ~GCAttackArmsOK4Factory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCAttackArmsOK4(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCAttackArmsOK4"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_ATTACK_ARMS_OK_4; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szSkillType+szObjectID; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCAttackArmsOK4Handler;
//
//////////////////////////////////////////////////////////////////////

class GCAttackArmsOK4Handler {

public :

	// execute packet's handler
	static void execute ( GCAttackArmsOK4 * pGCAttackArmsOK4 , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif

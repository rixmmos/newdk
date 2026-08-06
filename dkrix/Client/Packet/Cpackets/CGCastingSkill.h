//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGCastingSkill.h 
// Written By  : elca@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_CASTING_SKILL_H__
#define __CG_CASTING_SKILL_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGCastingSkill;
//
//////////////////////////////////////////////////////////////////////

class CGCastingSkill : public Packet {

public :
	
	// constructor
	CGCastingSkill () throw ();
	
	// destructor
	~CGCastingSkill () throw ();

	
public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CG_CASTING_SKILL; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () { return szSkillType; }

	// get packet name
	std::string getPacketName () const throw () { return "CGCastingSkill"; }

	// get/set SkillType
	SkillType_t getSkillType() const throw()  { return m_SkillType; }
	void setObjectID( SkillType_t SkillType ) throw() { m_SkillType = SkillType; }

	// get packet's debug std::string
	std::string toString () const throw ();

private :

	// SkillType
	SkillType_t m_SkillType;

};


//////////////////////////////////////////////////////////////////////
//
// class CGCastingSkillFactory;
//
// Factory for CGCastingSkill
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGCastingSkillFactory : public PacketFactory {

public :
	
	// constructor
	CGCastingSkillFactory () throw () {}
	
	// destructor
	virtual ~CGCastingSkillFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new CGCastingSkill(); }

	// get packet name
	std::string getPacketName () const throw () { return "CGCastingSkill"; }
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_CASTING_SKILL; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szSkillType; }

};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CGCastingSkillHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGCastingSkillHandler {

public :

	// execute packet's handler
	static void execute ( CGCastingSkill * pCGCastingSkill , Player * pPlayer ) throw ( ProtocolException , Error );

};
#endif

#endif
